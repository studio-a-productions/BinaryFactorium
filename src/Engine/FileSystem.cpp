/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine/FileSystem.hpp>
#include <Fri3dBadge_pins.h>
#include <SPI.h>
#include <SD.h>

namespace BinF::Engine {
    FileSystemClass::FileSystemClass() { }
    FileSystemClass::~FileSystemClass() { }

    FSState FileSystemClass::Begin() {
        if (!SD.begin(PIN_SDCARD_CS, SPI, 40000000)) {
            Serial.println("[BinF::Engine] (FAILURE) SD Card init fail");
            return FSState::Bad;
        } else return FSState::Good;
    }


}