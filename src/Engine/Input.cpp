#include <BinF/Engine.hpp>
#include <Fri3dBadge_pins.h> /* Owned by Fri3d Camp */

// internal comm between cpp
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {

    // KEY DATA ------------------------------------------------------------------------
    const u8 KEY_PINS[KEY_COUNT] = { PIN_A, PIN_B, PIN_X, PIN_Y, PIN_MENU, PIN_START };
    const u8 KEY_MODES[KEY_COUNT] = { PULLUP, PULLUP, PULLUP, PULLUP, PULLUP, INPUT };
    constexpr u32 BOUNCE_DELAY = 25; // 25 ms
    
    bool keyStates[KEY_COUNT] = {false};
    u32 lastBounceTime[KEY_COUNT] = {0};

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
        
    }

    // noice!
    bool ButtonPressed(keycode keyc) {
        return keyStates[keyc];
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

    void UpdateInput() {
        bool keyRead = false;
        
        u16 rawJX;
        u16 rawJY;

        Time curTim = millis();
        // key updates
        for (u8 i = 0; i < KEY_COUNT; i++) {
            // register "pressed"?
            if (KEY_MODES[i] == PULLUP)
                keyRead = (digitalRead(KEY_PINS[i]) == LOW);
            else 
                keyRead = (digitalRead(KEY_PINS[i]) == HIGH);

            /*
                Simple debounce: if different from state, check if debounce time passed. If same, just reset debounce timer.
            */

            if (keyRead != keyStates[i]) {
                if (curTim - lastBounceTime[i] > BOUNCE_DELAY) {
                    keyStates[i] = keyRead;
                    lastBounceTime[i] = curTim;
                }
            } else {
                // idk man, told ya its simple
                lastBounceTime[i] = curTim;
            }
        }


        // joystick updates
        rawJX = analogRead(PIN_JOY_X);
        rawJY = analogRead(PIN_JOY_Y);

        joystickX = JoystickDigital(rawJX);
        joystickY = JoystickDigital(rawJY);
    }
}