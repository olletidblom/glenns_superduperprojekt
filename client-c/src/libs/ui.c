#define TB_IMPL
#include "ui.h"

#include "termbox2.h"
#include "utils/utils.h"
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ui_constants.h"
#include "ui_drawing.h"

static ui_city_selection_update g_city_data = NULL;
static ui_search_city_update g_search_city = NULL;

int selected_index = 0;
int scroll_offset = 0;
int running = 1;
char search_text[MAX_SEARCH] = "";
int cursor_pos = 0;
int cursor_visual_pos = 0;
const int search_delay_ms = 300;
int search_needs_update = 0;

LinkedList *city_data = NULL;

City *search_data = NULL;
int search_number = 0;

City **filtered_cities = NULL;
int filtered_count = 0;
int filtered_capacity = 0;

void filter_cities(void) {
  if (!search_data || search_number == 0) {
    filtered_count = 0;
    return;
  }

  if (!filtered_cities || filtered_capacity < search_number) {
    filtered_capacity = search_number;
    filtered_cities =
        realloc(filtered_cities, filtered_capacity * sizeof(City *));
  }

  filtered_count = 0;

  for (int i = 0; i < search_number; i++) {
    if (strcasestr(search_data[i].name, search_text)) {
      filtered_cities[filtered_count++] = &search_data[i];
    }
  }

  selected_index = 0;
  scroll_offset = 0;
}

void render_outline() { draw_box(82, 25, 0, 0); }

void render_box_one() {
  draw_box(80, 1, 1, 1);

  char input_line[MAX_SEARCH + 10];
  snprintf(input_line, sizeof(input_line), "Search: %s", search_text);
  draw_text(3, 2, 60, input_line, TB_GREEN, TB_DEFAULT);

  tb_set_cursor(11 + cursor_visual_pos, 3 - 1);
}

void render_box_two() {
  draw_box(29, 20, 1, 4);

  int visible_start = scroll_offset;
  int visible_end = scroll_offset + BOX_TWO_HEIGHT;
  if (visible_end > filtered_count) {
    visible_end = filtered_count;
  }

  for (int i = visible_start; i < visible_end; i++) {
    unsigned short int fg = selected_index == i ? TB_BLACK : TB_WHITE;
    unsigned short int bg = selected_index == i ? TB_GREEN : TB_DEFAULT;

    int y_pos = (i - scroll_offset) + 5;
    draw_text(3, y_pos, 25, filtered_cities[i]->name, fg, bg);
  }
}

void render_box_three() {
  draw_box(49, 20, 32, 4);
  KeyValuePair *pair = NULL;
  if (city_data != NULL) {
    int offset_x = 0;
    LinkedList_ForEach(city_data, &pair) {
      char toPrint[BOX_THREE_WIDTH - 2];
      char temp[16];
      snprintf(temp, sizeof(temp), "%s:", pair->key);
      snprintf(toPrint, sizeof(toPrint), "%-17.17s%s", temp, pair->value);
      draw_text(33 + 1, 5 + offset_x++, BOX_THREE_WIDTH - 1, toPrint, TB_WHITE,
                TB_DEFAULT);
    }
  }
}

void render() {
  if (filtered_cities && selected_index < filtered_count) {
    g_city_data(filtered_cities[selected_index]->latitude,
                filtered_cities[selected_index]->longitude);
  }

  tb_clear();

  render_outline();
  render_box_one();
  render_box_two();
  render_box_three();

  tb_present();
}

void handle_input(struct tb_event *ev) {
  if (ev->type != TB_EVENT_KEY)
    return;

  if (ev->key == TB_KEY_ESC) {
    running = 0;
  }

  if (ev->key == TB_KEY_BACKSPACE || ev->key == TB_KEY_BACKSPACE2) {
    if (cursor_pos > 0) {

      int prev_pos = cursor_pos - 1;
      while (prev_pos > 0 &&
             ((unsigned char)search_text[prev_pos] & 0xC0) == 0x80) {
        prev_pos--;
      }

      memmove(&search_text[prev_pos], &search_text[cursor_pos],
              strlen(search_text) - cursor_pos + 1);

      cursor_pos = prev_pos;
      cursor_visual_pos--;
      filter_cities();
    }
  }

  else if (ev->key == TB_KEY_ARROW_UP) {
    if (selected_index > 0) {
      selected_index--;
      if (selected_index < scroll_offset) {
        scroll_offset = selected_index;
      }
    }
  } else if (ev->key == TB_KEY_ARROW_DOWN) {
    if (selected_index < filtered_count - 1) {
      selected_index++;
      if (selected_index >= scroll_offset + BOX_TWO_HEIGHT) {
        scroll_offset = selected_index - BOX_TWO_HEIGHT + 1;
      }
    }
  }

  else if (ev->key == TB_KEY_ENTER) {
    g_search_city(search_text);
  }

  else if (ev->ch) {
    char utf8_buf[5] = {0};
    int utf8_len = tb_utf8_unicode_to_char(utf8_buf, ev->ch);
    size_t current_len = strlen(search_text);

    if (current_len + utf8_len < MAX_SEARCH - 1) {

      memmove(&search_text[cursor_pos + utf8_len], &search_text[cursor_pos],
              current_len - cursor_pos + 1);

      memcpy(&search_text[cursor_pos], utf8_buf, utf8_len);

      cursor_pos += utf8_len;
      cursor_visual_pos++;
      filter_cities();
    }
  }
}

void start_ui(ui_city_selection_update get_city_data,
              ui_search_city_update search_city, City *initial_cities,
              int initial_count) {
  g_city_data = get_city_data;
  g_search_city = search_city;

  if (tb_init() != 0) {
    fprintf(stderr, "Failed to initialize termbox\n");
  }

  if (initial_cities != NULL && initial_count > 0) {
    ui_add_search_city_data(initial_cities, initial_count);
  }

  render();

  struct tb_event ev;

  while (running) {
    tb_poll_event(&ev);
    handle_input(&ev);
    render();
  }

  tb_shutdown();

  if (filtered_cities != NULL) {
    free(filtered_cities);
    filtered_cities = NULL;
    filtered_count = 0;
    filtered_capacity = 0;
  }

  if (city_data != NULL) {
    KeyValuePair *pair = NULL;
    while (city_data->length > 0) {
      pair = (KeyValuePair *)LinkedList_RemoveFirst(city_data);
      if (pair != NULL) {
        free(pair->key);
        free(pair->value);
        free(pair);
      }
    }
    free(city_data);
    city_data = NULL;
  }
}

void ui_add_search_city_data(City *cities, int number_of_cities) {
  search_number = number_of_cities;
  search_data = cities;

  filter_cities();

  if (filtered_cities && filtered_count > 0) {
    g_city_data(filtered_cities[0]->latitude, filtered_cities[0]->longitude);
  }
}

void ui_add_city_data(LinkedList *city_data_in) {
  if (city_data != NULL) {
    KeyValuePair *pair = NULL;
    while (city_data->length > 0) {
      pair = (KeyValuePair *)LinkedList_RemoveFirst(city_data);
      if (pair != NULL) {
        free(pair->key);
        free(pair->value);
        free(pair);
      }
    }
    free(city_data);
  }
  city_data = city_data_in;
}
