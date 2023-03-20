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
    // Montrela Location: 45.49383925519548,-73.57699117825592
    _buffer += "x";
    _buffer += random(6960, 6970)/10000.0 + 7357.0f;
    _buffer += "W";
    _buffer += "y";
    _buffer += random(3830, 3840)/10000.0 + 4549.0f;
    _buffer += "N";
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