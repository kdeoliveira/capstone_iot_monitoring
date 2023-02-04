#include <Arduino.h>
#include "data.hpp"


// Arduino data types differs from default c++
// https://www.tutorialspoint.com/arduino/arduino_data_types.htm#:~:text=int%20stores%20a%2016%2Dbit,%5E15)%20%2D%201).
void serialize(iot_monitoring::data::packet<int, float> input, unsigned char* output){
  int* id = (int*)output;
  *id = input._header.id;
  id++;
  float* p = (float*)id;
  *p = input.payload;
}

iot_monitoring::data::packet<int, float> _packet;


unsigned char* data = new unsigned char[sizeof(int) + sizeof(float)];  
void setup() {
  
  Serial.begin(38400,SERIAL_8N1); //8 is data bit, 1 is stop bit and N is parity none
  while(!Serial);

  randomSeed(10);
}


int i = 0;
void loop() {

  _packet = i % 4;
  _packet << random(100)*0.5f;
  serialize(_packet, data);
  Serial.write("\r\n");
  Serial.write(data, sizeof(int) + sizeof(float));
  
  //Serial.println("test1\n\r");
  i++;
  delay(500);
}


