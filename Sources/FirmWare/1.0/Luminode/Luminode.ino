/*
Luminode Firmware v1.0

Developed by Tamojit Saha and Sandeepan Sengupta
Baccalaureus Technologie, Electronics and Communication Enginering, Techno India University

Special Thanks to:
Sumant Banerjee
Baccalaureus Technologie, Electrical Engineering, West Bengal University of Technology

Copyright (c) 2015-2016
*/

#define WHITE   1
#define SLIDER  2
#define RGB     3
#define COLOR   4

//comment the following line to disable debugging mode
#define __DEBUG_

int redPin=0;
int greenPin=0;
int bluePin=0;

int whitePin=0;

int RGBstate=0;
int WHITEstate=0;

String codes[] = {"", "WHITE", "SLIDER", "RGB", "COLOR"};

void setup()
{
  Serial.begin(250000);
  #ifdef __DEBUG_
  Serial.println("DEBUG mode ON");
  #endif
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(bluePin,OUTPUT);
  pinMode(whitePin,OUTPUT);
}

void loop() {
  if(Serial.available())
  {
    processSerial();
  }
}
void processSerial()
{
  char _temp[20] = {};
  int code = -1;
  Serial.readBytesUntil('#', _temp, 20);
  String buffer = String(_temp);
  //check which code was sent
  switch (buffer[0])
  {
    case '$':
      code = WHITE;
      break;
    case '@':
      code = RGB;
      break;
    case '!':
      code = SLIDER;
      break;
    case '%':
      code = COLOR;
      break;
  }
  buffer.remove(0, 1); //removes the identifier
  processSerialData(buffer, code);
  #ifdef __DEBUG_
  Serial.println(codes[code]);
  #endif
  Serial.flush();
}
void processSerialData(String buffer, int code)
  {
  switch (code)
  {
    case WHITE:
      processWhite(buffer);
      break;
    case RGB:
      processRGB(buffer);
      break;
    case SLIDER:
      processSlider(buffer);
      break;
    case COLOR:
      processColor(buffer);
      break;
  }
}
void processWhite(String buffer)
{
  char state = buffer[0];
  if (state == '1')
  {
    buffer.remove(0, 2);
    WHITEstate=1;
    //TURN ON WHITE LED
    int brightness = buffer.toInt();
    #ifdef __DEBUG_
    Serial.println("White LED turned ON");
    Serial.print("brightness");
    Serial.println(brightness);
    #endif
    analogWrite(whitePin,brightness);
  }
  else if (state == '0')
  {
    //turn off white led
    WHITEstate=0;
    analogWrite(whitePin,0);
    #ifdef __DEBUG_
    Serial.println("White LED turned OFF");
    #endif
  }
}
void processRGB(String buffer)
{
  char state = buffer[0];
  if (state == '1')
  {
    //turn on rgb
    RGBstate=1;
    #ifdef __DEBUG_
    Serial.println("RGB turned ON");
    #endif
  }
  else if (state == '0')
  {
    //turn off rgb
    RGBstate=0;
    #ifdef __DEBUG_
    Serial.println("RGB turned OFF");
    #endif
  }
}
void processSlider(String buffer)
{
  int brightness = buffer.toInt();
  #ifdef __DEBUG_
  Serial.print("Brightness set to ");
  Serial.println(brightness);
  if(WHITEstate==1)
    analogWrite(whitePin,brightness); //write brightness value to white LED
  #endif
}
void processColor(String buffer)
{
  //process your RGB string 255,255,255
  #ifdef __DEBUG_
  Serial.println(buffer);
  #endif
  String red="",green="",blue="";
  
  int i1=buffer.indexOf(',');
  int i2=buffer.indexOf(',',i1+1);

  for(int i=0;i<i1;i++)
  {
    red+=buffer[i];
  }
  for(int i=i1+1;i<i2;i++)
  {
    green+=buffer[i];
  }
  buffer.remove(0,i2+1);
  int r=red.toInt();
  int g=green.toInt();
  int b=buffer.toInt();
  #ifdef __DEBUG_
  Serial.print("Red: ");
  Serial.println(r);
  Serial.print("Green: ");
  Serial.println(g);
  Serial.print("Blue: ");
  Serial.println(b);
  #endif
  if(RGBstate==1)
  {
    setRGB(r,g,b);  
  }
}
void setRGB(int _r, int _g, int _b)
{
  analogWrite(redPin,_r);
  analogWrite(greenPin,_g);
  analogWrite(bluePin,_b);
  #ifdef __DEBUG_
  Serial.print("Set Color");
  #endif
}
