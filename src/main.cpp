/*
  Copyright Free, not included in LICENCE

  Yes! Be happy!
*/



#include <BinF/Game.hpp>
#include <BinF/Engine.hpp>

#if BINF_PLATFORM == DESKTOP_SDL
int main(int, char**) {
    BinF::Game::Start();
    while (!BinF::Engine::ShouldExit()) BinF::Game::Update();
    BinF::Game::End();
    return 0;
}
#else
void setup() { BinF::Game::Start(); }
void loop()  { BinF::Game::Update(); }
#endif