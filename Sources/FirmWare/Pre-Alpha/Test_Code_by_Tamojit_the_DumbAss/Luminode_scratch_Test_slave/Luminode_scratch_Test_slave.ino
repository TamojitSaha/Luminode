#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"

RF24 radio(9,10);
//const uint64_t pipe = 0xE8E8F0F0E1LL;
const uint64_t pipe1 = 0xE8E8F0F0E2LL;

void setup()
{
 Serial.begin(115200);
  radio.begin();
  radio.powerUp();
  radio.openReadingPipe(1,pipe1);
  radio.startListening();

}

void loop()
{
 if (radio.available())
  { String s="";
    char temp[20]={};
    radio.read(&temp, sizeof(temp));
    Serial.println(temp);
    /*
    if (temp[0]= '$')
    {
      processWhite(temp);
    }
    */
  }

// void processWhite(char )
  
}
