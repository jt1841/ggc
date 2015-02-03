#include "flexSensors.h"
#include "Arduino.h"

flexSensors::flexSensors()
{   //Default constructor
    pin = 0;
    minBend = 768;
    maxBend = 853;
    rawValue = 0;
    angleValue = 90;
}

flexSensors::flexSensors(int pinNum, int min, int max)
{
    // Constructor with arguments
    pin = pinNum;   
    minBend = min;
    maxBend = max;
    rawValue = 0;
    angleValue = 90;
}


void flexSensors::read()
{   // Read raw input value from analog pin
    rawValue = analogRead(pin);
}

double flexSensors::angle()
{   // Return current angle of flex sensor in degrees
    return map(rawValue, minBend, maxBend, 0, 90);
}

void flexSensors::display()
{
    // Display output in Serial Monitor for debugging and determining the max and min bend values
    Serial.print("analog input: ");   // Print results for debugging
    Serial.print(rawValue, DEC);
    Serial.print("   degrees: ");
    Serial.println(angle(), DEC);
}

