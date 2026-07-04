/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once
#include "World.hpp"

namespace BinF::Game {
    class CameraClass {
    public:
        CameraClass() = default;
        CameraClass(WorldAxis x, WorldAxis y);
        void Move(WorldAxis newx, WorldAxis newy);
        WorldAxis GetX();
        WorldAxis GetY();
    private:
        WorldAxis x, y;
    };

    // handled by Game Resources
    // Camera's position is pixel-based, and the camera's position shall be used as the screen's center.
    extern CameraClass Camera;
}