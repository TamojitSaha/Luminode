#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"

#define debug

RF24 radio(9,10);
const uint64_t pipe1 = 0xE8E8F0F0E2LL;


void setup() 
{
  Serial.begin(115200);
  radio.begin();
  radio.powerUp();
  radio.openWritingPipe(pipe1);
  #ifdef debug
  Serial.println("Debug On");
  #endif
}

  void loop() 
{
  if(Serial.available())
  {
    processSerial();
  }
}


void processSerial()
{ 
  char temp[20]={};
  Serial.readBytes(temp,20);
  //Serial.readBytesUntil(' ', temp, 20);
  String buffer = String(temp);
  String s=String(buffer);
  #ifdef debug
  Serial.println();
  Serial.print("The received string from serial: ");
  Serial.println(s);
  #endif
  int buff_len=s.length();
  #ifdef debug
  Serial.print("Received buffer length: ");
  Serial.println(buff_len);
  #endif
  for(int i=0; i< buff_len; i++)
  {
  int charTosend[1];
  charTosend[0]=s.charAt(i);
  bool ok=radio.write(charTosend, 1);
  delay(10);
  if(ok)
  Serial.println("Data sent!!");
  else
  Serial.println("Plz Debug!!");
  }
  Serial.flush();
}

