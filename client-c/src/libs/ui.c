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


LinkedList *city_data = NULL;

City* search_data = NULL;
int search_number = 0;


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
  for (int i = 0; i < 25 && i < h - 2; i++) {
    uint16_t fg = TB_WHITE;
    uint16_t bg = TB_DEFAULT;
    if (i == selected_index) {
      fg = TB_BLACK;
      bg = TB_WHITE;
    }
    tb_set_cell(BOX_TWO_OFFSET, i + 3, HORIZONTAL_LINE, TB_WHITE, TB_DEFAULT);
    //if (i < filtered_count) {
    //  const char *text = filtered[i];
    //  draw_text(BOX_TWO_OFFSET + 2, i + 3, BOX_TWO_WIDTH, text, fg, bg);
    //}
    tb_set_cell(BOX_TWO_OFFSET + BOX_TWO_WIDTH, i + 3, HORIZONTAL_LINE,
                TB_WHITE, TB_DEFAULT);
  }
  PRINT_LINE_WITH_CORNER(BOX_TWO_OFFSET, 28, BOX_TWO_WIDTH, LEFT_BOTTOM_CORNER,
                         UP_HORIZONTAL);

  char input_line[MAX_SEARCH + 10];
  snprintf(input_line, sizeof(input_line), "Search: %s", search_text);
  tb_set_cell(BOX_TWO_OFFSET, 1, HORIZONTAL_LINE, TB_WHITE, TB_DEFAULT);
  draw_text(BOX_TWO_OFFSET + 2, 1, BOX_TWO_WIDTH, input_line, TB_GREEN,
            TB_DEFAULT);

  // Set cursor position
  tb_set_cursor(11 + cursor_pos, 2 - 1);
}

void render_box_three() {

  // int w = tb_width();
  int h = tb_height();

  PRINT_LINE_WITH_CORNER(BOX_THREE_OFFSET, 0, BOX_THREE_WIDTH, VERTICAL_LINE,
                         RIGHT_TOP_CORNER);
  tb_set_cell(BOX_THREE_OFFSET + BOX_THREE_WIDTH, 1, HORIZONTAL_LINE, TB_WHITE, TB_DEFAULT);
  PRINT_LINE_WITH_CORNER(BOX_THREE_OFFSET, 2, BOX_THREE_WIDTH, VERTICAL_LINE,
                         VERTICAL_LEFT);

  for (int i = 0; i < 25 && i < h - 2; i++) {
    tb_set_cell(BOX_THREE_OFFSET + BOX_THREE_WIDTH, i + 3, HORIZONTAL_LINE,
                TB_WHITE, TB_DEFAULT);
    if (i < search_number) {

    }
  }
  PRINT_LINE_WITH_CORNER(BOX_THREE_OFFSET, 28, BOX_THREE_WIDTH, VERTICAL_LINE,
                         RIGHT_BOTTOM_CORNER);
}

void render() {
  if (search_data && selected_index < search_number) {
    g_city_data(search_data[selected_index].latitude, search_data[selected_index].longitude);
  }

  tb_clear();
  render_box_two();
  render_box_three();

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
  for(int i = 0; i < search_number; i++) {
    unsigned short int fg = selected_index == i ? TB_BLACK : TB_WHITE;
    unsigned short int bg = selected_index == i ? TB_GREEN : TB_DEFAULT;
    draw_text(3, i+3, 30, search_data[i].name, fg, bg);
    //if(search_data && search_data[i].name != NULL) {
    //  char buf[200];
    //  snprintf(buf, 199, "%f & %f", search_data[i].latitude, search_data[i].longitude);
    //  draw_text(3, i+3, 30, buf, fg, bg);
    //  char buff[200];
    //  snprintf(buff, 199, "%d", selected_index);
    //  draw_text(3, 37, 30,  buff, fg, bg);
    //}
  }
  tb_present();
}

void handle_input(struct tb_event *ev) {
    if (ev->type != TB_EVENT_KEY)
        return;

    if (ev->key == TB_KEY_ESC) {
        tb_shutdown();
        exit(0);
    }

    if (ev->key == TB_KEY_BACKSPACE || ev->key == TB_KEY_BACKSPACE2) {
        if (cursor_pos > 0) {

            /* Find start of previous UTF-8 character */
            int prev_pos = cursor_pos - 1;
            while (prev_pos > 0 &&
                  ((unsigned char)search_text[prev_pos] & 0xC0) == 0x80) {
                prev_pos--;
            }

            /*
             * Remove bytes [prev_pos .. cursor_pos)
             * by shifting the tail of the string left
             */
            memmove(
                &search_text[prev_pos],
                &search_text[cursor_pos],
                strlen(search_text) - cursor_pos + 1 /* +1 copies '\0' */
            );

            cursor_pos = prev_pos;
        }
    }

    else if (ev->key == TB_KEY_ARROW_UP) {
        if (selected_index > 0)
            selected_index--;
    }
    else if (ev->key == TB_KEY_ARROW_DOWN) {
        if (selected_index < search_number-1)
            selected_index++;
    }

    else if (ev->key == TB_KEY_ENTER) {
        g_search_city(search_text);
    }

    else if (ev->ch) {
        char utf8_buf[5] = {0};
        int utf8_len = tb_utf8_unicode_to_char(utf8_buf, ev->ch);
        size_t current_len = strlen(search_text);

        /* Ensure space in buffer */
        if (current_len + utf8_len < MAX_SEARCH - 1) {

            /*
             * Make room for new UTF-8 bytes by shifting
             * everything from cursor_pos to the right
             */
            memmove(
                &search_text[cursor_pos + utf8_len],
                &search_text[cursor_pos],
                current_len - cursor_pos + 1 /* include '\0' */
            );

            /* Insert the UTF-8 bytes */
            memcpy(&search_text[cursor_pos], utf8_buf, utf8_len);

            cursor_pos += utf8_len;
        }
    }
}

void start_ui(ui_city_selection_update get_city_data, ui_search_city_update search_city) {
  g_city_data = get_city_data;
  g_search_city = search_city;

  if (tb_init() != 0) {
    fprintf(stderr, "Failed to initialize termbox\n");
  }

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
  search_number = number_of_cities;
  search_data = cities;
  selected_index = 0;
  if (search_data) {
    g_city_data(search_data[selected_index].latitude, search_data[selected_index].longitude);
  }

}

void ui_add_city_data(LinkedList *city_data_in) {
  city_data = city_data_in;
}
