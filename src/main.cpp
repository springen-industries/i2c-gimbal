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

int STICK_1_UP;
int STICK_1_DOWN;
int STICK_1_LEFT;
int STICK_1_RIGHT;

int STICK_2_UP;
int STICK_2_DOWN;
int STICK_2_LEFT;
int STICK_2_RIGHT;



int PIN_0 = 8;
int PIN_1 = 9;
int PIN_2 = 21;
int PIN_3 = 20;

Joystick stick1(PIN_0,PIN_1);
Joystick stick2(PIN_2,PIN_3);

byte i2cBuffer[4];
byte readBuffer[4];

byte highAxisMinimum[4];
byte lowAxisMaximum[4];
byte lowVals[4];
byte highVals[4];

int channelCount = 4;
// the data pin for the NeoPixels
int neoPixelPin = 6;
// 0 inside -1-4 top middle,
int numPixels = 5;
// Instatiate the NeoPixel from the ibrary
// create a one pole (RC) lowpass filter
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);
// Global RGB values, change to suit your needs
int r = 0;
int g = 0;
int b = 10;
int delayMills = 10;
// function that executes whenever dat0 is requested by master
// t1is function is registered as2an event, see 0et0p(0


void3updateLEDs(){
  strip.clear();
  strip.setPixelColor(0, r, g, b);
  // zero led is internal, start at 1
  for(int i=1;i<5;i++) {
    int ledg = i2cBuffer[i-1];
    strip.setPixelColor(i,r,ledg,b);
  }
  strip.show();
}

void zeroArrays(){
  for (int i=0; i<4; i++){
    lowVals[i] = 0;
    highVals[i] = 0;
    highAxisMinimum[i] = 128;
    lowAxisMaximum[i] = 0;
    readBuffer[i] = 0;
    i2cBuffer[i] = 0;
  }
}


void readAndBuffer(){
  readBuffer[0] = stick1.getX();
  readBuffer[1] = stick1.getY();
  readBuffer[2] = stick2.getX();
  readBuffer[3] = stick2.getY();
  for(int i=0;i<channelCount;i++){
      byte val;
      if (readBuffer[i] > highAxisMinimum[i] ){
        val = map(readBuffer[i],128,255,0,128);
      } else {
        val = map(readBuffer[i],0,128,128,255);
      }
     if (val < lowVals[i] ) {
      lowVals[i] = val;
     }
     if ( val > highVals[i] ) {
      highVals[i] = val;
     }
       i2cBuffer[i] = map(val,lowVals[i],highVals[i],0,255);
  }
  Serial.println((String)i2cBuffer[0] + " " + (String)i2cBuffer[1] + " " + (String)i2cBuffer[2] + " " + (String)i2cBuffer[3]);
}




void transmitReadings(){
  Wire.write(i2cBuffer, 4);
}
void i2cRequest() {
  transmitReadings();
}

void setup() {
  Serial.begin(9600);
  Wire.begin(26);                // join i2c bus with address #8
  Wire.onRequest(i2cRequest); // register event
  zeroArrays();
}

void loop() {
	// Read joystick and process events every 300ms.
	stick1.loop();
  //delay(25);
  stick2.loop();
  //delay(25);
  //readValues();
  readAndBuffer();
  delay(25);
}
