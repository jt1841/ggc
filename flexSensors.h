#pragma once
#include "Arduino.h"

using namespace std;

class flexSensors
{
public:
    flexSensors();  // Default Constructor
    flexSensors(int, int); // Constructor (pin, threshold)
    void read();     // Read the raw sensor value
    void display(); // Display raw input value of sensor in Serial Manager
    int pin;    // Analog input pin 0-5
    int value;   // Raw value read from analog input pin (range: 0-1023)
    int threshold;    // Bend threshold value for flex sensor    
};

