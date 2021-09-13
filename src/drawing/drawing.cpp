#include "drawing/drawing.h"

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>

#define DEG2RAD 0.0174532925

// Segments are 3 degrees wide = 120 segments for 360 degrees
#define FILL_ARC_SEG 3
#define FULL_CIRCLE_SEGMENTS 120
#define HALF_CIRCLE_SEGMENTS FULL_CIRCLE_SEGMENTS / 2

#define FONT_2_SIZE 16 // size of font #2 is 16 pixel
#define TEXT_MARGINB 10

#define ARC_COMPONENT_START_ANGLE 270

// Draw a circular or elliptical arc with a defined thickness
// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis outer radius
// ry = y axis outer radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same then an arc of a circle is drawn
void fillArc(TFT_eSPI &tft, int x, int y, int start_angle, int seg_count, int rx, int ry, int w, uint32_t color) {
  static uint8_t inc = 3; // Draw segments every 3 degrees

  // Calculate first pair of coordinates for segment start
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = sin((start_angle - 90) * DEG2RAD);
  uint16_t x0 = sx * (rx - w) + x;
  uint16_t y0 = sy * (ry - w) + y;
  uint16_t x1 = sx * rx + x;
  uint16_t y1 = sy * ry + y;

  // Draw colour blocks every inc degrees
  for (int i = start_angle; i < start_angle + FILL_ARC_SEG * seg_count; i += inc) {
    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + FILL_ARC_SEG - 90) * DEG2RAD);
    float sy2 = sin((i + FILL_ARC_SEG - 90) * DEG2RAD);
    int x2 = sx2 * (rx - w) + x;
    int y2 = sy2 * (ry - w) + y;
    int x3 = sx2 * rx + x;
    int y3 = sy2 * ry + y;

    tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
    tft.fillTriangle(x1, y1, x2, y2, x3, y3, color);

    // Copy segment end to segment start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}

// TODO: map arc color to value (e.g. (0 - 100) -> (green - yellow - orange - red))
void drawArcComponent(TFT_eSPI &tft, uint16_t center_x, uint16_t min_y, uint16_t max_y, const char* label, float value, uint16_t arc_radius) {
  uint16_t y_space = (max_y - min_y) - FONT_2_SIZE - arc_radius;

  tft.drawString(label, center_x, min_y, 2);
  char tx[10];
  sprintf(tx, "  %.1f  ", value);
  tft.drawString(tx, center_x, min_y + y_space, 2);

  // map the arc length to the value
  long arc_len = constrain(map(value, 0, 100, 1, HALF_CIRCLE_SEGMENTS), 1, HALF_CIRCLE_SEGMENTS);
  // When drawing the new arc, if the background has a different color than
  // the default background color (i.e. something else was previously drawn) that
  // color may still be visible. This problem could be resolved by drawing a
  // full-size default-background-colored arc, and then drawing the new arc, 
  // but this causes noticeable flickering.
  // Here, after drawing the new arc with the required length, we then draw a
  // default-background-colored arc from the new arc ends to the max_length.
  // This gets rid of anything that was left on the background without flickering.
  uint16_t end_angle = (ARC_COMPONENT_START_ANGLE + (arc_len * FILL_ARC_SEG)) % 360;
  fillArc(tft, center_x, max_y, ARC_COMPONENT_START_ANGLE, arc_len, arc_radius, arc_radius, 8, rainbow(0));
  fillArc(tft, center_x, max_y, end_angle, HALF_CIRCLE_SEGMENTS - arc_len, arc_radius, arc_radius, 8, TFT_BLACK);
}

// TODO: map line color to value (e.g. (0 - 100) -> (green - yellow - orange - red))
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
  tft.fillRect(line_len, max_y - 8, max_x, 8, TFT_BLACK);
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

// Return the 16 bit colour with brightness 0-100%
uint16_t brightness(unsigned int colour, int brightness) {
  uint8_t red = colour >> 11;
  uint8_t green = (colour & 0x7E0) >> 5;
  uint8_t blue = colour & 0x1F;

  blue = (blue * brightness) / 100;
  green = (green * brightness) / 100;
  red = (red * brightness) / 100;

  return (red << 11) + (green << 5) + blue;
}

// Return a 16 bit rainbow colour
uint16_t rainbow(byte value) {
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to 127 = red

  static uint8_t red = 31; // Red is the top 5 bits of a 16 bit colour value
  static uint8_t green = 0;// Green is the middle 6 bits
  static uint8_t blue = 0; // Blue is the bottom 5 bits
  static uint8_t state = 0;

  switch (state) {
    case 0:
      green ++;
      if (green == 64) {
        green = 63;
        state = 1;
      }
      break;
    case 1:
      red--;
      if (red == 255) {
        red = 0;
        state = 2;
      }
      break;
    case 2:
      blue ++;
      if (blue == 32) {
        blue = 31;
        state = 3;
      }
      break;
    case 3:
      green --;
      if (green == 255) {
        green = 0;
        state = 4;
      }
      break;
    case 4:
      red ++;
      if (red == 32) {
        red = 31;
        state = 5;
      }
      break;
    case 5:
      blue --;
      if (blue == 255) {
        blue = 0;
        state = 0;
      }
      break;
  }
  return red << 11 | green << 5 | blue;
}