/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include "common.hpp"
#include "Colour.hpp"
namespace BinF::Engine {
    using screen_pos = u16;
    constexpr screen_pos screen_x = 240U;
    constexpr screen_pos screen_y = 296U;
    constexpr screen_pos max_screen_pos = (screen_x < screen_y ? screen_y : screen_x) - 1U;
}