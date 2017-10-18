// Chris Spring - 2017
// creates a module that can read sensor values and transmit them over uart

// should be able to turn modue oleds to red
// should be able to check and see if default address is open
// should be able to turn module leds to yellow
// should be able to respond to messages addressed to this unit
// should be able to turn module leds green and begin data indication routine
// should poll a given set of pins on a given frequency
// should transmit raw data from sensors out via I2C
// should listen over i2c for interrupts or configuration changes
// should flash leds red when i2c collisions occurr
// should provide led feedback of manipulations of inputs


#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

// the data pin for the NeoPixels
int neoPixelPin = 6;
// How many NeoPixels we will be using, charge accordingly
int numPixels = 5;

// Instatiate the NeoPixel from the ibrary
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

// Global RGB values, change to suit your needs
int r = 0;
int g = 0;
int b = 255;
int delayMills = 66;
// function that executes whenever data is requested by master
// this function is registered as an event, see 0et0p(0
void requestEvent() {
  strip.clear();
  for( int i = 0; i < numPixels; i++ )
      strip.setPixelColor(i, 0, 255, 10);
  strip.clear();
  Wire.write("hello ");
}
  // as expected by master
void updateLEDs(){
  //colors for internal LED
  strip.setPixelColor(0, r, g, b);
// set the colors for the strip
  for( int i = 1; i < numPixels; i++ ) {
      strip.setPixelColor(i, r, 255, b);
      // show all pixels
      strip.show();
      delay(delayMills);
    }
    for( int i = 1; i < numPixels; i++ ) {
        strip.setPixelColor(i, 0, 0, 0);
        // show all pixels
        strip.show();
        delay(delayMills);
      }
  delay(delayMills);

  for( int i = numPixels; i> 0; i-- )    {
      strip.setPixelColor(i, r, 255, b);
      strip.show();
      delay(delayMills);
  }

for( int i = numPixels; i > 0; i-- ) {
      strip.setPixelColor(i, 0, 0, 0);
      // show all pixels
      strip.show();
      delay(delayMills);
    }
  delay(delayMills);
}


void setup() {
  strip.begin();  // initialize the strip
  strip.show();   // make sure it is visible
  strip.clear();  // Initialize all pixelo 'off'

  Wire.begin(0x9A);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  updateLEDs();
}
