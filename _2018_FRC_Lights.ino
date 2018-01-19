#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    50
#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];

boolean oddEffect = true;
long lastEffect = millis();
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

int bytes[4];

void setup() {
  // Serial stuff
  Serial.begin(115200);
  Serial.println("Hello world!");

  // FastLED stuff
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
}

void loop() {
  int i = 0;
  while (Serial.available() > 0) {
    bytes[i] = Serial.read();
    i ++;
  }

  // set color
  CRGB color;
  if (bytes[0] == 0) {
    color = CRGB::Blue;
  }
  else {
    color = CRGB::Red;
  }

  // 0x00 is flash
  if (bytes[1] == 0) {
    if (bytes[2] == 0) {
      flash(false, bytes[3], color);
    }
    else {
      flash(false, bytes[3], color);
    }
  }
  // 0x01 is stagger
  else if (bytes[1] == 1) {
    stagger(bytes[2], color, bytes[3]);
  }
  // 0x02 is rainbow
  else if (bytes[1] == 2) {
    rainbow();
  }

  stagger(400, CRGB::Red, 100);
  //flash(false, 500, CRGB::Purple);
  //rainbow();

  EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }

  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void stagger(int period, CRGB color, int fadingPercent) {
  long difference = millis() - lastEffect;
  if (difference > period) {
    if (oddEffect == true) {
      for (int i = 0; i < NUM_LEDS; i += 2) {
        leds[i] = color;
        leds[i + 1] = CRGB::Black;
      }
      oddEffect = false;
    }
    else {
      for (int i = 1; i < NUM_LEDS; i += 2) {
        leds[i] = color;
        leds[i - 1] = CRGB::Black;
      }
      oddEffect = true;
    }
    lastEffect = millis();
  }
}

void flash(boolean fade, int period, CRGB color) {
  if (fade == true) {
    fadeToBlackBy(leds, NUM_LEDS, 20);
  }

  long difference = millis() - lastEffect;

  if (difference > period) {
    if (oddEffect != true) {
      color = CRGB::Black;
      oddEffect = true;
    }
    else {
      oddEffect = false;
    }

    fill_solid(leds, NUM_LEDS, color);

    lastEffect = millis();
  }
}

void rainbow() {
  fill_rainbow(leds, NUM_LEDS, gHue, 10);
}

