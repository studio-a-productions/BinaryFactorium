/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Game/World.hpp>
#include <BinF/Game/Camera.hpp>

#include <BinF/Engine.hpp>


namespace BinF::Game {
    // CHUNKY DATA ---------
    constexpr u8 LocalChunkAmount = 9; /* 3x3, REMEMBER: update RenderWorld when this changes */


    ChunkData WorldView[LocalChunkAmount] = {  };
    
    // HELPERS -------------
    inline void RenderChunk(const u8 chunk, const PixelOffset start_cx, PixelOffset cy) {
        if ((start_cx + ChunkPixels < 0)  || (cy + ChunkPixels < 0))  return;
        if ((start_cx > Engine::screen_x) || (cy > Engine::screen_y)) return;
        
        u16 indx = 0;
        for (u8 row = 0; row < ChunkSize; row++) {
            PixelOffset cx = start_cx;
            for (u8 col = 0; col < ChunkSize; col++) {
                Engine::DrawSprite(cx, cy, TileSprites[TileSpriteIndx(WorldView[chunk].tiles[indx])], TileSize);
                indx++;
                cx += TileSize;
            }
            cy += TileSize;
        }
    }

    // yapping removed :)
    void UpdateWorld() {
        // NOT PRIORITY
        // IGNORE FUNCTION FOR NOW
    }

    void RenderWorld() {
        const PixelOffset cx = Engine::centre_x - Camera.cx - ChunkPixels;
        const PixelOffset cy = Engine::centre_y - Camera.cy - ChunkPixels;

        PixelOffset tx = cx;
        PixelOffset ty = cy;

        u8 chunk = 0;
        for (u8 chunk_y = 0; chunk_y < 3; chunk_y++) {
            tx = cx;
            for (u8 chunk_x = 0; chunk_x < 3; chunk_x++) {
                RenderChunk(chunk, tx, ty);
                tx += ChunkPixels;
                chunk++;
            }
            ty += ChunkPixels;
        }

    }
}