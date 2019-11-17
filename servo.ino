//servo controlling with angle change according to distance detected by the ultrasonic sensor.
#include<Servo.h> //Declare a servo library.
int servoPin=9;// select the servo controller pin and this pin should be an pwm pin.
Servo motorObject; // creating servo object.
int maxSpeed=110;
int angleRotate;
int servoPos;
void setup()
 {
   Serial.begin(9600); // set the baud rate.
   motorObject.attach(servoPin); //  identify the servo control pin.
   motorObject.write(160);      // calibrate the value of the servo to maximum speed i.e 110 km/hr.
   
 }
void loop()
 {
   if(distanceValue>=400)
    {
      angleRotate=15/10(70)+10;
      motorObject.write(angleRotate);
    }

   else if(distanceValue<400&&distanceValue>=300)
    {

      angleRotate=15/10(50)+10;
      motorObject.write(angleRotate);
    }

   else if(distanceValue<300&&distanceValue>=200)
    {
      angleRotate=15/10(30)+10;
      motorObject.write(angleRotate);
    }

   else if(distanceValue<200&&distanceVAlue>=100)
    {
      angleRotate=15/10(10)+10;
      motorObject.write(angleRotate);
    }

   else
     {
      angleRotate=10;
      motorObject.write(angleRotate);
     }
   
 }
