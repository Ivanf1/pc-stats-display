#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>

#include "drawing/drawing.h"

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  // fillArc(tft, 120, 120, 90, 60, 26, 26, 8, rainbow(0));
  static uint8_t buffer[1024];
    if (Serial.available()) {
      size_t read = Serial.readBytesUntil('\n', buffer, sizeof(buffer));
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, buffer);
      if (error) {
        Serial.println(error.c_str());
      } else {
        Serial.println(doc[0]["t"].as<const char*>());
        // Serial.println(read);
      }
    }
}