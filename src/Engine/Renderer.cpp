#include <BinF/Engine.hpp>
#include <BinF/Engine/Renderer.hpp>
#include <BinF/Engine/Palette.hpp>
#include <BinF/Engine/Internal.hpp>
#include <TFT_eSPI.h>

namespace BinF::Engine {
    TFT_eSPI tft = TFT_eSPI();

    // frame buffer memory
    colour framebuffer[screen_y][screen_x];
    void InitRenderer() {
        tft.init();
        tft.initDMA();
    }

    // probably should have a task do this later, though that makes things complicated
    inline void DrawData(const screen_pos x, const screen_pos y, const colourID* data, const screen_pos sx, const screen_pos sy) {
        colourID stencil = 0U;
        for (screen_pos i = y; i < sy; i++)
            for (screen_pos j = x; j < sx; j++) {
                stencil = *(data++);
                if (stencil) 
                    framebuffer[i][j] = ColourPalette[stencil];
            }
    }

    void ExitRenderer() {
        tft.deInitDMA();
    }
    
}