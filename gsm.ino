#include<SoftwareSerial.h> //include header file 
SoftwareSerial sim(10,11);// create additional serial ports
int timeOut;              //time counter
String bufferSpace;     //helps in storing data
String number ="+919790019680"; //contact number

void setup()
 {
   delay(7000);
   Serial.begin(9600);
   bufferSpace.reserve(50);
   Serial.println("Started Initializing:");
   sim.begin(9600);
   delay(1000);
   Serial.println("Type s to send sms,r to receive sms and c to make call");
   
   
 }
void loop()
 {
   if(Serial.available()>0)
    switch(Serial.read())
      { 
        case 's':
         sendMessage();
         break;

         case 'r':
         ReceiveMessage();
         break;

         case 'c':
         callNumber();
         break;
       
      }
     if(sim.available()>0)
      Serial.write(sim.read());

  
 }
void sendMessage()
 {

   Serial.println("Sending Message:");
   sim.println("AT+CMGF=1"); //sets th egsm module to text mode
   delay(1000);
   sim.println("AT+CMGS=\""+number+"\"\r"); // set sms number.
   delay(1000);
   String SMS ="Caution a head!";
   sim.println(SMS);
   delay(100);
   sim.println((char)26);
   delay(1000);
   bufferSpace= readSerial();
 }
void ReceiveMessage()
 {
   Serial.println("TO read sms:");
   delay(1000);
   sim.print("AT+CNMI=2,2,0,0,0"); //command to receive sms.
   delay(1000);
   Serial.write("Unread Message done");
   
 }
String readSerial()
 {
   int timeOut=0;
   while(!sim.available()&&timeOut<12000)
    {
      delay(12);
      timeOut++;
    }
    if(sim.available())
     
       return sim.readString();
     
    
   
 }
void callNumber() {
  sim.print (F("ATD"));
  sim.print (number);
  sim.print (F(";\r\n"));
  bufferSpace = readSerial();
  Serial.println(bufferSpace);
}
