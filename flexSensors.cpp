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
  long int x = 0;
  for(int i = 0; i<100; i++)
  {
    x += analogRead(pin); //This gets a small moving average
  }
    rawValue = x/100;
    
    if(rawValue<minBend)
      rawValue = minBend;
      
    if(rawValue>maxBend)
      rawValue = maxBend;
    
    
    
}

double flexSensors::angle()
{   // Return current angle of flex sensor in degrees
    return map(rawValue, minBend, maxBend, 0, 90);
}

void flexSensors::display()
{
    int numDigits = rawValue > 0 ? (int) log10 ((double) rawValue) + 1 : 1;
    // Display output in Serial Monitor for debugging and determining the max and min bend values
    if(numDigits == 3)
    {
      Serial.print(rawValue);
    }
}

