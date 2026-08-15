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
    #endif

    inline u16 ReadJoystickAveraged(u8 pin) {
        constexpr u8 samples = 8;
        u32 sum = 0;
        for (u8 i = 0; i < samples; i++) sum += analogRead(pin);
        return sum / samples;
    }

    void UpdateInput() {
        u16 rawJX;
        u16 rawJY;
        // key transport (so we don't pay the price of atomic)
        for (u8 i = 0; i < KEY_COUNT; i++) {
            keyPrevStates[i] = keyStates[i];
            #if BINF_PLATFORM == FRI3D2024
            keyStates[i] = taskKeyStates[i].load(std::memory_order_relaxed);
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
        #endif
        
        // joystick updates
        // these cannot be moved into the task, as the cost of analogRead is to big to pay for every BOUNCE_TIMEOUT
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
    }
}