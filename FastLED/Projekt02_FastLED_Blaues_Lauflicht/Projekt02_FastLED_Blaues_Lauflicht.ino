#include <FastLED.h>

#define NUM_LEDS  60
#define LED_PIN   4

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop () {
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot] = CRGB::Blue;
    FastLED.show();
    leds[dot] = CRGB::Black;
    delay(50);
  }
}
