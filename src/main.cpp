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

#include <JoystickLib.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

int channelCount = 4;
int PIN_0 = 18;
int PIN_1 = 19;
int PIN_2 = 20;
int PIN_3 = 21;

Joystick stick1(PIN_0,PIN_1);
Joystick stick2(PIN_2,PIN_3);

byte highAxisMinimum[4];
byte lowAxisMaximum[4];

byte i2cBuffer[4];
byte readBuffer[4];
// the data pin for the NeoPixels
int neoPixelPin = 6;
// 0 inside -1-4 top middle,
int numPixels = 5;
// Instatiate the NeoPixel from the ibrary
// create a one pole (RC) lowpass filter
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);
// Global RGB values, change to suit your needs
int r = 50;
int g = 20;
int b = 50;

void normalizeValues(){
for(int i=0;i<channelCount;i++){
    if (readBuffer[i] > highAxisMinimum[i] ){
      i2cBuffer[i] = map(readBuffer[i],highAxisMinimum[i],255,0,128);
    } else {
      i2cBuffer[i] = map(readBuffer[i],0,lowAxisMaximum[i],128,256);
    }
  }
  // Serial.print("s1x: ");
  // Serial.print((int)i2cBuffer[0]);
  // Serial.print("s1y: ");
  // Serial.print((int)i2cBuffer[1]);
  // Serial.print("s2x: ");
  // Serial.print((int)i2cBuffer[2]);
  // Serial.print("s3x: ");
  // Serial.print((int)i2cBuffer[3]);
  // Serial.println("-----");
}

void updateLEDs(){
  strip.setPixelColor(0, r, g, b);
  // zero led is internal, start at 1
  for(int i=1;i<5;i++) {
    int ledg = i2cBuffer[i-1];
    strip.setPixelColor(i,r,ledg+20,b);
  }
  strip.show();
}

void readValues(){
  stick1.loop();
  stick2.loop();
  readBuffer[0] = stick1.getX();
  readBuffer[1] = stick1.getY();
  readBuffer[2] = stick2.getX();
  readBuffer[3] = stick2.getY();
  // --------------------------------------
  // Serial.print("s1x: ");
  // Serial.print((int)readBuffer[0]);
  // Serial.print("s1y: ");
  // Serial.print((int)readBuffer[1]);
  // Serial.print("s2x: ");
  // Serial.print((int)readBuffer[2]);
  // Serial.print("s3x: ");
  // Serial.print((int)readBuffer[3]);
  // Serial.println("-----");
  // Serial.println("-----");
  // Serial.println("-----");
  // Serial.println("-----");
  // -------------------------------------------
}

void transmitReadings(){
    Wire.write(i2cBuffer, 4);
}

void requestEvent() {
  strip.clear();
  transmitReadings();
}

void zeroArrays(){
  for(int i=0; i<4; i++){
    highAxisMinimum[i] = 200;
    lowAxisMaximum[i] = 70;
  }
}

void setup() {
  zeroArrays();
  strip.begin();  // initialize the strip
  strip.show();   // make sure it is visible
  strip.clear();  // Initialize all pixelo 'off'
  Wire.begin(26);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);
}

void loop() {
  updateLEDs();
  normalizeValues();
  readValues();
}
