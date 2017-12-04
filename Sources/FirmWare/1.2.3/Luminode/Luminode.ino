/*
  Communication framework for Luminode v1.2.3
  Author(s): Sandeepan Sengupta, Tamojit Saha
  Special thanks to: Sumant Banerjee, B.Tech (Electrical Engineering), West Bengal University of Technology


  Frame format:

  Inputs are 8 charecters in an UART stream
  Charector 1 is Identifier
  Charector 2~7 are Payload in HEX charectors
  Charector 8 is Termination indicator

  Registered identifier(s): '~', '@' and '#'
  Registered termination indicator(s): ';'

  '~' is identified as configuration data
  '@' is identified as Neopixel RGB address
  '#' is identified as RGB data

  RGB payload (both address and data) is 6 charectors HEX in 2+2+2 = 6 format (#000000; to #ffffff;)

  Configuration payload is converted into 24 charectors BIN from 6 charector HEX
  24 charectors BIN in in 1+8+(5+5+5) = 24 format
  1st bit (BIT_0) used to change LED State
  2nd to 9th bit (BIT_1 ~ BIT_8) is used to produce PWM code for intensity adjustment
  Rest of the bit(all 15 of them in 5+5+5 format) are used to feed shift registers secondary configuration data to drive 5x5x5 LED cube

  Accepted input formats are
  (Single frame)
  ~000000;
  @000000;
  #000000;
  etc...

  (Multiple frames without seperation)
  ~000000;@000000;#000000;
  ~A0b1C2;@f0E1d2;#9F8e7D;
  etc...

  Test Code : ~a0b1c2;#0a1B2c;@6C5b4A;#f9e8d7;


  Copyright (c) 2015-2016, Intelectron India (https://www.intelectron.in)
*/

#define id_length 1
#define code_length 6

#define PWM_res 8
#define BIT_width 5

#define _debug_

uint8_t RGB_pin[3] = {9, 10, 11};

bool Register[16] = {};
uint8_t payload[2], address[3], RGB_value[3], segment[5] = {};

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    ;
  }
  for (uint8_t j = 0; j < 3; j++)
  {
    pinMode(RGB_pin[j], OUTPUT);
  }
}

