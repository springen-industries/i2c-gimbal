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

#include <Filters.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

int PIN_0 = 18;
int pin0_val;;
int PIN_1 = 19;
int pin1_val;
int PIN_2 = 20;
int pin2_val;
int PIN_3 = 21;
int pin3_val;
float filterAlpha = 0.2;

float filteredSignal0;
float filteredSignal1;
float filteredSignal2;
float filteredSignal3;

uint8_t buffer[4];

float sensorValue;
// the data pin for the NeoPixels
int neoPixelPin = 6;
// How many NeoPixels we will be using, charge accordingly
int numPixels = 5;

// Instatiate the NeoPixel from the ibrary
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

// filters out changes faster that 5 Hz.
float filterFrequency = 5.0;

// create a one pole (RC) lowpass filter
FilterOnePole lowpassFilter( LOWPASS, filterFrequency );



// Global RGB values, change to suit your needs
int r = 0;
int g = 0;
int b = 255;
int delayMills = 10;
// function that executes whenever data is requested by master
// this function is registered as an event, see 0et0p(0
void updateLEDs(){
  // as expected by master
  //colors for internal LED
  strip.setPixelColor(0, r, g, b);
  for( int i = 1; i < numPixels; i++ ) {
    // set the colors for the strip
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


void readValues(){



  sensorValue = analogRead(PIN_0);
  filteredSignal0 = (filterAlpha* sensorValue) + ((1-filterAlpha)*filteredSignal0);
  buffer[0] = filteredSignal0;
  Serial.println(buffer[0]);
  sensorValue = analogRead(PIN_1);
  filteredSignal1 = (filterAlpha* sensorValue) + ((1-filterAlpha)*filteredSignal1);
  buffer[1] = filteredSignal1;
  //buffer[1] = analogRead(PIN_1);// & 0xff;
  Serial.println(buffer[1]);
  sensorValue = analogRead(PIN_2);
  filteredSignal2 = (filterAlpha* sensorValue) + ((1-filterAlpha)*filteredSignal2);
  buffer[2] = filteredSignal2;
  Serial.println(buffer[2]);
  sensorValue = analogRead(PIN_3);
  filteredSignal3 = (filterAlpha* sensorValue) + ((1-filterAlpha)*filteredSignal3);
  buffer[3] = filteredSignal3;
 Serial.println(buffer[3]);

}

void transmitReadings(){
    Wire.write(buffer, 4);
}

void requestEvent() {
  strip.clear();
  //strip.setPixelColor(0, 0, 255, 10);
  //strip.show();
  transmitReadings();
}

void setup() {
  //Serial.begin(9600);
  strip.begin();  // initialize the strip
  strip.show();   // make sure it is visible
  strip.clear();  // Initialize all pixelo 'off'
  Wire.begin(26);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  updateLEDs();
  readValues();

}
