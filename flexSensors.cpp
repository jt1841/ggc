#include "flexSensors.h"
#include "Arduino.h"

flexSensors::flexSensors()
{   //Default constructor
    pin = 0;
    threshold = 950;
    rest = 0;
    value = 0;
    active = 0;
}

flexSensors::flexSensors(int _pin, int _threshold)
{
    // Constructor with arguments
    pin = _pin;  
    threshold = _threshold; 
    rest = 0;
    value = 0;
    active = 0;
}


void flexSensors::read()
{   // Read raw input value from analog pin
  long int x = 0;
  for(int i = 0; i<100; i++)
  {
    x += analogRead(pin); //This gets a small moving average
  }
     
  x /= 100;
  
  int numDigits = x > 0 ? (int) log10 ((double) x) + 1 : 1;
  
    if(numDigits == 3)
      value = x;
      
  if(threshold > rest)
  {
    if (value > (threshold - 25)){active = 1;}
    else{active = 0;}
  }
  else
  {
    if(value < (threshold + 25)){active = 1;}
    else{active = 0;} 
  }
  
}

void flexSensors::display()
{
    int numDigits = value > 0 ? (int) log10 ((double) value) + 1 : 1;
    // Display output in Serial Monitor for debugging and determining the max and min bend values
    if(numDigits <= 3)
    {
      Serial.print(value);
    }
    else
    {
      Serial.print(999);
    }
}

