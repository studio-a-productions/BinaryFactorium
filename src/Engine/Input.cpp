#include <BinF/Engine.hpp>
#include <Fri3dBadge_pins.h> /* Owned by Fri3d Camp */

// internal comm between cpp
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {
    const u8 KEY_PINS[KEY_COUNT] = { PIN_A, PIN_B, PIN_X, PIN_Y, PIN_MENU, PIN_START };

    bool keyStates[KEY_COUNT] = {false};
    u32 lastBounceTime[KEY_COUNT] = {0};
    constexpr u32 BOUNCE_DELAY = 25; // 25 ms

    void InitInput() {
        for (u8 i = 0; i < KEY_COUNT; i++)
            pinMode(KEY_PINS[i], PULLUP);
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