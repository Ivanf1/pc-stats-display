#include "utils/colorConversion.h"

#include <Arduino.h>

float hue2rgb(float p, float q, float t) {
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1 / 6.0) return p + (q - p) * 6 * t;
	if (t < 1 / 2.0) return q;
	if (t < 2 / 3.0) return p + (q - p) * (2 / 3.0 - t) * 6;
	return p;
}

uint16_t hslToRgb(float hue, float saturation, float lightness) {
	float r, g, b;

	if (saturation == 0) {
		r = g = b = lightness; // achromatic
	} else {
		auto q = lightness < 0.5 ? lightness * (1 + saturation) : lightness + saturation - lightness * saturation;
		auto p = 2 * lightness - q;
		r = hue2rgb(p, q, hue + 1 / 3.0);
		g = hue2rgb(p, q, hue);
		b = hue2rgb(p, q, hue - 1 / 3.0);
	}

	uint8_t red = static_cast<uint8_t>(r * 255);
	uint8_t green = static_cast<uint8_t>(g * 255);
	uint8_t blue = static_cast<uint8_t>(b * 255);

  return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}