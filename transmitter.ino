#include <TimerOne.h>
#include <util/atomic.h>
#include<LiquidCrystal.h>
LiquidCrystal LCD(10,9,5,4,3,2);
int echoPin=6;
int trigPin=12;
float pingTime;
float targetDistance;
float speedOfSound=1262.8;
float sPeed;
int buzzerPin=11;
String bufferSpace;
//Start of what should be an include ...


//#define TRANSMIT_SERIAL


// change to alter communication speed, 
// will lower values will result in faster communication
// the receiver must be tuned to the same value
#define SYMBOL_PERIOD 500 /* Defined a symbol period in us*/

#define WORD_LENGTH 10 /* Each byte is encoded on 10-bit with start, byte, stop */
#define SYNC_SYMBOL 0xD5 /* Synchronization symbol to send after a preamble, before data communication happens */
#define ETX 0x03  //end frame.
#define STX 0x02  //start frame.

//Fast manipulation of LED IO. 
//These defines are for a LED connected on D13
#define OUT_LED() DDRB |= (1 << 5);
#define SET_LED() PORTB |= (1 << 5)
#define CLR_LED() PORTB &= ~(1 << 5)
unsigned char frame_buffer [38] ; //buffer for frame
char frame_index = -1; // index in frame
char frame_size = -1  ; // size of the frame to be sent
//state variables of the manchester encoder
unsigned char bit_counter = 0 ;
unsigned short data_word = 0 ;  //8bit data + start + stop
unsigned char half_bit = 0 ;
unsigned long int manchester_data ;
void to_manchester(unsigned char data, unsigned long int * data_manchester){
  unsigned int i ;
 (*data_manchester) = 0x02 ; // STOP symbol
 (*data_manchester) = (*data_manchester) << 2 ;
  for(i = 0 ; i < 8; i ++){
    if(data & 0x80) (*data_manchester) |=  0x02  ; // data LSB first
    else (*data_manchester) |= 0x01 ;
    (*data_manchester) = (*data_manchester) << 2 ;
    data = data << 1 ; // to next bit
  }
  (*data_manchester) |= 0x01 ; //START symbol
}

//emitter interrupt
void emit_half_bit(){
     if(manchester_data & 0x01){
       SET_LED();
     }else{
       CLR_LED();
     }
     bit_counter -- ;
     manchester_data = (manchester_data >> 1);
     if(bit_counter == 0){   
        //is there still bytes to send in the frame ?
        manchester_data = 0xAAAAAAAA ; // keep sending ones if nothing to send
        if(frame_index >= 0 ){
          if(frame_index < frame_size){
            /*Serial.println(frame_index, DEC);
            Serial.println(frame_buffer[frame_index], HEX);*/
            to_manchester(frame_buffer[frame_index], &manchester_data);
            frame_index ++ ;
          }else{
            frame_index = -1 ;
            frame_size = -1 ;
          }
        }
        bit_counter = WORD_LENGTH * 2 ;
        //Serial.println(manchester_data, BIN);
      }
}

void init_frame(unsigned char * frame){
  memset(frame, 0xAA, 3);
  frame[3] = SYNC_SYMBOL ;
  frame[4] = STX;
  frame_index = -1 ;
  frame_size = -1 ;
}

int create_frame(char * data, int data_size, unsigned char * frame){
  memcpy(&(frame[5]), data, data_size);
  frame[5+data_size] = ETX;
  return 1 ;
}

int write(char * data, int data_size){
  if(frame_index >=  0) return -1 ;
  if(data_size > 32) return -1 ;
  create_frame(data, data_size,frame_buffer);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    frame_index = 0 ;
    frame_size = data_size + 6 ;
  }
  return 0 ;
}

int transmitter_available(){
  if(frame_index >=  0) return 0 ;
  return 1 ; 
}

void init_emitter(){
  manchester_data = 0xFFFFFFFF ;
  bit_counter = WORD_LENGTH * 2 ;
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  LCD.begin(16,2);
  LCD.setCursor(0,0);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  pinMode(buzzerPin,OUTPUT);
  LCD.print("Status:");
  OUT_LED();
  init_frame(frame_buffer);
  init_emitter();
  Timer1.initialize(SYMBOL_PERIOD); //1200 bauds
  Timer1.attachInterrupt(emit_half_bit); 
}


// the loop routine runs over and over again forever:
float * msg ;
char com_buffer [32] ;
char com_buffer_nb_bytes = 0 ;
void loop() {


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
       sPeed=targetDistance*0.9;
       *msg=sPeed;
       Serial.println(targetDistance);
       if(targetDistance<100)
        {
          LCD.setCursor(0,1);
          LCD.print("                ");
          LCD.setCursor(0,1);
          LCD.print("ALERT!");
          delay(15);
          buzzerOn();
          delay(100);
                     
        }

       
  #ifdef TRANSMIT_SERIAL
  if(Serial.available() && transmitter_available()){ //constructing the data frame only if transmitter is ready to transmit
    char c = Serial.read();
    com_buffer[com_buffer_nb_bytes] = c ;
    com_buffer_nb_bytes ++ ;
    if(com_buffer_nb_bytes >= 32 || c == '\n'){
      if(write(com_buffer, com_buffer_nb_bytes) < 0){
        Serial.println("Transmitter is busy");
      }else{
        com_buffer_nb_bytes = 0 ;
      }
    }
  }
  delay(10);
  #else
    static int i = 0 ;
    memcpy(com_buffer, msg, 11);
    com_buffer[11] = i + '0' ;
    if(write(com_buffer, 12) < 0){
      delay(10);
    }else{
      i ++ ; 
      if(i > 9) i = 0 ;
    }
  #endif
}
void buzzerOn()
 {
   for(int i=0; i<3;i++)
    {
      digitalWrite(buzzerPin,HIGH);
      delay(10);
    }
 }
