/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Game/World.hpp>
#include <BinF/Game/Camera.hpp>
#include <BinF/Game/Assets.hpp>
#include <BinF/Game/WorldGen.hpp>

#include <BinF/Engine.hpp>


namespace BinF::Game {
    // CHUNKY DATA ---------
    constexpr u8 LocalChunkAmount = 9; /* 3x3, REMEMBER: update RenderWorld when this changes */
    static bool WorldViewInitialized = false;
    static WorldAxis LastCameraX = 0;
    static WorldAxis LastCameraY = 0;

    ChunkData WorldView[LocalChunkAmount] = { };

    void RegenWorld() {
        WorldViewInitialized = false;
    }
    
    // HELPERS -------------
    inline void RenderChunk(const u8 chunk, const PixelOffset start_cx, PixelOffset cy) {
        if ((start_cx + ChunkPixels < 0)  || (cy + ChunkPixels < 0))  return;
        if ((start_cx > Engine::screen_x) || (cy > Engine::screen_y)) return;
        
        u16 indx = 0;
        for (u8 row = 0; row < ChunkSize; row++) {
            PixelOffset cx = start_cx;
            for (u8 col = 0; col < ChunkSize; col++) {
                Engine::DrawSprite(cx, cy, Tiles[TileSpriteIndx(WorldView[chunk].Tiles[indx])], TileSize);
                indx++;
                cx += TileSize;
            }
            cy += TileSize;
        }
    }

    // yapping removed :)
    void UpdateWorld() {  
        if (!WorldViewInitialized) {
            u8 indx = 0;
            for (WorldAxis dy = -1; dy <= 1; dy++) {
                for (WorldAxis dx = -1; dx <= 1; dx++) {
                    ChunkData* Generated = GenerateChunk(Camera.x + dx, Camera.y + dy);
                    WorldView[indx] = *Generated;
                    Engine::Free<ChunkData>(Generated);
                    indx++;
                }
            }

            LastCameraX = Camera.x;
            LastCameraY = Camera.y;
            WorldViewInitialized = true;
            return;
        }

        // get out as soon as possible
        if (Camera.x == LastCameraX && Camera.y == LastCameraY) return;

        ChunkData NewView[LocalChunkAmount];
        bool Reused[LocalChunkAmount] = {};

        u8 indx = 0;
        for (WorldAxis dy = -1; dy <= 1; dy++) {
            for (WorldAxis dx = -1; dx <= 1; dx++) {
                const WorldAxis TargetX = Camera.x + dx;
                const WorldAxis TargetY = Camera.y + dy;

                // Is it here already?
                bool Found = false;
                for (u8 old = 0; old < LocalChunkAmount; old++) {
                    if (!Reused[old] && WorldView[old].x == TargetX && WorldView[old].y == TargetY) {
                        NewView[indx] = WorldView[old];
                        Reused[old] = true;
                        Found = true;
                        break;
                    }
                }

                if (!Found) {
                    ChunkData* Generated = GenerateChunk(TargetX, TargetY);
                    NewView[indx] = *Generated;
                    Engine::Free<ChunkData>(Generated);
                }

                indx++;
            }
        }

        for (u8 i = 0; i < LocalChunkAmount; i++) {
            WorldView[i] = NewView[i];
        }

        LastCameraX = Camera.x;
        LastCameraY = Camera.y;
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