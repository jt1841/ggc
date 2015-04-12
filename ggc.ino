// ggc.ino
// Gesture Glove Controller
// ECE 4012 - Spring 2015
// Jonathan Talbott, Dan Russell, Justin Denard, Brian Nemsick, Erin Hanson
#include <Wire.h>
#include "flexSensors.h"
#include <LiquidCrystal.h>
//#include <TimerOne.h>
#include <EEPROM.h>

//EEPROM Value Storage
// Address //     Variable        //    
// 0       // flex0.threshold     //
// 1       // flex1.threshold     //
// 2       // flex2.threshold     //
// 3       // flex3.threshold     //
// 4       // flex4.threshold     //
// 5       // gyroLeftThreshold   //
// 6       // gyroRightThreshold  //
// 7       // forwardState[0]     //
// 8       // forwardState[1]     //
// 9       // forwardState[2]     //
// 10      // forwardState[3]     //
// 11      // forwardState[4]     //
// 12      // backwardState[0]    //
// 13      // backwardState[1]    //
// 14      // backwardState[2]    //
// 15      // backwardState[3]    //
// 16      // backwardState[4]    //

#define LEFTPIN 13
#define RIGHTPIN 12
#define FORPIN 11
#define BACKPIN 10
#define HIGHV 255
#define LOWV 0

using namespace std;

// Global Variables
flexSensors flex0(0,834,1004,EEPROM.read(0)); // Declare flex sensor
flexSensors flex1(1,922,1012,EEPROM.read(1));
flexSensors flex2(2,963,1010,EEPROM.read(2));
flexSensors flex3(3,874,999,EEPROM.read(3));
flexSensors flex4(4,842,1005,EEPROM.read(4));

/*  773-1004, 834
904-1012, 922
934-1010, 963
795-999, 874
768-1005, 842 */

const int MPU {0x68};  // I2C address of the MPU-6050
double gyroX {0.0};
int gyroLeftThreshold {EEPROM.read(5)};
int gyroRightThreshold {-EEPROM.read(6)};
int modeCount {0};
float debugGUI = 0;

boolean mode {1};
boolean left {0};
boolean right {0};
boolean forward {0};
boolean backward {0};
boolean forwardState[5] = {EEPROM.read(7),EEPROM.read(8),EEPROM.read(9),EEPROM.read(10),EEPROM.read(11)};
boolean backwardState[5] = {EEPROM.read(12),EEPROM.read(13),EEPROM.read(14),EEPROM.read(15),EEPROM.read(16)};
boolean flexState[5] = {0};

String processingOutput = "";

//Functions
void flexCheck();
void gyroCheck();
void processingSetup();
void processingCommand();
void PWMout();
void onOff();
void calibrationSetupLCD();
void gyroSetup();
void PWMSetup();
//LiquidCrystal lcd(8, 7, 4, 5, 1, 0);

void setup()
{  
  Serial.begin(9600);
  //while(!Serial){;}
  
  gyroSetup();
  PWMSetup();

  //Timer1 Setup
//Timer1.initialize(10000);
//  Timer1.attachInterrupt(processingSetup);

  //calibrationSetupLCD();
  while(!Serial){;}
  
  for (int i = 0; i < 16; i++)
  {
    Serial.print(EEPROM.read(i));
    Serial.print(",");
  }
  Serial.println();

}

void loop()
{
 // Serial.println(EEPROM.read(0));
  flexCheck();
  gyroCheck();
  PWMout(); //Call "check" functions before this
  processingSetup();
  processingCommand();

  //Debugging and Demo - Call after all other functions
  //Serial.println(flexOutput + " " + gyroOutput); // For Serial Manager Output
  //Serial.println((mode) + "," + (modeCount));
  // processingSetup(); // Output for Processing
  //Serial.println((flex0.threshold));

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
	  backward = 0;
          forward = 1;
    }
    else
    {
      forward = 0;
      break;
    }
  }

  if (forward != 1)
  {
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
   }
   
   if (forward) {backward == 0;}
   if (backward) {forward == 0;}

}

