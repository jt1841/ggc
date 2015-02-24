// ggc.ino
// Gesture Glove Controller
// ECE 4012 - Spring 2015
// Jonathan Talbott, Dan Russell, Justin Denard, Brian Nemsick, Erin Hanson
#include <Wire.h>
#include "flexSensors.h"

using namespace std;

// Global Variables
flexSensors flex0(0,940); // Declare flex sensor
flexSensors flex1(1,950);
flexSensors flex2(2,970);
flexSensors flex3(3,950);
flexSensors flex4(4,950);

const int MPU = 0x68;  // I2C address of the MPU-6050
int gyroX;
int gyroThreshold = 10000;

//Functions
String flexOutput = "null"; // For debugging the outputs
String gyroOutput = "null";
String processingOutput = "000111222GGGGG";
void flexCheck();
void gyroCheck();
void processingDemo();

void setup()
{
  
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);  
  
  Serial.begin(9600);	// Initialize Serial Communication and set 
                        // Rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200 bits/second
 // pinMode(13, OUTPUT);  // Digital Ouput pin for testing output.
 
}

void loop()
{
    flexCheck();
    gyroCheck();
    
    //Serial.println(flexOutput + " " + gyroOutput); // For Serial Manager Output
    processingDemo(); // Output for Processing
}


void flexCheck()
{
    flex0.read();
    flex1.read();
    flex2.read();
    flex3.read();
    flex4.read();
    
// Check Flex Sensor Values

  if (flex1.value > flex1.threshold && flex2.value > flex2.threshold)
  {
    if (flex0.value > flex0.threshold)
    {
       flexOutput = "backward"; 
    }
    else
    {
      flexOutput = "forward";
    }
  }
  else
  {
    flexOutput = "rest";
  } 
  
}

void gyroCheck()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,2,true);  // request a total of 14 registers
  gyroX = Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  
  if(gyroX > gyroThreshold)
  {
    gyroOutput = "left";
  }
  else if(gyroX < -gyroThreshold)
  {
    gyroOutput = "right";
  }
  else
  {
    gyroOutput = "straight";
  }
  
  
}

void processingDemo()
{
  processingOutput = String(flex0.value) + "," + String(flex1.value) + "," + String(flex2.value) + ",g," + String(gyroX);
  Serial.println(processingOutput);
  
}
  
