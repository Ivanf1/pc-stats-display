#ifndef PCSTATS_DRAWING_DRAWING_H_
#define PCSTATS_DRAWING_DRAWING_H_

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>

enum DrawingType: int {
  ARC,
  LINE
};

void drawArcStats(TFT_eSPI &tft, uint16_t min_x, uint16_t max_x, uint16_t min_y, uint16_t max_y, const char* hw_name, const JsonArray &sensors);
void drawLineStats(TFT_eSPI &tft, uint16_t min_x, uint16_t max_x, uint16_t min_y, uint16_t max_y, const char* hw_name, const JsonArray &sensors);
uint16_t hslToRgb(uint16_t h, uint16_t s, uint16_t l);
int hsl2rgb(float h, float s, float l);

#endif