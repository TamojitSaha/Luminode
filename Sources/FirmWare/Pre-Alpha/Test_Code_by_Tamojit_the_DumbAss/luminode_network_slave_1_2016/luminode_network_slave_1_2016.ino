#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include <SPI.h>
/*
This sketch receives strings from sending unit via nrf24 
and prints them out via serial.  The sketch waits until
it receives a specific value (2 in this case), then it 
prints the complete message and clears the message buffer.
*/
int messageLength = 7;
int ledState=0;
int msg[1];
RF24 radio(9,10);
//const uint64_t pipe = 0xE8E8F0F0E1LL;
const uint64_t pipe1 = 0xE8E8F0F0E2LL;
int lastmsg = 1;
String theMessage = "";
int brightness;
void setup(void){
  Serial.begin(115200);
  radio.begin();
  radio.powerUp();
  radio.openReadingPipe(1,pipe1);
  radio.startListening();
}
void loop(void){
  if (radio.available())
  { delay(10); 
  
    
      
      radio.read(msg, 1); 
      char theChar = msg[0];
      if (msg[0] != 2){
        theMessage.concat(theChar);
        }
      else {
       if (theMessage.length() == messageLength) { 
       Serial.println(theMessage);
       if(theMessage== "whiteON")
        {
          digitalWrite(5,HIGH);
          ledState=1;
        }
      else if(theMessage=="whiteOF")
        {
          digitalWrite(5,LOW);
          ledState=0;
        }
       }
       theMessage= "";
       delay(10);
       radio.startListening(); 
      }

      radio.read(&brightness, sizeof(brightness));
   }
}

