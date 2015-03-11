// ggc.ino
// Gesture Glove Controller
// ECE 4012 - Spring 2015
// Jonathan Talbott, Dan Russell, Justin Denard, Brian Nemsick, Erin Hanson
#include <Wire.h>
#include "flexSensors.h"
#include <LiquidCrystal.h>
#include <TimerOne.h>

#define LEFTPIN 10
#define RIGHTPIN 11
#define FORPIN 12
#define BACKPIN 13
#define HIGHV 255
#define LOWV 0

using namespace std;

// Global Variables
flexSensors flex0(0,980); // Declare flex sensor
flexSensors flex1(1,980);
flexSensors flex2(2,980);
flexSensors flex3(3,980);
flexSensors flex4(4,980);

const int MPU = 0x68;  // I2C address of the MPU-6050
int gyroX = 0;
int gyroThreshold = 10000;
boolean mode = 0;
boolean left = 0;
boolean right = 0;
boolean forward = 0;
boolean backward = 0;
int modeCount = 0;
String flexOutput = "null"; // For debugging the outputs
String gyroOutput = "null";
String processingOutput = "null";
boolean forwardState[5] = {0,1,1,0,0};
boolean backwardState[5] = {1,1,1,0,0};
boolean flexState[5] = {0};

//Functions
void flexCheck();
void gyroCheck();
void processingDemo();
void PWMout();
void onOff();
void calibrationSetup();
LiquidCrystal lcd(8, 7, 4, 5, 1, 0);

void setup()
{  
  //LCD Setup
  lcd.begin(16, 2);

  //Gyro Setup
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);  

  Serial.begin(9600);	// Initialize Serial Communication and set 
  // Rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200 bits/second

  // Setup PWM output pins
  pinMode(LEFTPIN, OUTPUT);
  pinMode(RIGHTPIN, OUTPUT);
  pinMode(FORPIN, OUTPUT);
  pinMode(BACKPIN, OUTPUT);

  //Timer1 Setup
  Timer1.initialize(100000);
  Timer1.attachInterrupt(onOff);

  calibrationSetup();

}

void loop()
{

  flexCheck();
  gyroCheck();
  PWMout(); //Call "check" functions before this

  //Debugging and Demo - Call after all other functions
  //Serial.println(flexOutput + " " + gyroOutput); // For Serial Manager Output
  //Serial.println(String(mode) + "," + String(modeCount));
  // processingDemo(); // Output for Processing
  //Serial.println(String(flex0.threshold));

  //LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(flexOutput);
  lcd.setCursor(0,1);
  lcd.print(gyroOutput);
  lcd.setCursor(13,0);
  if(mode){
    lcd.print("ON");
  }
  else{
    lcd.print("OFF");
  }

}


void flexCheck()
{
  flex0.read();
  flex1.read();
  flex2.read();
  flex3.read();
  flex4.read();

  flexState[0] = flex0.active;
  flexState[1] = flex1.active;
  flexState[2] = flex2.active;
  flexState[3] = flex3.active;
  flexState[4] = flex4.active;

  // Check Flex Sensor Values
  for(int j = 0; j < 5; j++)
  {
    if (forwardState[j] == flexState[j])
    {
      forward = 1;
    }
    else
    {
      forward = 0;
      break;
    }
  }

  for(int j = 0; j < 5; j++)
  {
    if (backwardState[j] == flexState[j])
    {
      backward = 1;
    }
    else
    {
      backward = 0;
      break;
    }    
  }

  if(backward)
  {
    flexOutput = "backward";
  }
  else if(forward)
  {
    flexOutput = "forward";
  }
  else
  {
    flexOutput = "rest";
  } 


}

void gyroCheck()
{
  long int sampleGyro = 0;

  //100 Samples of Gyro
  for (int i = 0; i < 100; i++)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,2,true);  // request a total of 14 registers
    sampleGyro += Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  }

  gyroX = sampleGyro/100; //Take average value

  if(gyroX > gyroThreshold)
  {
    gyroOutput = "left";
    left = 1;
    right = 0;
  }
  else if(gyroX < -gyroThreshold)
  {
    gyroOutput = "right";
    right = 1;
    left = 0;
  }
  else
  {
    gyroOutput = "straight";
    left = 0;
    right = 0;
  }


}

