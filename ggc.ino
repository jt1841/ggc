// ggc.ino
// Gesture Glove Controller
// ECE 4012 - Spring 2015
// Jonathan Talbott, Dan Russell, Justin Denard, Brian Nemsick, Erin Hanson

#include "flexSensors.h"

using namespace std;

// Global Variables
flexSensors flex0(0,803,945); // Declare flex sensor
flexSensors flex1(1,815,950);
flexSensors flex2(2,769,920);
flexSensors flex3(3,771,940);
flexSensors flex4(4,855,955);

void setup()
{
  Serial.begin(9600);	// Initialize Serial Communication and set 
                        // Rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200 bits/second
  pinMode(13, OUTPUT);  // Digital Ouput pin for testing output. This will later be part of a "control" class
}

void loop()
{
    flex0.read();
    flex1.read();
    flex2.read();
    flex3.read();
    flex4.read();
    flex0.display();
    flex1.display();
    flex2.display();
    flex3.display();
    flex4.display();
    Serial.println("");
    delay(10);
    
    // Code for outputting HIGH and LOW voltage values depending on the flex sensors degree reading 
    // Replace with function in "control" class
    if (flex0.angle() > 30)
        digitalWrite(13, HIGH);
    else
        digitalWrite(13, LOW);
}
