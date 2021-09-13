#ifndef PCSTATS_DRAWING_DRAWING_H_
#define PCSTATS_DRAWING_DRAWING_H_

#include <Arduino.h>
#include <TFT_eSPI.h>

// Segments are 3 degrees wide = 120 segments for 360 degrees
#define FILL_ARC_SEG 3

void fillArc(TFT_eSPI &tft, int x, int y, int start_angle, int seg_count, int rx, int ry, int w, uint32_t color);
uint16_t brightness(unsigned int color, int brightness);
uint16_t rainbow(byte value);

#endif