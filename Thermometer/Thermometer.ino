#include <SoftwareSerial.h>
// include the library codes for I2C, LCD, and RTC:
#include "Wire.h"
#include "RTClib.h"

// this delcaration allows communication with the RTC chip
RTC_DS1307 RTC;

// Create a software serial port!
// first parameter as input and the second as output
// http://arduino.cc/en/Reference/SoftwareSerial
SoftwareSerial lcd = SoftwareSerial(0,2); 
// this constant shows where we'll find the temp sensor
const int sensorPin = A1;   // thermometer must be an analog pin !!
const int buttonPin = 13;     // the number of the pushbutton pin

// these variables are used to calculate the temperature
int sensorValue;  // raw analog data
int sensorMV;     // corresponding voltage
float tempCent;   // degrees Fahrenheit
float tempFahr;   // degrees Centigrade
float red;
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int displayedDegC = 0;

void setup() {
  lcd.begin(9600);  
  // setup the serial port for sending data to the serial monitor or host computer
  Serial.begin(9600);
  
  Wire.begin();  // for I2C
  RTC.begin();  // the clock
  
  // setup the pin for the temp sensor input
  pinMode(sensorPin, INPUT); 
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);  
  
  // set the size of the display if it isn't 16x2 (you only have to do this once)
  // set LCD size
  lcd.write(0xFE);
  lcd.write(0xD1);
  lcd.write(16);  // 16 columns
  lcd.write(2);   // 2 rows
  delay(10);       
  // we suggest putting delays after each command to make sure the data 
  // is sent and the LCD is updated.

  // set the contrast, 200 is a good place to start, adjust as desired
  lcd.write(0xFE);
  lcd.write(0x50);
  lcd.write(200);
  delay(10);       
  
  // set the brightness - we'll max it (255 is max brightness)
  lcd.write(0xFE);
  lcd.write(0x99);
  lcd.write(255);
  delay(10);       
  
  // turn off cursors
  lcd.write(0xFE);
  lcd.write(0x4B);  // turn off the underline cursor
  lcd.write(0xFE);
  lcd.write(0x54);  // turn off the blinking block cursor
  
//  // create a custom character
//  lcd.write(0xFE);
//  lcd.write(0x4E);  // create custom character
//  lcd.write((uint8_t)0);     // location #0
//  lcd.write((uint8_t)0x00);  // 8 bytes of character data
//  lcd.write(0x0A);
//  lcd.write(0x15);
//  lcd.write(0x11);
//  lcd.write(0x11);
//  lcd.write(0x0A);
//  lcd.write(0x04);
//  lcd.write((uint8_t)0x00);
//  delay(10);   // we suggest putting delays after each command 
  
  // clear screen
  lcd.write(0xFE);
  lcd.write(0x58);
  delay(10);   // we suggest putting delays after each command 
  
  // go 'home'
  lcd.write(0xFE);
  lcd.write(0x48);
  delay(10);   // we suggest putting delays after each command 

//  lcd.print("We ");
//  lcd.write((uint8_t)0);            // to print the custom character, 'write' the location
//  lcd.println(" Arduino!");
//  lcd.print("     - Adafruit");
  
  delay(1000);
}

//uint8_t red, green, blue;
void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH){
    displayedDegC = !displayedDegC;
  }
  
  DateTime rightNow = RTC.now();
  int curYear = rightNow.year();
  int curMonth = rightNow.month();
  int curDay = rightNow.day();
  int curHour = rightNow.hour();
  int curMin = rightNow.minute();
  int curSec = rightNow.second();
  int curWeekday = rightNow.dayOfWeek();
  
  // reformat it into a single string that looks like "mm/dd/yyyy hh:mm:sec"
  char string[20];  // a string with 19 characters in it (it has to be one longer than you need)

  sprintf(string, "%02d/%02d/%04d %02d:%02d", 
  curMonth, curDay, curYear, curHour, curMin);
  
  // read the raw value of the temp sensor
  sensorValue = analogRead(sensorPin);
  // an analog sensor on the Arduino returns a value between 0 and 1023, depending on the strength
  // of the signal it reads. (this is because the Arduino has a 10-bit analog to digital converter.)
    
  // first, convert the sensor reading to the right voltage level (5000 mV, since the Arduino port is +5V)
  // the map function is just an efficient shortcut for some ratio math
  sensorMV = map(sensorValue, 0, 1023, 0, 4999);
  
  // now convert this voltage to degrees, both Fahrenheit and Centigrade
  // this calculation comes from the spec of the specific sensor we are using
  tempCent = (sensorMV - 500) / 10.0;
  tempFahr = (tempCent * 9 / 5.0) + 32;
  
  // go 'home'
  lcd.write(0xFE);
  lcd.write(0x48);
  
  // print time
  lcd.print(string);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (displayedDegC == true) {     
    // print temperature in deg c   
    lcd.print(tempCent);
    lcd.print(" deg C   "); 
  } 
  else {
    // print temperature in deg f
    lcd.print(tempFahr);
    lcd.print(" deg F   "); 
  }

  // write these temperatures out to the serial monitor so we can see them
  Serial.println(string);
  Serial.print(tempFahr);
  Serial.print(" deg F  ");
  Serial.print(tempCent);
  Serial.print(" deg C");
  Serial.println();
  tempCent = constrain(tempCent, 15, 30);
  red = (tempCent-15)/15*255;
  lcd.write(0xFE);
  lcd.write(0xD0);
  lcd.write(red); 
  lcd.write(255-red);
  lcd.write((uint8_t)0);
  delay(500);   // we suggest putting delays after each command    
}
