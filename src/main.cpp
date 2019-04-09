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
//#include <Adafruit_NeoPixel.h>
#include <Wire.h>

//constants used for stick movements
const int STICK_1_UP = 0;
const int STICK_1_DOWN = 1;
const int STICK_1_LEFT = 2;
const int STICK_1_RIGHT = 3;

const int STICK_2_UP = 4;
const int STICK_2_DOWN = 5;
const int STICK_2_LEFT = 6;
const int STICK_2_RIGHT = 7;


/// pin assignments
//const int PIN_0 = 8;
//const int PIN_1 = 9;
//const int PIN_2 = 21;
//const int PIN_3 = 20;

// pin association to joystick object input
const int channelCount = 4;

// the data pin for the NeoPixels
byte i2cBuffer[channelCount];
byte readBuffer[channelCount];

byte highAxisMinimum[channelCount];
byte lowAxisMaximum[channelCount];
byte lowVals[channelCount];
byte highVals[channelCount];



int PIN_0 = 24;
int PIN_1 = 25;
int PIN_2 = 23;
int PIN_3 = 21;

Joystick stick1(PIN_1,PIN_0);
Joystick stick2(PIN_2,PIN_3);




//int neoPixelPin = 6;
// 0 inside -1-4 top middle,
//int numPixels = 5;
// Instatiate the NeoPixel from the ibrary


/*
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);
// Global RGB values, change to suit your needs
int r = 0;
int g = 0;
int b = 10;
*/

int delayMills = 10;
// function that executes whenever dat0 is requested by master
// t1is function is registered as2an event, see 0et0p(0

/*
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
*/

void zeroArrays(){
  for (int i=0; i<channelCount; i++){
    lowVals[i] = 1;
    highVals[i] = 255;
    highAxisMinimum[i] = 128;
    lowAxisMaximum[i] = 0;
    readBuffer[i] = 0;
    i2cBuffer[i] = 0;
  }
}


void readAndBuffer(){
  readBuffer[1] = stick1.getX();
  readBuffer[0] = stick1.getY();
  readBuffer[3] = stick2.getX();
  readBuffer[2] = stick2.getY();
  for(int i=0;i<channelCount;i++){
      byte val;
      if (readBuffer[i] > highAxisMinimum[i] ){
        val = 255 - map(readBuffer[i],128,255,0,128);
      } else {
        val = 255 - map(readBuffer[i],0,128,128,255);
      }
     if (val < lowVals[i] && val != 0 ) {
      lowVals[i] = val;
     }
     if ( val > highVals[i] & val != 2000 ) {
      highVals[i] = val;
     }
     i2cBuffer[i] = map(val,lowVals[i],highVals[i],0,255);
  }

  Serial.println((String)lowVals[0] + " " + (String)i2cBuffer[0] + " " + (String)i2cBuffer[1] + " " + (String)i2cBuffer[2] + " " + (String)i2cBuffer[3]);
}




void transmitReadings(){
  //Wire.beginTransmission(6);
  Wire.write(i2cBuffer, channelCount);
  //Wire.endTransmission(6);
}
void i2cRequest() {
  transmitReadings();
  //Serial.println("Got request for eadings via i2c");
}

void setup() {
  Serial.begin(9600);
  Wire.begin(10);                // join i2c bus with address #8
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
  //delay(25);
  //transmitReadings();
}
