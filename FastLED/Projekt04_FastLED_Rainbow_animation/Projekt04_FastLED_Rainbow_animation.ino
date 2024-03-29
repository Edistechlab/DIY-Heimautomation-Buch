#include <FastLED.h>

#define NUM_LEDS  60
#define LED_PIN   4

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop () {
  uint8_t beatA = beatsin8(30, 0, 255);
  uint8_t beatB = beatsin8(20, 0, 255);
  fill_rainbow(leds, NUM_LEDS, (beatA+beatB)/2, 8);
  FastLED.show();
}
