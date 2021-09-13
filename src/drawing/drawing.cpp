#include "drawing/drawing.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#define DEG2RAD 0.0174532925

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