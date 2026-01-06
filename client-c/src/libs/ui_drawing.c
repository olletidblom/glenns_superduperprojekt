#include "ui_drawing.h"
#include "termbox2.h"
#include "ui.h"
#include "ui_constants.h"

void draw_box(int width, int height, int offset_x, int offset_y) {
  tb_set_cell(offset_x, offset_y, LEFT_TOP_CORNER, TB_WHITE, TB_DEFAULT);
  PRINT_LINE(offset_x + 1, offset_y, width);
  tb_set_cell(offset_x + width, offset_y, RIGHT_TOP_CORNER, TB_WHITE,
              TB_DEFAULT);
  for (int i = 0; i < height; i++) {
    tb_set_cell(offset_x, offset_y + i + 1, HORIZONTAL_LINE, TB_WHITE,
                TB_DEFAULT);
    tb_set_cell(offset_x + width, offset_y + i + 1, HORIZONTAL_LINE, TB_WHITE,
                TB_DEFAULT);
  }
  tb_set_cell(offset_x, offset_y + height + 1, LEFT_BOTTOM_CORNER, TB_WHITE,
              TB_DEFAULT);
  PRINT_LINE(offset_x + 1, offset_y + height + 1, width);
  tb_set_cell(offset_x + width, offset_y + height + 1, RIGHT_BOTTOM_CORNER,
              TB_WHITE, TB_DEFAULT);
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