void processingDemo()
{
  processingOutput = String(flex0.value) + "," + String(flex1.value) + "," + String(flex2.value) + ",g," + String(gyroX);
  Serial.println(processingOutput);

}

void PWMout()
{

  if(modeCount > 29)
  {
    mode = !mode; 
  }


  if(backward)
  {
    analogWrite(BACKPIN,LOWV);
    analogWrite(FORPIN, HIGHV);
  }
  else if(forward)
  {
    analogWrite(FORPIN, LOWV);
    analogWrite(BACKPIN,HIGHV);
  }
  else
  {
    analogWrite(BACKPIN,HIGHV);
    analogWrite(FORPIN,HIGHV);
  } 

  if(mode)
  { 

    if(left)
    {
      analogWrite(LEFTPIN,LOWV);
      analogWrite(RIGHTPIN,HIGHV); 
    }
    else if(right)
    {
      analogWrite(RIGHTPIN,LOWV);
      analogWrite(LEFTPIN,HIGHV);
    }  
    else
    {
      analogWrite(RIGHTPIN,HIGHV);
      analogWrite(LEFTPIN,HIGHV);
    }  

  }
  else
  {
    analogWrite(RIGHTPIN,HIGHV); 
    analogWrite(LEFTPIN,HIGHV); 
    analogWrite(FORPIN,HIGHV); 
    analogWrite(BACKPIN,HIGHV); 
  }
}

void onOff()
{
  if(flex0.active && !flex1.active && !flex2.active)
  {
    modeCount += 1;
  } 

  if (!flex0.active || flex1.active || flex2.active || modeCount == 31)
  {
    modeCount = 0;
  }
}

