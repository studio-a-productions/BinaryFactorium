/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/


#include <BinF/Engine.hpp>
#include <Fri3d.h>
#include <atomic>

// internal comm between cpp-impl and resource manager: engine.cpp (eg InitInput, UpdateInput)
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {

    #if BINF_PLATFORM == FRI3D2024
    // KEY DATA ------------------------------------------------------------------------
    const u8 KEY_PINS[KEY_COUNT] = { PIN_A, PIN_B, PIN_X, PIN_Y, PIN_MENU, PIN_START };
    const u8 KEY_MODES[KEY_COUNT] = { INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP, INPUT };
    constexpr u8 BOUNCE_DELAY = 10; // ms
    constexpr u8 BOUNCE_TIMEOUT = 5;
    
    std::atomic<bool> taskKeyStates[KEY_COUNT] = { }; // task
    Time lastBounceTime[KEY_COUNT] = {0}; // task
    #elif BINF_PLATFORM == DESKTOP_SDL
    SDL_Gamepad* Gamepad = nullptr;
    constexpr SDL_Scancode KEY_SCANCODES[KEY_COUNT] = {
        SDL_SCANCODE_Q, SDL_SCANCODE_E, SDL_SCANCODE_Z,
        SDL_SCANCODE_X, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_RETURN
    };
    constexpr SDL_GamepadButton KEY_PADBUTTONS[KEY_COUNT] = {
        SDL_GAMEPAD_BUTTON_SOUTH, SDL_GAMEPAD_BUTTON_EAST, SDL_GAMEPAD_BUTTON_WEST,
        SDL_GAMEPAD_BUTTON_NORTH, SDL_GAMEPAD_BUTTON_BACK, SDL_GAMEPAD_BUTTON_START
    };

    static inline void RefreshGamepad() {
        if (Gamepad && !SDL_GamepadConnected(Gamepad)) { SDL_CloseGamepad(Gamepad); Gamepad = nullptr; }
        if (!Gamepad) {
            int count = 0;
            SDL_JoystickID* ids = SDL_GetGamepads(&count);
            if (ids && count > 0) Gamepad = SDL_OpenGamepad(ids[0]);
            if (ids) SDL_free(ids);
        }
    }

    static inline s16 ApplyDeadzone(s16 raw) {
        if (abs(raw) <= joystickDeadzone) return 0;
        return (raw > 0)
            ? static_cast<s16>((static_cast<s32>(raw - joystickDeadzone) * joystickDigitalH) / (joystickDigitalH - joystickDeadzone))
            : static_cast<s16>((static_cast<s32>(raw + joystickDeadzone) * joystickDigitalL) / (joystickDigitalL + joystickDeadzone));
    }

    #endif
    bool keyPrevStates[KEY_COUNT] = { false };
    bool keyStates[KEY_COUNT] = { false };


    // JOYSTICK DATA ------------------------------------------------------------------
    constexpr u16 joystickLow   = 0;
    constexpr u16 joystickMid   = 2048;
    constexpr u16 joystickHigh  = 4095;

    constexpr s16 joystickDigitalL  = -32768;
    constexpr s16 joystickDigitalH  = 32767;
    constexpr s16 joystickDeadzone  = joystickDigitalH/2;


    s16 joystickX = 0;
    s16 joystickY = 0;

    void InputTask(void* param);

    void InitInput() {
        #if BINF_PLATFORM == FRI3D2024
        // init keys
        for (u8 i = 0; i < KEY_COUNT; i++)
            pinMode(KEY_PINS[i], KEY_MODES[i]);
        // init joystick
        pinMode(PIN_JOY_X, INPUT);
        pinMode(PIN_JOY_Y, INPUT);
        
        // init task
        xTaskCreatePinnedToCore(
            InputTask, "InputTask",
            2048,
            NULL, /* not needed */
            2, /* 1 is loop() priotity */
            NULL,
            0 /* core 0 should probably be free, right? */
        );
        #elif BINF_PLATFORM == FRI3D2026
        expander.begin();

        #elif BINF_PLATFORM == DESKTOP_SDL
        SDL_InitSubSystem(SDL_INIT_GAMEPAD);
        RefreshGamepad();
        #endif
    }

    // noice!
    bool ButtonDown(const keycode keyc) {
        return keyStates[keyc];
    }

    bool ButtonPressed(const keycode keyc) {
        return keyStates[keyc] && !keyPrevStates[keyc];
    }
    bool ButtonReleased(const keycode keyc) {
        return !keyStates[keyc] && keyPrevStates[keyc];
    }
    
    s16 JoystickX() {
        return joystickX;
    }
    s16 JoystickY() {
        return joystickY;
    }

    #if BINF_PLATFORM != DESKTOP_SDL
    inline s16 JoystickDigital(u16 raw) {
        s16 digiDat = map(raw, joystickLow, joystickHigh, joystickDigitalL, joystickDigitalH);

        if (abs(digiDat) <= joystickDeadzone)
            return 0;
        
        if (digiDat > joystickDeadzone)
            return static_cast<s16>(
                (static_cast<s32>(digiDat - joystickDeadzone)*joystickDigitalH) / (joystickDigitalH - joystickDeadzone)
            );
        else 
            return static_cast<s16>(
                (static_cast<s32>(digiDat + joystickDeadzone)*joystickDigitalL) / (joystickDigitalL + joystickDeadzone)
            );
    }
    #endif

    // because we're not using WiFi or BT for now, we can use tasks without too many wories
    // hopefully we don't get into conflict :)
    #if BINF_PLATFORM == FRI3D2024
    void InputTask(void* param) {
        bool keyRead = false;
        for (;;) {
            Time curTim = millis();
            for (u8 i = 0; i < KEY_COUNT; i++) {
                keyRead = (KEY_MODES[i] == INPUT_PULLUP)
                    ? (digitalRead(KEY_PINS[i]) == LOW)
                    : (digitalRead(KEY_PINS[i]) == HIGH);
            
                //Logger.Info("Keystate (%hhu): %d", i, keyRead );
                if (keyRead != taskKeyStates[i].load(std::memory_order_relaxed)) {
                    if (curTim - lastBounceTime[i] > BOUNCE_DELAY) {
                        taskKeyStates[i].store(keyRead, std::memory_order_relaxed);
                        lastBounceTime[i] = curTim;
                    }
                } else {
                    lastBounceTime[i] = curTim;
                }
            }
        
            vTaskDelay(pdMS_TO_TICKS(BOUNCE_TIMEOUT));
        }
    }
    

    inline u16 ReadJoystickAveraged(u8 pin) {
        constexpr u8 samples = 8;
        u32 sum = 0;
        for (u8 i = 0; i < samples; i++) sum += analogRead(pin);
        return sum / samples;
    }
    #endif

    void UpdateInput() {
        #if BINF_PLATFORM != DESKTOP_SDL
        u16 rawJX;
        u16 rawJY;
        #endif  
        // key transport (so we don't pay the price of atomic)
        for (u8 i = 0; i < KEY_COUNT; i++) {
            keyPrevStates[i] = keyStates[i];
            #if BINF_PLATFORM == FRI3D2024
            keyStates[i] = taskKeyStates[i].load(std::memory_order_relaxed);
            #elif BINF_PLATFORM == DESKTOP_SDL
            const bool* keyboard = SDL_GetKeyboardState(nullptr);
            bool down = keyboard[KEY_SCANCODES[i]];
            if (Gamepad) down = (down || SDL_GetGamepadButton(Gamepad, KEY_PADBUTTONS[i]));

            keyStates[i] = down;
            #endif
        }
        #if BINF_PLATFORM == FRI3D2026
        expander.loop();
        keyStates[KEY_A] = expander.getButtonA();
        keyStates[KEY_B] = expander.getButtonB();
        keyStates[KEY_X] = expander.getButtonX();
        keyStates[KEY_Y] = expander.getButtonY();
        keyStates[KEY_MENU]  = expander.getButtonMenu();
        keyStates[KEY_START] = expander.getButtonStart();
        #elif BINF_PLATFORM == DESKTOP_SDL
        SDL_PumpEvents();
        RefreshGamepad();
        #endif
        
        // joystick updates
        // these cannot be moved into the task, as the cost of analogRead is to big to pay for every BOUNCE_TIMEOUT
        #if BINF_PLATFORM == DESKTOP_SDL
        joystickX = 0;
        joystickY = 0;
        const bool* keyboard = SDL_GetKeyboardState(nullptr);
        if (Gamepad) {
           /* poll first this */
            
            joystickX = ApplyDeadzone(SDL_GetGamepadAxis(Gamepad, SDL_GAMEPAD_AXIS_LEFTX));
            joystickY = ApplyDeadzone(SDL_GetGamepadAxis(Gamepad, SDL_GAMEPAD_AXIS_LEFTY));
        }
        joystickX = (keyboard[SDL_SCANCODE_D] || keyboard[SDL_SCANCODE_RIGHT]) ? joystickDigitalH
            : (keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_LEFT]) ? joystickDigitalL : joystickX;
        joystickY = (keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_DOWN]) ? joystickDigitalH
            : (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_UP]) ? joystickDigitalL : joystickY;
        
        #else
        rawJX = 
            #if BINF_PLATFORM == FRI3D2024
            ReadJoystickAveraged(PIN_JOY_X);
            #elif BINF_PLATFORM == FRI3D2026
            expander.getJoystickX();
            #else
            joystickMid;
        #endif
        rawJY = 
            #if BINF_PLATFORM == FRI3D2024
            ReadJoystickAveraged(PIN_JOY_Y);
            #elif BINF_PLATFORM == FRI3D2026
            expander.getJoystickY();
            #else
            joystickMid;
        #endif

        joystickX = JoystickDigital(rawJX);
        joystickY = JoystickDigital(rawJY);

        #endif
    }
}