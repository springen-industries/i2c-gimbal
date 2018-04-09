// Chris Spring - 2017
// reads voltages from pots
// translates 0-5 volts into 0-255 byte value
// one value per axis so 4 bytes are used to represent positions of gimbals on 4 axies
// JoystickLib normalizes and filters readings into reasonably sane values
// bytes are sent via i2c as a response to a request from master

#include <JoystickLib.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
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
const int PIN_0 = 8;
const int PIN_1 = 9;
const int PIN_2 = 21;
const int PIN_3 = 20;

// pin association to joystick object input
const int channelCount = 4;

// the data pin for the NeoPixels
byte i2cBuffer[channelCount];
byte readBuffer[channelCount];

byte highAxisMinimum[channelCount];
byte lowAxisMaximum[channelCount];
byte lowVals[channelCount];
byte highVals[channelCount];



Joystick stick1(PIN_0,PIN_1);
Joystick stick2(PIN_2,PIN_3);




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
  for (int i=0; i<channelCount; i++){
    lowVals[i] = 0;
    highVals[i] = 0;
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
  Wire.write(i2cBuffer, channelCount);
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
