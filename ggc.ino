// ggc.ino
// Gesture Glove Controller
// ECE 4012 - Spring 2015
// Jonathan Talbott, Dan Russell, Justin Denard, Brian Nemsick, Erin Hanson

void setup()
{
  Serial.begin(9600);	// Initialize Serial Communication and set 
                        // Rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200 bits/second
  pinMode(13, OUTPUT);
}

void loop()
{
  int sensor, degrees;
  
  sensor = analogRead(0);	// Read voltage from analog input 0
  degrees = map(sensor, 768, 853, 0, 90);	// Convert reading to degrees. Min and max values will be unique to each flex sensor

  Serial.print("analog input: "); // Print results for debugging
  Serial.print(sensor,DEC);
  Serial.print("   degrees: ");
  Serial.println(degrees,DEC);
  delay(10);	// Delay in ms before next reading
  
  //Code for outputting HIGH and LOW voltage values depending on the flex sensors degree reading 
  if (degrees > 30)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
  
}