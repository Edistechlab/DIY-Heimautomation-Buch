#include <FastLED.h>

#define NUM_LEDS  60
#define LED_PIN   4

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop () {
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.setTemperature(Tungsten40W);
  nextColor();
  FastLED.setTemperature(Tungsten100W);
  nextColor();
  FastLED.setTemperature(Halogen);
  nextColor();
  FastLED.setTemperature(CarbonArc);
  nextColor();
  FastLED.setTemperature(HighNoonSun);
  nextColor();
  FastLED.setTemperature(DirectSunlight);
  nextColor();
  FastLED.setTemperature(OvercastSky);
  nextColor();
  FastLED.setTemperature(ClearBlueSky);
  nextColor();
}

void nextColor(){
  FastLED.show();
  delay(1000);
}
