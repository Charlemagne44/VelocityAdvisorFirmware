#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include <LiquidCrystal.h>


SoftwareSerial serial_connection(9, 8); //RX, TX (Reversed)
TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
bool lock = false; //switch for finding signal lock initially
const int buttonPin = 6;     // the number of the pushbutton pin
int buttonState = 0;


void setup()
{
  Serial.begin(9600);//This opens up communications to the Serial monitor in the Arduino IDE
  serial_connection.begin(9600);//This opens up communications to the GPS
  Serial.println("GPS Start");//Just show to the monitor that the sketch has started
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT);//for distance recording
  
}

void loop()
{
  buttonState = digitalRead(buttonPin);
  Serial.println("button state: ");
  //Serial.println(buttonState);

  while(serial_connection.available())//While there are characters to come from the GPS
  {
    gps.encode(serial_connection.read());//This feeds the serial NMEA data into the library one char at a time
  }
  if(gps.location.isUpdated())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {
    //check to see if signal was initially acquired
    if(lock == false) {
      lcd.clear();
      lcd.print("Signal found");
      delay(3000);
      lcd.clear();
      lock = true;
    }

    //calculate the split on 2 second intervals;
    //this interval should be slow enough for the 3-1min 500m range
    
    


    //print split information
    Serial.println(gps.speed.mph());
    float split = gps.speed.mph();
    split = 1 / split;
    split  =  split * 60;
    split  =  split  * 60;
    split  =  split  * (1  /  1.60934);
    split  =  split / 2;
    lcd.print("split: ");
    int minutes  =  int(split  /  60);
    int seconds  = int(split)  % 60;

    //only  print if  split  is less than 10  min
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
    lcd.setCursor(0,0);
  }
  else {
    //if lock switch is true then signal was found, THEN lost
    if(lock) {
     lcd.clear();
     lcd.print("Signal lost");
     delay(3000);
     lcd.clear(); 
    }
    lcd.print("Searching for");
    lcd.setCursor(0,1);
    lcd.print("signal");
    lcd.setCursor(0,0);
  }
}
