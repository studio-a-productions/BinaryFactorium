#include <BinF/Engine.hpp>

#include <TFT_eSPI.h>
namespace BinF::Engine {
    TFT_eSPI tft = TFT_eSPI();
    void Init() {
        tft.init();
        tft.initDMA();
    }

    void Exit() {
        tft.deInitDMA();
    }
}