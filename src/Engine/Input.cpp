/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/


#include <BinF/Engine.hpp>
#include <Fri3dBadge_pins.h> /* Owned by Fri3d Camp */
#include <atomic>

// internal comm between cpp-impl and resource manager: engine.cpp (eg InitInput, UpdateInput)
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {

    // KEY DATA ------------------------------------------------------------------------
    const u8 KEY_PINS[KEY_COUNT] = { PIN_A, PIN_B, PIN_X, PIN_Y, PIN_MENU, PIN_START };
    const u8 KEY_MODES[KEY_COUNT] = { PULLUP, PULLUP, PULLUP, PULLUP, PULLUP, INPUT };
    constexpr u8 BOUNCE_DELAY = 10; // ms
    constexpr u8 BOUNCE_TIMEOUT = 5;
    
    bool keyStates[KEY_COUNT] = { false };
    bool keyPrevStates[KEY_COUNT] = { false };
    std::atomic<bool> taskKeyStates[KEY_COUNT] = { }; // task
    Time lastBounceTime[KEY_COUNT] = {0}; // task

    // JOYSTICK DATA ------------------------------------------------------------------
    constexpr u16 joystickLow   = 0;
    constexpr u16 joystickMid   = 2048;
    constexpr u16 joystickHigh  = 4095;

    constexpr s16 joystickDigitalL  = -32768;
    constexpr s16 joystickDigitalH  = 32767;
    constexpr s16 joystickDeadzone  = 1500;


    s16 joystickX = 0;
    s16 joystickY = 0;

    void InitInput() {
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
    void InputTask(void* param) {
        bool keyRead = false;
        for (;;) {
            Time curTim = millis();
            for (u8 i = 0; i < KEY_COUNT; i++) {
                keyRead = (KEY_MODES[i] == PULLUP)
                    ? (digitalRead(KEY_PINS[i]) == LOW)
                    : (digitalRead(KEY_PINS[i]) == HIGH);
            
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

    void UpdateInput() {
        u16 rawJX;
        u16 rawJY;

        // key transport (so we don't pay the price of atomic)
        for (u8 i = 0; i < KEY_COUNT; i++) {
            keyPrevStates[i] = keyStates[i];
            keyStates[i] = taskKeyStates[i].load(std::memory_order_relaxed);
        }
            

        // joystick updates
        // these cannot be moved into the task, as the cost of analogRead is to big to pay for every BOUNCE_TIMEOUT
        rawJX = analogRead(PIN_JOY_X);
        rawJY = analogRead(PIN_JOY_Y);

        joystickX = JoystickDigital(rawJX);
        joystickY = JoystickDigital(rawJY);
    }
}