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


int PIN_0 = 8;
int PIN_1 = 9;
int PIN_2 = 21;
int PIN_3 = 20;

Joystick stick1(PIN_0,PIN_1);
Joystick stick2(PIN_2,PIN_3);

byte i2cBuffer[4];
byte readBuffer[4];

byte highAxisMaximum[4];
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
// function that executes whenever data is requested by master
// this function is registered as an event, see 0et0p(0


void updateLEDs(){
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
    highAxisMaximum[i] = 128;
    lowAxisMaximum[i] = 0;
  }
}


void normalizeValues(){
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
}


void fillBuffer() {
  readBuffer[0] = stick1.getX();
  readBuffer[1] = stick1.getY();
  readBuffer[2] = stick2.getX();
  readBuffer[3] = stick2.getY();
  normalizeValues();
  //Serial.println((String)i2cBuffer[0] + " " + (String)i2cBuffer[1] + " " + (String)i2cBuffer[2] + " " + (String)i2cBuffer[3]);
}

void onUpHandler() {
	Serial.println(F("Direction: Up"));
	fillBuffer();
}

void onRightUpHandler() {
	Serial.println(F("Direction: Right-Up"));
	fillBuffer();
}

void onRightHandler() {
	Serial.println(F("Direction: Right"));
	fillBuffer();
}

void onRightDownHandler() {
	Serial.println(F("Direction: Right-Down"));
	fillBuffer();
}

void onDownHandler() {
	Serial.println(F("Direction: Down"));
	fillBuffer();
}

void onLeftDownHandler() {
	Serial.println(F("Direction: Left-Down"));
	fillBuffer();
}

void onLeftHandler() {
	Serial.println(F("Direction: Left"));
	fillBuffer();
}

void onLeftUpHandler() {
	Serial.println(F("Direction: Left-Up"));
	fillBuffer();
}


void transmitReadings(){
    Wire.write(i2cBuffer, 4);
}

void requestEvent() {
  transmitReadings();
}

void setup() {
  //Serial.begin(9600);
  strip.begin();  // initialize the strip
  strip.show();   // make sure it is visible
  strip.clear();  // Initialize all pixelo 'off'

  Wire.begin(26);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  zeroArrays();
  fillBuffer();

	// Wire up event handlers.
	stick1.onUp(onUpHandler);
	stick1.onRightUp(onRightUpHandler);
	stick1.onRight(onRightHandler);
	stick1.onRightDown(onRightDownHandler);
	stick1.onDown(onDownHandler);
	stick1.onLeftDown(onLeftDownHandler);
	stick1.onLeft(onLeftHandler);
	stick1.onLeftUp(onLeftUpHandler);

  stick2.onUp(onUpHandler);
	stick2.onRightUp(onRightUpHandler);
	stick2.onRight(onRightHandler);
	stick2.onRightDown(onRightDownHandler);
	stick2.onDown(onDownHandler);
	stick2.onLeftDown(onLeftDownHandler);
	stick2.onLeft(onLeftHandler);
	stick2.onLeftUp(onLeftUpHandler);
}

void loop() {
	// Read joystick and process events every 300ms.
	 stick1.loop();
    delay(150);
    stick2.loop();
      delay(150);
}
