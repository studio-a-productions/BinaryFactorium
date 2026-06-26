#include <BinF/Engine.hpp>
#include <BinF/Engine/Internal.hpp>
#include <TFT_eSPI.h>



namespace BinF::Engine {
    TFT_eSPI tft = TFT_eSPI();
    void Init() {
        tft.init();
        tft.initDMA();

        InitInput();
    }

    void Update() {
        UpdateTime();
        UpdateInput();
    }

    void Exit() {
        tft.deInitDMA();
    }
}