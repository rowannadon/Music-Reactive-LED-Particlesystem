#include <Arduino.h>
#include <FastLED.h>

void drawPixels(int start, int end, CRGB color) {
    if (start >= end) {
        for (int i = end; i < start; i++) {
            FastLED.leds()[i] += color;
        }
    }

    if (start < end) {
        for (int i = start; i < end; i++) {
            FastLED.leds()[i] += color;
        }
    }
    
}