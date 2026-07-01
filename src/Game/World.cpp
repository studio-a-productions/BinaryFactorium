#include <BinF/Game/World.hpp>
#include <BinF/Game/Chunk.hpp>

#include <BinF/Engine/Palette.hpp>


namespace BinF::Game {
    // screen buffer
    // contains the pixel data before it will be pushed to the frame buffer
    Engine::colourID ScreenBuffer[20*22*TileSize*TileSize];

    WorldAxis CamX = 0;
    WorldAxis CamY = 0;

}