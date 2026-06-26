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
    constexpr u16 joystickLow  = 0;
    constexpr u16 joystickHigh = 0;

    void InitInput() {
        for (u8 i = 0; i < KEY_COUNT; i++)
            pinMode(KEY_PINS[i], KEY_MODES[i]);
    }

    // noice!
    bool ButtonPressed(keycode keyc) {
        return keyStates[keyc];
    }

    void UpdateInput() {
        static bool keyRead = false;

        for (u8 i = 0; i < KEY_COUNT; i++) {
            // register "pressed"?
            keyRead = (digitalRead(KEY_PINS[i]) == LOW);

            /*
                Simple debounce: if different from state, check if debounce time passed. If same, just reset debounce timer.
            */

            if (keyRead != keyStates[i]) {
                if (millis() - lastBounceTime[i] > BOUNCE_DELAY) {
                    keyStates[i] = keyRead;
                    lastBounceTime[i] = millis();
                }
            } else {
                lastBounceTime[i] = millis();
            }
        }
    }



}