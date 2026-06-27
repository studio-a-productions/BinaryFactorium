#include <Arduino.h>
#include <BinF/Engine.hpp>


void setup() {
  Serial.begin(115200);
  BinF::Engine::Init(NULL);
}

void loop() {
  
}