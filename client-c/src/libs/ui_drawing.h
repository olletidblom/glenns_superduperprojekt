#ifndef UI_DRAWING_H
#define UI_DRAWING_H

#include <stdint.h>

void draw_box(int width, int height, int offset_x, int offset_y);
void draw_text(int x, int y, int width, const char *text, uint16_t fg,
               uint16_t bg);

#endif
