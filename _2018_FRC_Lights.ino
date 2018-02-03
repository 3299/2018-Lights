#include "FastLED.h"
#include <Wire.h>

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

String command = "";
boolean fade = true;
CRGB color = CRGB::Purple;
int speed = 255;

void setup() {
  // Serial stuff
  Serial.begin(115200);
  Serial.println("Hello world!");

  // FastLED stuff
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);

  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
}

void loop() {
  char commandByte = command.charAt(0);
  if (commandByte != 'r') {
    if (command.charAt(1) == 'b') {
      color = CRGB::Blue;
    }
    else {
      color = CRGB::Red;
    }

    if (command.charAt(2) == 't') {
      boolean fade = true;
    }
    else {
      boolean fade = false;
    }

    int speed = command.substring(3, command.length() - 1).toInt();
  }

  if (commandByte == 'r') {
    rainbow();
  }
  else if (commandByte == 's') {
    if (fade == true) {
      stagger(speed, color, 30);
    }
    else {
      stagger(speed, color, 0);
    }
  }
  else if (commandByte == 'f') {
    flash(fade, speed, color);
  }


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

void receiveEvent(int howMany) {
  String newCommand = "";
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
    newCommand += c;
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println();

  command = newCommand;
}



