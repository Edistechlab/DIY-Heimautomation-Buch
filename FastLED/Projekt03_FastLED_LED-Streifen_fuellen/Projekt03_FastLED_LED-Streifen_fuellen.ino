#include <FastLED.h>

#define NUM_LEDS  60
#define LED_PIN   4

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop() {
  fill_solid(leds, NUM_LEDS, CRGB::Aqua);
  FastLED.show();
  nextLight();

  fill_gradient_RGB(leds, NUM_LEDS, CRGB::Red, CRGB::Green);
  FastLED.show();
  nextLight();

  fill_gradient_RGB(leds, NUM_LEDS, CRGB::Blue, CRGB::Yellow, CRGB::Red, CRGB::Green);
  FastLED.show();
  nextLight();
  
  fill_rainbow(leds, NUM_LEDS, 0, 255 / NUM_LEDS);
  FastLED.show();
  nextLight();
}

void nextLight() {
  delay(4000);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(1000);
}
