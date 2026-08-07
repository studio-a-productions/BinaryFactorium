/*
  Copyright Free, not included in LICENCE

  Yes! Be happy!
*/

#include <Arduino.h>
#include <BinF/Game.hpp>


void setup() {
  BinF::Game::Start();
}

void loop() {
  BinF::Game::Update();
}