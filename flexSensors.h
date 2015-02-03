#pragma once
#include "Arduino.h"

using namespace std;

class flexSensors
{
public:
    flexSensors();  // Default Constructor
    flexSensors(int, int, int); // Constructor with Pin Assignment Analog Input 0 - 5
    void read();     // Read the raw sensor value
    double angle();    // returns the current flex value in degrees
    void display(); // Display raw input value of sensor reading for debugging purposes
private:
    int pin;    // Analog input pin 0-5
    int rawValue;   // Raw value read from analog input pin (range: 0-1023)
    int angleValue; // Angle of flex sensor in degrees
    int maxBend;    // Maximum bend value set in call to constructor - Unique to each flex sensor
    int minBend;    // Minimum bend value set in call to constructor - Unique to each flex sensor
};

