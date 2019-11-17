#include<LiquidCrystal.h> 
LiquidCrystal LCD(10,9,5,4,3,2);
int echoPin=6;
int trigPin=13;
int analogPin=A0;
float pingTime;
float targetDistance;
float speedOfSound=1262;
int sPeed;
int buzzerPin =3;
int bNum=0;

void setup() {

        Serial.begin(9600);
        LCD.begin(16,2);
        LCD.setCursor(0,0);
        pinMode(trigPin,OUTPUT);
        pinMode(echoPin,INPUT);
        pinMode(analogPin,OUTPUT);
        pinMode(buzzerPin,OUTPUT);
        LCD.print("status:");

}

void loop() {

       //we need to caliber the ultasonic to get pingTime;

       digitalWrite(trigPin,LOW);
       delayMicroseconds(2000);
       digitalWrite(trigPin,HIGH);
       delayMicroseconds(20);
       digitalWrite(trigPin,LOW);
       delayMicroseconds(10);

       pingTime=pulseIn(echoPin,HIGH);
       pingTime=pingTime/1000000;
       pingTime=pingTime/3600;

       targetDistance=speedOfSound*pingTime;
       targetDistance=targetDistance/2;
       targetDistance=targetDistance*100000;

       
        
         if(targetDistance<100)
          {
           LCD.setCursor(0,1);
           LCD.print("                ");
           LCD.setCursor(0,1);
           LCD.print("ALERT!");
           LCD.setCursor(12,1);
           LCD.print(sPeed); 
           analogWrite(analogPin,1023);
           delay(250);
           analogWrite(analogPin,0);
           delay(250);
           while(bNum<3)
           digitalWrite(buzzerPin,HIGH);
           delay(100);
          }
     
        else
         {
          LCD.setCursor(0,1);
           LCD.print("                ");
           LCD.setCursor(0,1);
           LCD.print("SAFE ZONE$");
           delay(100);
           
            
         }
        

}
