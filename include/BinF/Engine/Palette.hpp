/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include "Colour.hpp"

namespace BinF::Engine {
    using colourID = u8;
    constexpr colourID NaC = 0;
    constexpr u8 PaletteSize = u8m;

    // user MUST change their palette here (for now)
    // later the engine may add additional palette support (runtime loaded) it'll though slower
    const colour ColourPalette[] = { 
        0U,     // indx 0 reserved in engine
        0x0000,
        0x0173,
        0x04C1,
        0x19A9,
        0x1A35,
        0x1AE2,
        0x1C92,
        0x2149,
        0x23AD,
        0x3184,
        0x31A6,
        0x3205,
        0x3504,
        0x39C6,
        0x43CE,
        0x4925,
        0x4A48,
        0x4B30,
        0x4B84,
        0x51C9,
        0x5289,
        0x528A,
        0x55A9,
        0x5B94,
        0x5BCF,
        0x5D55,
        0x6904,
        0x6B0A,
        0x732E,
        0x73AD,
        0x740B,
        0x7AE7,
        0x83CC,
        0x8410,
        0x8430,
        0x8503,
        0x85E6,
        0x8B8B,
        0x8C05,
        0x948B,
        0x95F9,
        0x9CAB,
        0x9CF3,
        0xA48C,
        0xA553,
        0xAD50,
        0xC62C,
        0xC945,
        0xCCCD,
        0xD5D6

    };
}