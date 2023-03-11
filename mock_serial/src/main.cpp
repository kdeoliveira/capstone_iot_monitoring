#include <Arduino.h>
#include "Lora.hpp"
#include "protothreads.h"

// Arduino data types differs from default c++
// https://www.tutorialspoint.com/arduino/arduino_data_types.htm#:~:text=int%20stores%20a%2016%2Dbit,%5E15)%20%2D%201).

//Forward declaration
static void onReceiveCallback(Lora*);

static struct pt _ptLoraSend;

Lora lora1;


static int threadedReader(struct pt* pt, Lora& _lora, unsigned long interval, int id) {
  PT_BEGIN(pt);
  static unsigned long timestamp = 0;
  for(;;) {
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval);
    timestamp = millis();
    _lora.receive();
  }
  PT_END(pt);
}

static void onReceiveCallback(Lora* lora){
  
  //Serial.println("Lora callback...");
  //Serial.println(packetSize);
  int packetSize = lora->parsePacket();
  if(packetSize == 0)
    return;
  
  Serial.flush();
  while(lora->available()){
    Serial.write(lora->read());
  }
}

void setup() {
  
  Serial.begin(38400,SERIAL_8N1); //8 is data bit, 1 is stop bit and N is parity none
  while(!Serial);

  Serial.flush();
  lora1.begin(millis());
  lora1.onReceive(onReceiveCallback);

  PT_INIT(&_ptLoraSend);
  
}

void loop() {

  threadedReader(&_ptLoraSend, lora1, 1200, 1);
  lora1.send();
}


