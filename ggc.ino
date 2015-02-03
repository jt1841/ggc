// ggc.ino
// Gesture Glove Controller
// ECE 4012 - Spring 2015
// Jonathan Talbott, Dan Russell, Justin Denard, Brian Nemsick, Erin Hanson

#include "flexSensors.h"

using namespace std;

// Global Variables
flexSensors flex0(0,700,900); // Declare flex sensor

void setup()
{
  Serial.begin(9600);	// Initialize Serial Communication and set 
                        // Rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200 bits/second
  pinMode(13, OUTPUT);  // Digital Ouput pin for testing output. This will later be part of a "control" class
}

void loop()
{
    flex0.read();
    flex0.display();
    
    // Code for outputting HIGH and LOW voltage values depending on the flex sensors degree reading 
    // Replace with function in "control" class
    if (flex0.angle() > 30)
        digitalWrite(13, HIGH);
    else
        digitalWrite(13, LOW);
}
