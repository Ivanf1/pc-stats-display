#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>

#include "drawing/drawing.h"

#define FONT_2_SIZE 16 // size of font #2 is 16 pixel

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.fillScreen(TFT_BLACK);
}

// TODO: map arc color to sensor value (e.g. (0 - 100) -> (green - yellow - orange - red))
void displayArcStat(TFT_eSPI &tft, uint16_t center_x, uint16_t min_y, uint16_t max_y, const char* label, float value, uint16_t arc_radius) {
  uint16_t y_space = (int)((max_y - min_y) - FONT_2_SIZE - arc_radius);
  // set text alignment middle center
  tft.setTextDatum(MC_DATUM);

  tft.drawString(label, center_x, min_y, 2);
  tft.drawFloat(value, 1, center_x, min_y + y_space, 2);

  // // map the arc length to the sensor value
  long arc_len = constrain(map(value, 0, 100, 0, 60), 0, 60);
  fillArc(tft, center_x, max_y, 270, arc_len, arc_radius, arc_radius, 8, rainbow(0));
}

// TODO: refactor this thing
void displayHardwareStats(TFT_eSPI &tft, uint16_t max_x, uint16_t min_y, uint16_t max_y, const char* hw_name, const char* s1, const float s1_val, const char* s2, const float s2_val) {
  uint16_t center_x = max_x / 2;
  uint16_t left_center_x = center_x / 2;
  uint16_t right_center_x = center_x + left_center_x;
  
  tft.setTextDatum(MC_DATUM);
  tft.drawString(hw_name, center_x, min_y + 10, 2);

  // reset screen to black before re-drawing
  // doing it separately causes less noticeable flickering
  tft.fillRect(0, min_y + 26, center_x, max_y, TFT_BLACK);
  displayArcStat(tft, left_center_x, min_y + 26, max_y, s1, s1_val, 26);

  tft.fillRect(center_x, min_y + 26, max_x, max_y, TFT_BLACK);
  displayArcStat(tft, right_center_x, min_y + 26, max_y, s2, s2_val, 26);
}

// TODO: refactor this thing
void displayStats(JsonDocument &doc, TFT_eSPI &tft) {
  const char* hw_name = doc[0]["n"].as<const char*>();
  const char* s1 = doc[0]["s"][0]["t"].as<const char*>();
  float s1_val = doc[0]["s"][0]["v"].as<float>();
  const char* s2 = doc[0]["s"][1]["t"].as<const char*>();
  float s2_val = doc[0]["s"][1]["v"].as<float>();

  displayHardwareStats(tft, 240, 0, 85, hw_name, s1, s1_val, s2, s2_val);

  hw_name = doc[1]["n"].as<const char*>();
  s1 = doc[1]["s"][0]["t"].as<const char*>();
  s1_val = doc[1]["s"][0]["v"].as<float>();
  s2 = doc[1]["s"][1]["t"].as<const char*>();
  s2_val = doc[1]["s"][1]["v"].as<float>();

  displayHardwareStats(tft, 240, 86, 171, hw_name, s1, s1_val, s2, s2_val);
}

void loop() {
  static uint8_t buffer[1024];
  static StaticJsonDocument<1024> doc;

  if (Serial.available()) {
    Serial.readBytesUntil('\n', buffer, sizeof(buffer));
    DeserializationError error = deserializeJson(doc, buffer);
    if (error) {
      Serial.println(error.c_str());
    } else {
      Serial.println("ok");
      displayStats(doc, tft);
    }
  }
}