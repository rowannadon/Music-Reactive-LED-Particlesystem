#include <Arduino.h>
#include <FastLED.h>

void drawPixels(int start, int end, CRGB color, int brightness) {
    uint16_t i;
    if (start >= end) {
        for (i = end; i < start; i++) {
            //nscale8x3_video(color.r, color.g, color.b, brightness);
            FastLED.leds()[i] += color.nscale8_video(brightness);
        }
    }
    if (start < end) {
        for (i = start; i < end; i++) {
            //nscale8x3_video(color.r, color.g, color.b, brightness);
            FastLED.leds()[i] += color.nscale8_video(brightness);
        }
    }
}