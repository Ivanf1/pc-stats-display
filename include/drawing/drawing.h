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

#endif