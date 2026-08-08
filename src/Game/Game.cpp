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
            x = Engine::screen_x/2;
            y = Engine::screen_y/2;
        }

        Logger.Info("I init!!!");
    }

    void Update() {
        Engine::Update();
        Engine::ClearFrame();
        x += Engine::JoystickX();
        y += Engine::JoystickY();
        Engine::DrawSprite(x, y, Miner, 32, 74);
        
        Engine::PushFrame();
        Logger.Info("Frametime: %u", Engine::DeltaTime());
        Logger.Info("X (%hd), Y (%hd)", x, y);

        Engine::Wait(200);
    }

    void End() {
        Engine::Exit();
    }

}
