/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <SPI.h>
#include <BinF/Engine/Internal.hpp>

namespace BinF {
    Engine::LoggerClass Logger = Engine::LogLevel::Info;

}

namespace BinF::Engine {
    FileSystemClass FileSystem = FileSystemClass();
    void Init() {
        Logger.Info("(Engine) Init SPI");
        SPI.begin(7, 8, 6);
        Logger.Info("(Engine) Init Input");
        InitInput();
        Logger.Info("(Engine) Init Renderer");
        InitRenderer();
        Logger.Info("(Engine) Init FS");
        FileSystem.Begin();
    }

    void Update() {
        UpdateTime();
        UpdateInput();
    }

    void Exit() {
        // while (true) scream("coffee!")
        ExitRenderer();
    }
}
