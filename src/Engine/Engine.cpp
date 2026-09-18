/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#if BINF_PLATFORM == FRI3D2024 || BINF_PLATFORM == FRI3D2026
#include <SPI.h>
#elif BINF_PLATFORM == DESKTOP_SDL
#include <SDL3/SDL.h>
#endif
#include <BinF/Engine/Internal.hpp>

namespace BinF {
    Engine::LoggerClass Logger = Engine::LogLevel::Info;

}

namespace BinF::Engine {
    FileSystemClass& FileSystem = *New<FileSystemClass>();
    #if BINF_PLATFORM == FRI3D2026
    Fri3dXP expander = Fri3dXP();
    #endif
    void Init() {
        #if BINF_PLATFORM == FRI3D2024 || BINF_PLATFORM == FRI3D2026
        Serial.begin(115200);
        Wait(2000);
        #endif

        //Logger.Info("(Engine) Init SPI");
        //SPI.begin(7, 8, 6);
        if (!&FileSystem) {
            Logger.Warn("(Engine) No File System! Creating new FileSys");
            FileSystem = *New<FileSystemClass>();
            if (!&FileSystem) Logger.Crit("(Engine) Couldn't create File System!");
        }

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
        ExitInput();
        Delete(&FileSystem);

        #if BINF_PLATFORM == DESKTOP_SDL
        SDL_Quit();
        #endif
    }
}
