// #ifndef _GNU_SOURCE
// #define _GNU_SOURCE
// #endif
#define TB_IMPL
#include "ui.h"

#include "termbox2.h"
#include "utils/utils.h"
#include <bits/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ITEMS 100
#define MAX_SEARCH 256

#define BOX_ONE_WIDTH 30
#define BOX_TWO_WIDTH 30
#define BOX_THREE_WIDTH 50
#define BOX_ONE_OFFSET 1
#define BOX_TWO_OFFSET 1
//(BOX_ONE_WIDTH + BOX_ONE_OFFSET)
#define BOX_THREE_OFFSET (BOX_TWO_OFFSET + BOX_TWO_WIDTH + 1)
//---- BOX CHARACTERS ----
#define LEFT_TOP_CORNER 0x256D
#define RIGHT_TOP_CORNER 0x256E
#define LEFT_BOTTOM_CORNER 0x2570
#define RIGHT_BOTTOM_CORNER 0x256F

#define VERTICAL_LINE 0x2500
#define HORIZONTAL_LINE 0x2502

#define DOWN_HORIZONTAL 0x252C
#define VERTICAL_RIGHT 0x251C
#define VERTICAL_LEFT 0x2524
#define UP_HORIZONTAL 0x2534

#define CROSS 0x253C
//------------------------

static ui_city_selection_update g_city_data = NULL;
static ui_search_city_update g_search_city = NULL;

int selected_index = 0;
char search_text[MAX_SEARCH] = "";
int cursor_pos = 0;
const int search_delay_ms = 300;
int search_needs_update = 0;

// Filtered list
const char *filtered[MAX_ITEMS];
int filtered_count = 0;

// List of cities
char **list_of_cities = NULL; // dynamic array of string pointers
size_t cities_count = 0;      // how many strings
size_t cities_capacity = 0;

LinkedList *city_data = NULL;

City* search_data = NULL;
int search_number = 0;

void draw_box(int width, int height, int offset_x, int offset_y) {
  tb_set_cell(offset_x, offset_y, LEFT_TOP_CORNER, TB_WHITE, TB_DEFAULT);
  PRINT_LINE(offset_x + 1, offset_y, width);
  tb_set_cell(offset_x + width, offset_y, RIGHT_TOP_CORNER, TB_WHITE, TB_DEFAULT);
  for(int i = 1; i < height; i++) {
    tb_set_cell(offset_x, offset_y+i, HORIZONTAL_LINE, TB_WHITE, TB_DEFAULT);
    tb_set_cell(offset_x+width, offset_y+i, HORIZONTAL_LINE, TB_WHITE, TB_DEFAULT);
  }
  tb_set_cell(offset_x, offset_y+height, LEFT_BOTTOM_CORNER, TB_WHITE, TB_DEFAULT);
  PRINT_LINE(offset_x + 1, offset_y+height, width);
  tb_set_cell(offset_x + width, offset_y+height, RIGHT_BOTTOM_CORNER, TB_WHITE, TB_DEFAULT);
}

int add_city(const char *name) {
  if (cities_count == cities_capacity) {
    size_t newcap = cities_capacity ? cities_capacity * 2 : 4;
    char **tmp = realloc(list_of_cities, newcap * sizeof *list_of_cities);
    if (!tmp) {
      return -1;
    }
    list_of_cities = tmp;
    cities_capacity = newcap;
  }

  // TODO: Add check for name already exists

  list_of_cities[cities_count] = strdup(name);
  if (!list_of_cities[cities_count]) {
    return -1;
  }
  cities_count++;
  return 0;
}

void free_cities(void) {
  for (size_t i = 0; i < cities_count; i++) {
    free(list_of_cities[i]);
  }
  free(list_of_cities);
  list_of_cities = NULL;
  cities_count = 0;
  cities_capacity = 0;
}

void filter_items() {
  filtered_count = 0;
  for (size_t i = 0; i < cities_count; i++) {
    if (strcasestr(list_of_cities[i], search_text)) {
      filtered[filtered_count++] = list_of_cities[i];
    }
  }
  if (selected_index >= filtered_count) {
    selected_index = filtered_count - 1;
  }
  if (selected_index < 0)
    selected_index = 0;
}

void draw_text(int x, int y, int width, const char *text, uint16_t fg,
               uint16_t bg) {
  uint32_t c;
  int index = 0;
  while (*text && index++ < width - 4) {
    const int w = tb_utf8_char_to_unicode(&c, text);
    tb_set_cell(x++, y, c, fg, bg);
    text += w;
  }
  while (index++ < width - 3) {
    tb_set_cell(x++, y, ' ', fg, bg);
  }
}

void render_box_one() {}

