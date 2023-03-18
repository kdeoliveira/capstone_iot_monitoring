#include "lora.hpp"

void Lora::begin(long seed){
    randomSeed(seed);
}

void Lora::onReceive(void(*callback)(Lora*)){
    _onReceive = callback;
}

void Lora::receive(){
    _buffer.remove(0,_buffer.length());
    
    _buffer += "a";
    _buffer += random(1,5);
    _buffer += "b";
    _buffer += random(100)*0.5f;
    _buffer += "c";
    _buffer += random(100)*0.5f;
    _buffer += "d";
    _buffer += random(100)*0.5f;
    _buffer += "y";
    _buffer += random(4930, 4950)/10000.0 + 45.0001f;
    _buffer += "N";
    _buffer += "x";
    _buffer += random(5760, 5790)/10000.0 + 73.0001f;
    _buffer += "W";
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
    char chr = _buffer.charAt(0);
    _buffer.remove(0,1);
    return chr;
}

void Lora::send(){
    if(bufferCompleted){
        _onReceive(this);
        bufferCompleted = false;
    }
}