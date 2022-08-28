#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include <math.h>
#define pi 3.14159265358979323846
#include <LiquidCrystal.h>
#include <Wire.h>
#include <MPU6050.h>


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int buttonPin = 6;     // the number of the pushbutton pin
int buttonState = 0;
int prevButtonState = 1;
float mToMiConversion = 0.000621371;
bool lockOn = false;
bool firstRun = true;
float prevLat;
float prevLng;
int mTraveled = 0;
int recordingState = 0;
unsigned long previousMillis = 0;
const long interval = 3000;

SoftwareSerial serial_connection(9, 8); //RX, TX (Reversed)
TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
MPU6050 mpu;

void setup()
{
  //Begin Serial Connection
  Serial.begin(9600);//This opens up communications to the Serial monitor in the Arduino IDE
  serial_connection.begin(9600);//This opens up communications to the GPS
  //Initialize GPS pins
  Serial.println("GPS Start");//Just show to the monitor that the sketch has started
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT);//for distance recording
  //Initialize Accelerometer
  Serial.println("MPU6050 Accelerometer Start");
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find valid MPU6050, check wiring");
  }
  //Set offsets if needed - could be needed if not firmly fastened to prototype
  //mpu.setAccelOffsetX();
  //mpu.setAccelOffsetY();
  //mpu.setAccelOffsetZ();
  //Show accelerometer stats
  checkSettings();
}

void loop()
{

  //create vectors for acceleration tracking
  Vector rawAccel = mpu.readRawAccel();
  Vector normAccel = mpu.readNormalizeAccel();

  //keep track of seconds passed for distance calc
  unsigned long currentMillis = millis();

  //button state
  buttonState = digitalRead(buttonPin);

  //if first run, give the initial GPS coords
  if(firstRun) {
    prevLat = gps.location.lat();
    prevLng = gps.location.lng();
    mTraveled = 0;
    firstRun = false;
  }

  //While there are characters to come from the GPS
  while(serial_connection.available())
  {
    //This feeds the serial NMEA data into the library one char at a time
    gps.encode(serial_connection.read());
  }
  //This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  if(gps.location.isUpdated())
  {
    // clear signal search
    if(lockOn == false) {
      lockOn = true;
      lcd.clear();
    }

//if button goes from 0 to 1, that is a button press and we rotate states
  if(buttonState == 1 && prevButtonState == 0) {
    if(recordingState == 0) {
      recordingState = 1;
      mTraveled = 0;
    }
    else if(recordingState == 1) {
      recordingState = 2;
      lcd.setCursor(15,1);
      lcd.print("P");
      lcd.setCursor(0,0);
    }
    else {
      recordingState = 0;
      mTraveled = 0;
      lcd.clear();
    }
    Serial.println("Recording state:");
    Serial.println(recordingState);
    
  }

      if(recordingState == 1) {
        lcd.setCursor(15,1);
        lcd.print("R");
        lcd.setCursor(0,0);
        if(currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          //do calc and add
          mTraveled += TinyGPSPlus::distanceBetween(
            gps.location.lat(),
            gps.location.lng(),
            prevLat,
            prevLng
         );
         prevLat = gps.location.lat();
         prevLng = gps.location.lng();
        }
      }
      
    
    //calculate split information
    float split = gps.speed.mph();
    split = 1 / split;
    split  =  split * 60;
    split  =  split  * 60;
    split  =  split  * (1  /  1.60934);
    split  =  split / 2;
    lcd.print("split: ");
    int minutes  =  int(split  /  60);
    int seconds  = int(split)  % 60;

    //calculate stroke rate information
    

    //print split info
    if(minutes  < 10) {
      lcd.print(minutes);
      lcd.print(":");
      if(seconds > 0) {
        lcd.print(seconds);
      }
    }
    else {
      lcd.print("-:--");
    }
    lcd.setCursor(0,1);
    lcd.print("m: ");
    lcd.print(mTraveled);
    lcd.setCursor(0,0);
    
  }
  else {
    if(lockOn == false) {
      lcd.print("Searching for");
      lcd.setCursor(0,1);
      lcd.print("signal");
      lcd.setCursor(0,0);
    } 
  }
  prevButtonState = buttonState;
}


void checkSettings()
{
  Serial.println();
  
  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  
  Serial.print(" * Clock Source:          ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:         ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());
  
  Serial.println();
}