void render_box_two() {

  // int w = tb_width();
  const int h = tb_height();

  PRINT_LINE_WITH_CORNER(BOX_TWO_OFFSET, 0, BOX_TWO_WIDTH, LEFT_TOP_CORNER,
                         VERTICAL_LINE);
  PRINT_LINE_WITH_CORNER(BOX_TWO_OFFSET, 2, BOX_TWO_WIDTH, VERTICAL_RIGHT,
                         DOWN_HORIZONTAL);
  //  Display filtered list
  for (int i = 0; (i < filtered_count || i < 25) && i < h - 2; i++) {
    uint16_t fg = TB_WHITE;
    uint16_t bg = TB_DEFAULT;
    if (i == selected_index) {
      fg = TB_BLACK;
      bg = TB_WHITE;
    }
    tb_set_cell(BOX_TWO_OFFSET, i + 3, HORIZONTAL_LINE, TB_WHITE, TB_DEFAULT);
    if (i < filtered_count) {
      const char *text = filtered[i];
      draw_text(BOX_TWO_OFFSET + 2, i + 3, BOX_TWO_WIDTH, text, fg, bg);
    }
    tb_set_cell(BOX_TWO_OFFSET + BOX_TWO_WIDTH, i + 3, HORIZONTAL_LINE,
                TB_WHITE, TB_DEFAULT);
  }
  PRINT_LINE_WITH_CORNER(BOX_TWO_OFFSET, 28, BOX_TWO_WIDTH, LEFT_BOTTOM_CORNER,
                         UP_HORIZONTAL);

  // Draw input line
  char input_line[MAX_SEARCH + 10];
  snprintf(input_line, sizeof(input_line), "Search: %s", search_text);
  tb_set_cell(BOX_TWO_OFFSET, 1, HORIZONTAL_LINE, TB_WHITE, TB_DEFAULT);
  draw_text(BOX_TWO_OFFSET + 2, 1, BOX_TWO_WIDTH, input_line, TB_GREEN,
            TB_DEFAULT);

  // Set cursor position
  tb_set_cursor(10 + cursor_pos, 2 - 1);
}

void render_box_three() {

  // int w = tb_width();
  int h = tb_height();

  PRINT_LINE_WITH_CORNER(BOX_THREE_OFFSET, 0, BOX_THREE_WIDTH, VERTICAL_LINE,
                         RIGHT_TOP_CORNER);
  tb_set_cell(BOX_THREE_OFFSET + BOX_THREE_WIDTH, 1, HORIZONTAL_LINE, TB_WHITE, TB_DEFAULT);
  PRINT_LINE_WITH_CORNER(BOX_THREE_OFFSET, 2, BOX_THREE_WIDTH, VERTICAL_LINE,
                         VERTICAL_LEFT);

  for (int i = 0; (i < filtered_count || i < 25) && i < h - 2; i++) {
    tb_set_cell(BOX_THREE_OFFSET + BOX_THREE_WIDTH, i + 3, HORIZONTAL_LINE,
                TB_WHITE, TB_DEFAULT);
    if (i < search_number) {

    }
  }
  PRINT_LINE_WITH_CORNER(BOX_THREE_OFFSET, 28, BOX_THREE_WIDTH, VERTICAL_LINE,
                         RIGHT_BOTTOM_CORNER);
}

void render() {
  tb_clear();
  render_box_two();
  render_box_three();

  draw_box(30, 30, 90, 0);

  KeyValuePair *pair = NULL;
  if (city_data != NULL) {
    int offset_x = 0;
    LinkedList_ForEach(city_data, &pair) {
      char toPrint[BOX_THREE_WIDTH-2];
      char temp[16];
      snprintf(temp, sizeof(temp), "%s:", pair->key);
      snprintf(toPrint, sizeof(toPrint), "%-17.17s%s", temp, pair->value);
      draw_text(BOX_THREE_OFFSET + 1, 3 + offset_x++, BOX_THREE_WIDTH - 1, toPrint, TB_WHITE,
                       TB_DEFAULT);
    }
  }

  tb_present();
}

void handle_input(struct tb_event *ev) {
  if (ev->type == TB_EVENT_KEY) {
    if (ev->key == TB_KEY_ESC) {
      tb_shutdown();
      exit(0);
    }
    if (ev->key == TB_KEY_BACKSPACE || ev->key == TB_KEY_BACKSPACE2) {
      if (cursor_pos > 0) {
        int prev_pos = cursor_pos - 1;
        while (prev_pos > 0 && (search_text[prev_pos] & 0xC0) == 0x80) {
          prev_pos--;
        }

        // int char_len = cursor_pos - prev_pos;

        // Remove the character
        memmove(&search_text[prev_pos], &search_text[cursor_pos],
                strlen(search_text) - cursor_pos + 1);

        cursor_pos = prev_pos;
      }
    } else if (ev->key == TB_KEY_ARROW_UP) {
      if (selected_index > 0)
        selected_index--;
    } else if (ev->key == TB_KEY_ARROW_DOWN) {
      if (selected_index < filtered_count - 1)
        selected_index++;
    } else if (ev->key == TB_KEY_ENTER) {
      g_search_city(search_text);
    } else if (ev->ch) {
      char utf8_buf[5] = {0};
      int utf8_len = tb_utf8_unicode_to_char(utf8_buf, ev->ch);
      size_t current_len = strlen(search_text);

      // Check if there's enough space
      if (current_len + utf8_len < MAX_SEARCH - 1) {
        // Make room for the new character(s)
        memmove(&search_text[cursor_pos + utf8_len], &search_text[cursor_pos],
                current_len - cursor_pos + 1); // +1 for null terminator

        // Copy the UTF-8 bytes
        memcpy(&search_text[cursor_pos], utf8_buf, utf8_len);
        cursor_pos += utf8_len;
      }
    }
    filter_items();
    g_city_data(filtered[selected_index]);
  }
}

void start_ui(ui_city_selection_update get_city_data, ui_search_city_update search_city) {
  g_city_data = get_city_data;
  g_search_city = search_city;

  if (tb_init() != 0) {
    fprintf(stderr, "Failed to initialize termbox\n");
  }

  filter_items();
  render();

  struct tb_event ev;

  while (1) {
    tb_poll_event(&ev);
    handle_input(&ev);
    render();
  }

  tb_shutdown();
}

void ui_add_search_city_data(City* cities, int number_of_cities) {
  search_data = cities;
}

void ui_add_city_data(LinkedList *city_data_in) {
  city_data = city_data_in;
}
