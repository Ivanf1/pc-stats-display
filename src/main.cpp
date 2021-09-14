#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>

#include "drawing/drawing.h"

#define ARC_STATS_HEIGHT 85
#define LINE_STATS_HEIGHT 70

#define DEFAULT_BG_COLOR TFT_BLACK

TFT_eSPI tft = TFT_eSPI();
StaticJsonDocument<1024> doc;

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.fillScreen(DEFAULT_BG_COLOR);

  tft.setTextColor(TFT_WHITE, DEFAULT_BG_COLOR);
}

void displayStats(JsonDocument &doc, TFT_eSPI &tft) {
  size_t num_hw = doc.size();
  uint16_t y_start = 0, y_max = 0;

  for (int i = 0; i < num_hw; i++) {
    y_start = y_max;
    const char* hw_name = doc[i]["n"].as<const char*>();
    int type = doc[i]["t"].as<int>();
    if (type == DrawingType::ARC) {
      y_max += ARC_STATS_HEIGHT;
      drawArcStats(tft, 0, 240, y_start, y_max, hw_name, doc[i]["s"]);
    } else if (type == DrawingType::LINE) {
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
      displayStats(doc, tft);
    }
  }
}