void loop()
{
  Serial.flush();
  while (Serial.available() != NULL)
  {
    char tmp[id_length + code_length] = {};
    const uint8_t _tmp = sizeof(tmp) / sizeof(char);

    Serial.readBytesUntil(';', tmp, _tmp);
    if (strlen(tmp) >= _tmp)
    {
      char id = NULL;
      boolean change = false;
      uint32_t code = NULL;
      bool bin[code_length * 4] = {}; //bool bin[strlen(tmp)] = {}; //strlen(tmp) consumes less space and works but I've no idea how or why.

      id = tmp[0];
      uint8_t i, j = NULL;
      String code_string = "";

      const uint8_t _segment = sizeof(segment) / sizeof(uint8_t);
      const uint8_t _payload = sizeof(payload) / sizeof(uint8_t);
      const uint8_t _Register = sizeof(Register) / sizeof(bool);

      for (i = id_length; i < _tmp; i++)
      {
        code_string = String(code_string + String(tmp[i]));
      }
      code = (uint32_t) strtol( &code_string[0], NULL, HEX);
#ifdef _debug_
      Serial.print("Entered string data is\t\t:\t'");
      Serial.print(id);
      Serial.print(code_string);
      Serial.println("'");

      Serial.print("Rectified code (in HEX) is\t:\t");
      Serial.print(code, HEX);
      Serial.println("\n");

      Serial.print("Configuration code in DEC\t:\t");
      Serial.println(code, DEC);
      Serial.print("Configuration code in BIN\t:\t");
      Serial.println(code, BIN);
#endif

      switch (id)
      {
        case '~':
          for (i = 0; i < _segment; i++)
          {
            segment[i] = NULL;
          }

          while (code > 0)
          {
            bin[j] = code % 2;
#ifdef _debug_
            Serial.print("Bit number ");
            Serial.print(j + 1);
            Serial.print(" or BIT_");
            Serial.print(j);
            Serial.print(" is\t:\t");
            Serial.println(bin[j]);
#endif
            j++;
            code = code / 2;
          }

          segment[0] = bin[0];
          for (i = 0; i < PWM_res; i++)
          {
            segment[1] += round(bin[i + id_length] * pow(2, i));
          }
          for (j = 0 ; j < 3; j++)
          {
            for (i = 0; i < BIT_width; i++)
            {
              segment[2 + j] += round(bin[i + 1 + PWM_res + BIT_width * j] * pow(2, i));
            }
          }
          for (i = 0; i < BIT_width * 3; i++)
          {
            Register[i + 1] = bin[i + 1 + PWM_res];
          }
          for (j = 0; j < _payload; j++)
          {
            payload[j] = NULL;
            for (i = 0; i < _Register / 2; i++)
            {
              payload[j] += round(Register[i + (_Register / 2) * j] * pow(2, i));
            }
          }
#ifdef _debug_
          Serial.print("\nConfiguration frame (in BIN) is\t:\t[");
          Serial.print(segment[0], BIN);
          Serial.print(",");
          Serial.print(segment[1], BIN);
          Serial.print(",(");
          Serial.print(segment[2], BIN);
          Serial.print(",");
          Serial.print(segment[3], BIN);
          Serial.print(",");
          Serial.print(segment[4], BIN);
          Serial.println(")]");
          Serial.print("Configuration frame (in HEX) is\t:\t[");
          Serial.print(segment[0], HEX);
          Serial.print(",");
          Serial.print(segment[1], HEX);
          Serial.print(",(");
          Serial.print(segment[2], HEX);
          Serial.print(",");
          Serial.print(segment[3], HEX);
          Serial.print(",");
          Serial.print(segment[4], HEX);
          Serial.println(")]");
          Serial.print("System state is\t\t\t:\t");
          Serial.println(segment[0]);
          Serial.print("Configuration value is\t\t:\t");
          Serial.println(segment[1], HEX);
          Serial.print("5x5x5 LED cube configuration is\t:\t(");
          Serial.print(segment[2]);
          Serial.print(",");
          Serial.print(segment[3]);
          Serial.print(",");
          Serial.print(segment[4]);
          Serial.println(")");
          Serial.print("RGB state is\t\t\t:\t");
          Serial.println(Register[0]);
          Serial.print("First shift register code is\t:\t");
          Serial.println(payload[0], HEX);
          Serial.print("Second shift register code is\t:\t");
          Serial.println(payload[1], HEX);
          Serial.println("\n");
#endif
          break;

        case '@':
          address[0] = code >> 16;
          address[1] = code >> 8 & 0xFF;
          address[2] = code & 0xFF;
#ifdef _debug_
          Serial.print("Address of X axis\t\t:\t");
          Serial.println(address[0]);
          Serial.print("Address of Y axis\t\t:\t");
          Serial.println(address[1]);
          Serial.print("Address of Z axis\t\t:\t");
          Serial.println(address[2]);
          Serial.print("Neopixel address code set is\t:\t(");
          Serial.print(address[0], HEX);
          Serial.print(",");
          Serial.print(address[1], HEX);
          Serial.print(",");
          Serial.print(address[2], HEX);
          Serial.println(")");
          Serial.print("RGB color value set is\t\t:\t(");
          Serial.print(RGB_value[0], HEX);
          Serial.print(",");
          Serial.print(RGB_value[1], HEX);
          Serial.print(",");
          Serial.print(RGB_value[2], HEX);
          Serial.println(")");
          Serial.println("\n");
#endif
          break;

        case '#':
          RGB_value[0] = code >> 16;
          RGB_value[1] = code >> 8 & 0xFF;
          RGB_value[2] = code & 0xFF;

          if (RGB_value[0] != NULL || RGB_value[1] != NULL || RGB_value[2] != NULL)
          {
            Register[0] = HIGH;
          }
          else
          {
            Register[0] = LOW;
          }
          for (j = 0; j < _payload; j++)
          {
            payload[j] = NULL;
            for (i = 0; i < _Register / 2; i++)
            {
              payload[j] += round(Register[i + (_Register / 2) * j] * pow(2, i));
            }
          }
#ifdef _debug_
          Serial.print("RED is\t\t\t\t:\t");
          Serial.println(RGB_value[0]);
          Serial.print("GREEN is\t\t\t:\t");
          Serial.println(RGB_value[1]);
          Serial.print("BLUE is\t\t\t\t:\t");
          Serial.println(RGB_value[2]);
          Serial.print("Neopixel address code set is\t:\t(");
          Serial.print(address[0], HEX);
          Serial.print(",");
          Serial.print(address[1], HEX);
          Serial.print(",");
          Serial.print(address[2], HEX);
          Serial.println(")");
          Serial.print("RGB color value set is\t\t:\t(");
          Serial.print(RGB_value[0], HEX);
          Serial.print(",");
          Serial.print(RGB_value[1], HEX);
          Serial.print(",");
          Serial.print(RGB_value[2], HEX);
          Serial.println(")");
          Serial.print("RGB state is\t\t\t:\t");
          Serial.println(Register[0]);
          Serial.print("First shift register code is\t:\t");
          Serial.println(payload[0], HEX);
          Serial.print("Second shift register code is\t:\t");
          Serial.println(payload[1], HEX);
          Serial.println("\n");
#endif
          for (j = 0; j < 3; j++)
          {
            analogWrite(RGB_pin[j], RGB_value[j]);
          }
          break;

        default:
          exit(NULL);
          break;
      }
    }
  }
}
