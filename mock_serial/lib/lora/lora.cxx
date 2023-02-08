#include "lora.hpp"

void Lora::begin(long seed){
    randomSeed(seed);
}

void Lora::onReceive(void(*callback)(Lora*)){
    _onReceive = callback;
}

void Lora::receive(){
    _buffer += "a";
    _buffer += random(1,5);
    _buffer += "b";
    _buffer += random(100)*0.5f;
    _buffer += "c";
    _buffer += random(100)*0.5f;
    _buffer += "d";
    _buffer += random(100)*0.5f;
    _buffer += "\r\n";
    bufferCompleted = true;
}

int Lora::parsePacket(){
    return _buffer.length();
}

bool Lora::available(){
    return _buffer.length() > 0;
}

byte Lora::read(){
    if(_buffer.length() == 0)
        return -1;
    char chr = _buffer.begin()[0];
    _buffer.remove(0,1);
    return chr;
}

void Lora::send(){
    if(bufferCompleted){
        _onReceive(this);
        bufferCompleted = false;
    }
}