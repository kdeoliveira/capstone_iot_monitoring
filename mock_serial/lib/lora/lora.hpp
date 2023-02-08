#pragma once
#include <Arduino.h>

class Lora{
private:
    void (*_onReceive)(Lora*);
    String _buffer;
    int _id;
    bool bufferCompleted = false;
public:
    void begin(long);
    void onReceive(void(*)(Lora*));
    void receive();
    bool available();
    int parsePacket();
    byte read();
    void send();
};