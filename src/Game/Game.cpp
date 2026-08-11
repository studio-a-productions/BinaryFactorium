/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <BinF/Game.hpp>
#include <BinF/Game/World.hpp>
#include <BinF/Game/Assets.hpp>
namespace BinF::Game {
    constexpr char SettingsFileName[] = "binf/settings.bin";
    Engine::screen_pos x;
    Engine::screen_pos y;
    void Start() {
        Engine::Init();
        if (Engine::FileSystem.State() != Engine::FSState::Bad)
        if (Engine::FileSystem.FileExists(SettingsFileName)) {
            auto& settingsfile = Engine::FileSystem.GetFile(SettingsFileName);
            if (!settingsfile.IsValid()) return;
            settingsfile.~FileHandle();
        }

        Logger.Info("I init!!!");
    }

    void Update() {
        Engine::Update();
        Engine::DrawSprite(0, 0, TitleScreen, Engine::screen_x, Engine::screen_y);
        
        Engine::PushFrame();
        Logger.Info("Frametime: %u", Engine::DeltaTime());
        Logger.Info("X (%hd), Y (%hd)", x, y);

    }

    void End() {
        Engine::Exit();
    }

}
