/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Game/Camera.hpp>

namespace BinF::Game {
    CameraClass::CameraClass() : x{0}, y{0}, cx{ChunkSize*TileSize/2}, cy{ChunkSize*TileSize/2} {
        // init at origin
    }

    CameraClass::CameraClass(const WorldAxis x, const WorldAxis y, const PixelOffset cx, const PixelOffset cy) 
    : x{x}, y{y}, cx{cx}, cy{cy} {

    }

    void CameraClass::SetPosition(const WorldAxis nx, const WorldAxis ny, const PixelOffset ncx, const PixelOffset ncy) {
        x = nx;
        y = ny;
        cx= ncx;
        cy= ncy;
    }

    constexpr s32 ChunkSizePixels = ChunkSize * TileSize;
    void CameraClass::Move(const PixelOffset nx, const PixelOffset ny) {
        cx += nx;
        cy += ny;

        const WorldAxis ChunkMoveX = (cx >= 0) ? (cx / ChunkSizePixels) : ((cx - ChunkSizePixels + 1) / ChunkSizePixels);
        const WorldAxis ChunkMoveY = (cy >= 0) ? (cy / ChunkSizePixels) : ((cy - ChunkSizePixels + 1) / ChunkSizePixels);

        x += ChunkMoveX;
        y += ChunkMoveY;

        cx = ((cx % ChunkSizePixels) + ChunkSizePixels) % ChunkSizePixels;
        cy = ((cy % ChunkSizePixels) + ChunkSizePixels) % ChunkSizePixels;
    }

    CameraClass Camera = CameraClass();
}