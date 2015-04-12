#include "flexSensors.h"
#include "Arduino.h"

flexSensors::flexSensors()
{   //Default constructor
    pin = 0;
    rest = 0;
    value = 0;
    active = 0;
    minimum = 850;
    maximum = 1024;
    threshold = 80;
}

flexSensors::flexSensors(int _pin, int _minimum, int _maximum, int _threshold)
{
    // Constructor with arguments
    pin = _pin;  
    rest = 0;
    value = 0;
    active = 0;
    minimum = _minimum;
    maximum = _maximum;
    threshold = _threshold;
}


void flexSensors::read()
{   // Read raw input value from analog pin
  long double x = 0;
  for(int i = 0; i<100; i++)
  {
    x += analogRead(pin); //This gets a small moving average
  }
     
  x /= 100;
  x = floor( abs((x - minimum)/(maximum-minimum))*100);
  
  if (x > 100)
    value = 100;
  else if (x < 0)
    value = 0;
  else
    value = x;
  
  //value = abs((x - minimum)/(maximum-minimum));
      
  if(threshold > rest)
  {
    if (value > (threshold)){active = 1;}
    else{active = 0;}
  }
  else
  {
    if(value < (threshold)){active = 1;}
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

