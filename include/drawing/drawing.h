#ifndef PCSTATS_DRAWING_DRAWING_H_
#define PCSTATS_DRAWING_DRAWING_H_

#include <Arduino.h>
#include <TFT_eSPI.h>

void fillArc(TFT_eSPI &tft, int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour);
unsigned int brightness(unsigned int colour, int brightness);
unsigned int rainbow(byte value);

#endif