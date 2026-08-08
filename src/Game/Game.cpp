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
        Engine::DrawSprite(Engine::screen_x/2, Engine::screen_y/2, Miner, 32, 74);
        if (Engine::ButtonDown(Engine::KEY_A)) {
            Logger.Info("A Pressed!!!");
            Engine::Wait(100);
            //Engine::ClearFrame();
        }
        Engine::PushFrame();
        Logger.Info("Frametime: %u", Engine::DeltaTime());

        Engine::Wait(100);
    }

    void End() {
        Engine::Exit();
    }

}
