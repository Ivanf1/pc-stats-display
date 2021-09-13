#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>

#include "drawing/drawing.h"

#define FONT_2_SIZE 16 // size of font #2 is 16 pixel
#define TEXT_MARGINB 10
#define DEFAULT_BG_COLOR TFT_BLACK

#define ARC_STATS_HEIGHT 85
#define LINE_STATS_HEIGHT 70

TFT_eSPI tft = TFT_eSPI();
StaticJsonDocument<1024> doc;

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.fillScreen(DEFAULT_BG_COLOR);

  tft.setTextColor(TFT_WHITE, DEFAULT_BG_COLOR);
}

// TODO: map arc color to sensor value (e.g. (0 - 100) -> (green - yellow - orange - red))
void drawArcComponent(TFT_eSPI &tft, uint16_t center_x, uint16_t min_y, uint16_t max_y, const char* label, float value, uint16_t arc_radius) {
  uint16_t y_space = (max_y - min_y) - FONT_2_SIZE - arc_radius;

  tft.drawString(label, center_x, min_y, 2);
  char tx[10];
  sprintf(tx, "  %.1f  ", value);
  tft.drawString(tx, center_x, min_y + y_space, 2);

  // map the arc length to the value
  long arc_len = constrain(map(value, 0, 100, 1, 60), 1, 60);
  // When drawing the new arc, if the background has a different color than
  // the default background color (i.e. something else was previously drawn) that
  // color may still be visible. This problem could be resolved by drawing a
  // full-size default-background-colored arc, and then drawing the new arc, 
  // but this causes noticeable flickering.
  // Here, after drawing the new arc with the required length, we then draw a
  // default-background-colored arc from the new arc ends to the max_length.
  // This gets rid of anything that was left on the background without flickering.
  uint16_t end_angle = (270 + (arc_len * FILL_ARC_SEG)) % 360;
  fillArc(tft, center_x, max_y, 270, arc_len, arc_radius, arc_radius, 8, rainbow(0));
  fillArc(tft, center_x, max_y, end_angle, 60 - arc_len, arc_radius, arc_radius, 8, DEFAULT_BG_COLOR);
}

void drawLineComponent(TFT_eSPI &tft, uint16_t min_x, uint16_t max_x, uint16_t min_y, uint16_t max_y, const char* label, float value) {
  uint16_t y_space = (max_y - min_y) - FONT_2_SIZE - 8;
  uint16_t center_x = (max_x - min_x) / 2;

  tft.drawString(label, center_x, min_y, 2);
  char tx[10];
  sprintf(tx, "  %.1f  ", value);
  tft.drawString(tx, center_x, min_y + y_space, 2);

  // map the line length to the value
  long line_len = constrain(map(value, 0, 100, min_x, max_x), min_x, max_x);
  // When drawing the new rectangle, if the background has a different color than
  // the default background color (i.e. something else was previously drawn) that
  // color may still be visible. This problem could be resolved by drawing a
  // full-size default-background-colored rectangle, but this causes noticeable flickering.
  // Here, after drawing the new line with the required length, we then draw a
  // default-background-colored line from the new line ends to the max_x.
  // This gets rid of anything that was left on the background without flickering.
  tft.fillRect(min_x, max_y - 8, line_len, 8, rainbow(0));
  tft.fillRect(line_len, max_y - 8, max_x, 8, DEFAULT_BG_COLOR);
}

void drawArcStats(TFT_eSPI &tft, uint16_t min_x, uint16_t max_x, uint16_t min_y, uint16_t max_y, const char* hw_name, const JsonArray &sensors) {
  uint16_t center_x = max_x / 2;
  size_t num_sensors = sensors.size();
  uint16_t column_space = max_x / num_sensors;
  uint16_t sensor_center_x = column_space / num_sensors;

  tft.setTextDatum(MC_DATUM);

  tft.drawString(hw_name, center_x, min_y + TEXT_MARGINB, 2);

  for (uint8_t i = 0; i < num_sensors; i++) {
    uint16_t center = (column_space * i) + sensor_center_x;
    drawArcComponent(tft, center, min_y + TEXT_MARGINB + FONT_2_SIZE, max_y, sensors[i]["t"], sensors[i]["v"], 26);
  }
}

void drawLineStats(TFT_eSPI &tft, uint16_t min_x, uint16_t max_x, uint16_t min_y, uint16_t max_y, const char* hw_name, const JsonArray &sensors) {
  uint16_t center_x = max_x / 2;
  size_t num_sensors = sensors.size();

  tft.setTextDatum(MC_DATUM);

  tft.drawString(hw_name, center_x, min_y + TEXT_MARGINB, 2);

  for (uint8_t i = 0; i < num_sensors; i++) {
    drawLineComponent(tft, min_x, max_x, min_y + TEXT_MARGINB + FONT_2_SIZE, max_y, sensors[i]["t"], sensors[i]["v"]);
  }
}

void displayStats(JsonDocument &doc, TFT_eSPI &tft) {
  size_t num_hw = doc.size();
  uint16_t y_start = 0, y_max = 0;

  for (int i = 0; i < num_hw; i++) {
    y_start = y_max;
    const char* hw_name = doc[i]["n"].as<const char*>();
    int type = doc[i]["t"].as<int>();
    if (type == 0) {
      y_max += ARC_STATS_HEIGHT;
      drawArcStats(tft, 0, 240, y_start, y_max, hw_name, doc[i]["s"]);
    } else if (type == 1) {
      y_max += LINE_STATS_HEIGHT;
      drawLineStats(tft, 0, 240, y_start, y_max, hw_name, doc[i]["s"]);
    }
  }
}

void loop() {
  static uint8_t buffer[1024];

  if (Serial.available()) {
    Serial.readBytesUntil('\n', buffer, sizeof(buffer));
    DeserializationError error = deserializeJson(doc, buffer);
    if (error) {
      Serial.println(error.c_str());
    } else {
      Serial.println(doc.memoryUsage());
      displayStats(doc, tft);
    }
  }
}