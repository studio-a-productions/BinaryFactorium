/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {
    void Init() {
        InitInput();
        InitRenderer();
    }

    void Update() {
        UpdateTime();
        UpdateInput();
    }

    void Exit() {
        ExitRenderer();
    }
}