void calibrationSetup()
{


  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("187 Days: GGC");
  lcd.setCursor(0,1);
  lcd.print("Tilt Left");

  while(gyroOutput != "left")
  {
    gyroCheck();
  }

  while(1)
  {

    lcd.clear();

    while(gyroOutput != "right")
    {
      gyroCheck();
      flex0.read();
      flex1.read();
      flex2.read();
      lcd.setCursor(0,0);
      lcd.print("Rest Hand: " + String(flex0.value));
      lcd.setCursor(0,1);
      lcd.print("Right to Confirm");
      flex0.rest = flex0.value;
      flex1.rest = flex1.value;
      flex2.rest = flex2.value;
      delay(10);
    }
    
      Serial.println("Thumb Rest = " + String(flex0.rest));
      Serial.println("Middle Rest = " + String(flex1.rest));
      Serial.println("Ring Rest = " + String(flex2.rest));

    lcd.clear();

    while(gyroOutput != "left")
    {
      gyroCheck();
      flex0.read();
      lcd.setCursor(0,0);
      lcd.print("Flex Thumb: " + String(flex0.value));
      lcd.setCursor(0,1);
      lcd.print("left to Confirm");
      flex0.threshold = flex0.value;
      delay(10);
    }

    Serial.println("Thumb Flex = " + String(flex0.threshold));
    lcd.clear();

    while(gyroOutput != "right")
    {
      gyroCheck();
      flex1.read();
      lcd.setCursor(0,0);
      lcd.print("Flex Middle: " + String(flex1.value));
      lcd.setCursor(0,1);
      lcd.print("Right to Confirm");
      flex1.threshold = flex1.value;
      delay(10);
    }

    Serial.println("Middle Flex = " + String(flex1.threshold));
    lcd.clear();

    while(gyroOutput != "left")
    {
      gyroCheck();
      flex2.read();
      lcd.setCursor(0,0);
      lcd.print("Flex Ring: " + String(flex2.value));
      lcd.setCursor(0,1);
      lcd.print("Left to Confirm");
      flex2.threshold = flex2.value;
      delay(10);
    }
    
    Serial.println("Ring Flex = " + String(flex2.threshold));

    if( abs(flex0.rest - flex0.threshold) > 30 && abs(flex1.rest - flex1.threshold) > 30 && abs(flex2.rest - flex2.threshold) > 30)
    {
      break; 
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Range of values");
      lcd.setCursor(0,1);
      lcd.print("too small");
      delay(3000);
      lcd.clear();
      while(gyroOutput != "right")
      {
        gyroCheck();
        lcd.setCursor(0,0);
        lcd.print("Tilt right"); 
        lcd.setCursor(0,1);
        lcd.print("to try again");
      }
    }

  }

  lcd.clear();

  while(1)
  {

    while(gyroOutput != "right")
    {
      gyroCheck();
      lcd.setCursor(0,0);
      lcd.print("Control Setup");
      lcd.setCursor(0,1);
      lcd.print("Right to cont.");
    }

    lcd.clear();

    while(gyroOutput != "left")
    {
      flex0.read();
      flex1.read();
      flex2.read();
      flex3.read();
      flex4.read();
      gyroCheck();
      lcd.setCursor(0,0);
      lcd.print("Forward Flex Command");
      lcd.setCursor(0,1);
      lcd.print("Left to Confirm");
      forwardState[0] = flex0.active;
      forwardState[1] = flex1.active;
      forwardState[2] = flex2.active;
      forwardState[3] = flex3.active;
      forwardState[4] = flex4.active;
    }

    lcd.clear();

    while(gyroOutput != "right")
    {
      flex0.read();
      flex1.read();
      flex2.read();
      flex3.read();
      flex4.read();
      gyroCheck();
      lcd.setCursor(0,0);
      lcd.print("Backward Flex Command");
      lcd.setCursor(0,1);
      lcd.print("Right to Confirm");
      backwardState[0] = flex0.active;
      backwardState[1] = flex1.active;
      backwardState[2] = flex2.active;
      backwardState[3] = flex3.active;
      backwardState[4] = flex4.active;
    }

    lcd.clear();
    
    //Confirm States
    int check = 0;

    for(int ii = 0; ii < 5; ii++)
    { 
      if(backwardState[ii] == forwardState[ii])
        check += 1;
    }
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Forward state is:");
    lcd.setCursor(0,1);
    String forwardConfirm = "";
    
    if(forwardState[0] == 1){forwardConfirm += "th,";}
    if(forwardState[1] == 1){forwardConfirm += "id,";}
    if(forwardState[2] == 1){forwardConfirm += "mi,";}
    if(forwardState[3] == 1){forwardConfirm += "ri,";}
    if(forwardState[4] == 1){forwardConfirm += "pi,";}
    lcd.print(forwardConfirm);
    delay(5000);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Backward state is:");
    lcd.setCursor(0,1);
    String backwardConfirm = "";
    
    if(backwardState[0] == 1){backwardConfirm += "th,";}
    if(backwardState[1] == 1){backwardConfirm += "id,";}
    if(backwardState[2] == 1){backwardConfirm += "mi,";}
    if(backwardState[3] == 1){backwardConfirm += "ri,";}
    if(backwardState[4] == 1){backwardConfirm += "pi,";}
    lcd.print(backwardConfirm);
    delay(5000);
    

    if(check == 5)
    {
      lcd.setCursor(0,0);
      lcd.print("Same states for");
      lcd.setCursor(0,1);
      lcd.print("forward & back!");
      delay(3000);

      while(gyroOutput != "left")
      {
        lcd.clear();
        gyroCheck();
        lcd.setCursor(0,0);
        lcd.print("Tilt left");
        lcd.setCursor(0,1);
        lcd.print("to try again");
      }
    }
    else
    {
       Serial.print("Forward State = ");
        for(int i = 0; i < 5; i++)
        {
          Serial.print(forwardState[i]);
        }
      
      Serial.println("");
      Serial.print("Backward State = ");
        for(int i = 0; i < 5; i++)
        {
          Serial.print(backwardState[i]);
        }
      Serial.println("");
      break;
    }

  }

}


