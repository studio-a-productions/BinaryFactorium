/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <BinF/Game.hpp>
#include <BinF/Game/World.hpp>
#include <BinF/Game/WorldGen.hpp>
#include <BinF/Game/Camera.hpp>
#include <BinF/Game/Assets.hpp>
namespace BinF::Game {
    constexpr char SettingsFileName[]           = "binf/settings.bin";
    constexpr Engine::screen_pos StartButtonX   = 10;
    constexpr Engine::screen_pos StartButtonY   = 10;
    constexpr Engine::screen_pos ConfButtonX    = 10;
    constexpr Engine::screen_pos ConfButtonY    = 80;

    constexpr u32 JoystickSense = 000;

    enum class GameMode {
        Start,
        Pause,
        Play
    };

    GameMode currentMode = GameMode::Play;

    void Start() {
        Engine::Init();
        if (Engine::FileSystem.State() != Engine::FSState::Bad)
        if (Engine::FileSystem.FileExists(SettingsFileName)) {
            auto& settingsfile = Engine::FileSystem.GetFile(SettingsFileName);
            if (!settingsfile.IsValid()) return;
            settingsfile.~FileHandle();
        }
        SetSeed(esp_random());
        Logger.Info("I init!!!");
    }
    static inline void DisplayMain() {
        static u8 buttonselect = 0;
        

    }

    static inline void DisplayGame() {
        if (Engine::ButtonPressed(Engine::KEY_B)) {
            Camera.SetPosition(0, 0, 0, 0);
            SetSeed(esp_random());
            RegenWorld();
        }
        s32 multipli = Engine::ButtonDown(Engine::KEY_A) ? 4 : 1;
        Camera.Move(Engine::JoystickX()/8190*multipli, -(Engine::JoystickY()/8190)*multipli);
        UpdateWorld();
        RenderWorld();

        Engine::DrawSprite(Engine::screen_x/2-Engine::JoystickX()/7900, Engine::screen_y/2+Engine::JoystickY()/7900, Player, 16);

        Logger.Info("X %d.%d Y %d.%d",Camera.x, Camera.cx, Camera.y, Camera.cy);
    }
    void Update() {
        Engine::Update();

        switch (currentMode) {
            case GameMode::Start:
                DisplayMain();
                break;
            case GameMode::Play:
                DisplayGame();
                break;
            default: break;
        }

        Engine::PushFrame();
        Logger.Info("Frametime: %u", Engine::DeltaTime());

    }

    void End() {
        Engine::Exit();
    }

}
