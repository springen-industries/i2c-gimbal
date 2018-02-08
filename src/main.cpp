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


int PIN_0 = 18;
int PIN_1 = 19;
int PIN_2 = 20;
int PIN_3 = 21;

Joystick stick1(PIN_0,PIN_1);
Joystick stick2(PIN_2,PIN_3);

uint8_t buffer[4];

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
  strip.setPixelColor(0, r, g, b);
  // zero led is internal, start at 1
  for(int i=1;i<5;i++) {
    int ledg = buffer[i-1];
    strip.setPixelColor(i,r,ledg,b);
  }
  strip.show();
}


void fillBuffer() {
  buffer[0] = stick1.getX();
  buffer[1] = stick1.getY();
  buffer[2] = stick2.getX();
  buffer[3] = stick2.getY();
  Serial.println((int)buffer[0] + " " + (int)buffer[1] + " " + (int)buffer[2] + " " + (int)buffer[3]);
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
    Wire.write(buffer, 4);
}

void requestEvent() {
  strip.clear();
  transmitReadings();
}

void setup() {
  //Serial.begin(9600);
  strip.begin();  // initialize the strip
  strip.show();   // make sure it is visible
  strip.clear();  // Initialize all pixelo 'off'
  Wire.begin(26);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);
	while (!Serial) {
		delay(10);
	}

	// Calibrate and print out position.
	Serial.println();
	Serial.print(F("Calibrating.... "));
	stick.calibrate();
	Serial.println(F("Done."));
	Serial.println(F("Calibrated zero position: "));
	fillBuffer();

	// Wire up event handlers.
	stick.onUp(onUpHandler);
	stick.onRightUp(onRightUpHandler);
	stick.onRight(onRightHandler);
	stick.onRightDown(onRightDownHandler);
	stick.onDown(onDownHandler);
	stick.onLeftDown(onLeftDownHandler);
	stick.onLeft(onLeftHandler);
	stick.onLeftUp(onLeftUpHandler);
}

void loop() {
	// Read joystick and process events every 300ms.
	stick.loop();
    delay(300);
}