void gyroCheck()
{
  long int sampleGyro {0};

  //100 Samples of Gyro
  for (int i = 0; i < 100; i++)
  {
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(0x68,2,true);  // request a total of 14 registers
    sampleGyro += Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  }

  gyroX = sampleGyro/100.0; //Take average value

  if (gyroX > 15000)
    gyroX = 150000;
  else if (gyroX < -15000)
    gyroX = -15000;
    
  //Normalize to degrees
    gyroX = gyroX/15000 * 90;

  if(gyroX > gyroLeftThreshold)
  {
    left = 1;
    right = 0;
  }
  else if(gyroX < gyroRightThreshold)
  {
    right = 1;
    left = 0;
  }
  else
  {
    left = 0;
    right = 0;
  }


}

void processingSetup()
{
  
/*  processingOutput = "value," + \
  String(flex0.value) + "," + \
  String(flex1.value) + "," + \
  String(flex2.value) + "," + \
  String(flex3.value) + "," + \
  String(flex4.value) + "," + \
  String(gyroX);

  processingOutput = processingOutput + ",active," + \
  String(flex0.active) + "," + \
  String(flex1.active) + "," + \
  String(flex2.active) + "," + \
  String(flex3.active) + "," + \
  String(flex4.active);
  
  processingOutput = processingOutput + ",command," + \
  String(left) + "," + \
  String(right) + "," +\
  String(forward) + "," +\
  String(backward) + ",";
  Serial.println(processingOutput); */
  
  processingOutput = "value," + \
  String(debugGUI+1) + "," + \
  String(debugGUI+2) + "," + \
  String(debugGUI+3) + "," + \
  String(debugGUI+4) + "," + \
  String(debugGUI) + "," + \
  String((debugGUI - 50)/50 * 90 );

  processingOutput = processingOutput + ",active," + \
  String(flex0.active) + "," + \
  String(flex1.active) + "," + \
  String(flex2.active) + "," + \
  String(flex3.active) + "," + \
  String(flex4.active);
  
  processingOutput = processingOutput + ",command," + \
  String(left) + "," + \
  String(right) + "," +\
  String(forward) + "," +\
  String(backward);
  Serial.println(processingOutput);
  
 debugGUI++;
 
 if(debugGUI > 100)
    debugGUI = 0;
}

void PWMout()
{

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
// Add on off code for the button here
}

void gyroSetup()
{
  //Gyro Setup
  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); 
}

void PWMSetup()
{
  pinMode(LEFTPIN, OUTPUT);
  pinMode(RIGHTPIN, OUTPUT);
  pinMode(FORPIN, OUTPUT);
  pinMode(BACKPIN, OUTPUT); 
}

void readEEPROMData()
{

}

void processingCommand()
{
  if(Serial.available() > 0)
  {
  String inputString = Serial.readStringUntil('$');
  
  if (inputString.length() > 0)
  { 
  
  String checkStart = inputString.substring(0,2);
  
  if( checkStart == "!!") //Update Flex Values
  {
    String Flex0 = inputString.substring(2,4);
    String Flex1 = inputString.substring(4,6);
    String Flex2 = inputString.substring(6,8);
    String Flex3 = inputString.substring(8,10);
    String Flex4 = inputString.substring(10,12);
    
    EEPROM.write(0,Flex0.toInt());
    EEPROM.write(1,Flex1.toInt());
    EEPROM.write(2,Flex2.toInt());
    EEPROM.write(3,Flex3.toInt());
    EEPROM.write(4,Flex4.toInt());
    flex0.threshold = Flex0.toInt();
    flex1.threshold = Flex1.toInt();
    flex2.threshold = Flex2.toInt();
    flex3.threshold = Flex3.toInt();
    flex4.threshold = Flex4.toInt();
      
  }
  else if (checkStart == "##") //Update Gyro Values
  {
    String GyroLeft = inputString.substring(2,4);
    String GyroRight = inputString.substring(4,6);
    EEPROM.write(5,GyroLeft.toInt());
    EEPROM.write(6,GyroRight.toInt());
    gyroLeftThreshold = GyroLeft.toInt();
    gyroRightThreshold = -GyroRight.toInt(); 
  }
  else if (checkStart == "@@")
  {
    for (int i = 0; i < 5; i++)
    {
      forwardState[i] = inputString.substring(2 + i, 3 + i).toInt();
      EEPROM.write(i+7,forwardState[i]); 
    }
    
    for (int i = 0; i < 5; i++)
    {
      backwardState[i] = inputString.substring(7 + i, 8 + i).toInt();
      EEPROM.write(i+12,backwardState[i]); 
    }
    
  }
  
  
 }
  }
    
}



