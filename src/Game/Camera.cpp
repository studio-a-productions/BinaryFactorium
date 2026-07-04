/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Game/Camera.hpp>

namespace BinF::Game {
    CameraClass::CameraClass(const WorldAxis x, const WorldAxis y) : x{x}, y{y} {
        // do nothing
    }

    WorldAxis CameraClass::GetX() { return x; }
    WorldAxis CameraClass::GetY() { return y; }

    void CameraClass::Move(const WorldAxis newx, const WorldAxis newy) {
        x=newx;
        y=newy;
    }
}