#include <SD.h> //Load SD card library
#include<SPI.h> //Load SPI Library 
#include <Adafruit_GPS.h>    //Install the adafruit GPS library
#include <SoftwareSerial.h> //Load the Software Serial library
SoftwareSerial mySerial(3,2); //Initialize the Software Serial port
Adafruit_GPS GPS(&mySerial); //Create the GPS Object
 
String NMEA1; //Variable for first NMEA sentence
String NMEA2; //Variable for second NMEA sentence
char c; //to read characters coming from the GPS
float deg; //Will hold positin data in simple degree format
float degWhole; //Variable for the whole part of position 
float degDec;  //Variable for the decimal part of degree
 
int chipSelect = 4; //chipSelect pin for the SD card Reader
File mySensorData; //Data object you will write your sensor data to
 
 
void setup() {
  
  Serial.begin(115200); //Turn on serial monitor
  GPS.begin(9600); //Turn on GPS at 9600 baud
  GPS.sendCommand("$PGCMD,33,0*6D");  //Turn off antenna update nuisance data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //Request RMC and GGA Sentences only
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); //Set update rate to 1 hz
  delay(1000); 
  
  pinMode(10, OUTPUT); //Must declare 10 an output and reserve it to keep SD card happy
  SD.begin(chipSelect); //Initialize the SD card reader
  
  if (SD.exists("NMEA.txt")) { //Delete old data files to start fresh
    SD.remove("NMEA.txt");
  }
  if (SD.exists("GPSData.txt")) { //Delete old data files to start fresh
    SD.remove("GPSData.txt");
  }
 
}
 
void loop() {
  
  readGPS();
 
  if(GPS.fix==1) { //Only save data if we have a fix
  mySensorData = SD.open("NMEA.txt", FILE_WRITE); //Open file on SD card for writing
  mySensorData.println(NMEA1); //Write first NMEA to SD card
  mySensorData.println(NMEA2); //Write Second NMEA to SD card
  mySensorData.close();  //Close the file
  
 
  mySensorData = SD.open("GPSData.txt", FILE_WRITE);
  
  degWhole=float(int(GPS.longitude/100)); //gives me the whole degree part of Longitude
  degDec = (GPS.longitude - degWhole*100)/60; //give me fractional part of longitude
  deg = degWhole + degDec; //Gives complete correct decimal form of Longitude degrees
  if (GPS.lon=='W') {  //If you are in Western Hemisphere, longitude degrees should be negative
    deg= (-1)*deg;
  }
  mySensorData.print(deg,4); //writing decimal degree longitude value to SD card
  mySensorData.print(","); //write comma to SD card
  
  degWhole=float(int(GPS.latitude/100)); 
  degDec = (GPS.latitude - degWhole*100)/60; 
  deg = degWhole + degDec; 
  if (GPS.lat=='S') { 
    deg= (-1)*deg;
  }
  mySensorData.print(deg,4); //writing decimal degree longitude value to SD card
  mySensorData.print(","); //write comma to SD card
  
  mySensorData.print(GPS.altitude); //write altitude to file
  mySensorData.print(" ");  //format with one white space to delimit data sets
 
  mySensorData.close();
  }
  
}
 
void readGPS() {
  
  clearGPS();
  while(!GPS.newNMEAreceived()) { 
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); 
  NMEA1=GPS.lastNMEA();
  
   while(!GPS.newNMEAreceived()) { 
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); 
  NMEA2=GPS.lastNMEA();
  
  Serial.println(NMEA1);
  Serial.println(NMEA2);
  Serial.println("");
  
}
 
void clearGPS() {  //Clear old and corrupt data from serial port 
  while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
  
  while(!GPS.newNMEAreceived()) { 
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); 
   while(!GPS.newNMEAreceived()) { 
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); 
  
}
