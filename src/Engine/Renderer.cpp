/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <BinF/Engine/Renderer.hpp>
#include <BinF/Engine/Palette.hpp>
#include <BinF/Engine/Internal.hpp>
#include <TFT_eSPI.h>

namespace BinF::Engine {
    TFT_eSPI tft = TFT_eSPI();

    using ScreenRow = colour[screen_x];

    // frame buffer memory
    ScreenRow* framebuffer = nullptr;
    ScreenRow* renderbuffer = nullptr;

    // RESOURCE MANAGEMENT -----------
    void InitRenderer() {
        framebuffer = (ScreenRow*)heap_caps_malloc(screen_y * screen_x * sizeof(colour), MALLOC_CAP_DMA);
        renderbuffer = (ScreenRow*)heap_caps_malloc(screen_y * screen_x * sizeof(colour), MALLOC_CAP_DMA);
        
        tft.init();
        tft.initDMA();
    }
    void ExitRenderer() {
        if (tft.dmaBusy()) tft.dmaWait();

        tft.deInitDMA();

        heap_caps_free(framebuffer);
        heap_caps_free(renderbuffer);
    }

    // HELPER FUNCTIONS --------------
    inline void SwapBuffers() {
        ScreenRow* tempbuffer = renderbuffer;
        renderbuffer = framebuffer;
        framebuffer = tempbuffer;
    }
    inline void DrawData(const screen_pos x, const screen_pos y, const colourID* data, const screen_pos sx, const screen_pos sy) {
        // later add bounds checking ;-;
        for (screen_pos i = y; i < y+sy; i++)
            for (screen_pos j = x; j < x+sx; j++) {
                colourID stencil = *(data++);
                if (stencil) 
                    framebuffer[i][j] = ColourPalette[stencil];
            }
    }

    // Engine.hpp promises ---------

    // clear frame buffer 
    void ClearFrame(const colour col = black) {
        for (screen_pos i = 0U; i < screen_y; i++)
            for (screen_pos j = 0U; j < screen_x; j++)
                framebuffer[i][j] = col;
    }

    void DrawSprite(const SpritePos& pos, const SpriteData& data) {
        DrawData(pos.x, pos.y, data.pxlData, data.width, data.height);
    }
    void DrawSprite(const screen_pos x, const screen_pos y, const SpriteData& data) {
        DrawData(x, y, data.pxlData, data.width, data.height);
    }
    void DrawSprite(const screen_pos x, const screen_pos y, const colourID* sprite, const screen_pos s) {
        DrawData(x, y, sprite, s, s);
    }
    void DrawSprite(const screen_pos x, const screen_pos y, const colourID* sprite, const screen_pos sprite_sx, const screen_pos sprite_sy) {
        DrawData(x, y, sprite, sprite_sx, sprite_sy);
    }

    void PushFrame() {
        // aparently pushImageDMA does this, but if we want our own logic, then this is prob good to do manually
        if (tft.dmaBusy()) tft.dmaWait();

        SwapBuffers();

        tft.startWrite();
        tft.pushImageDMA( 0, 0, screen_x, screen_y, &renderbuffer[0][0]);
        tft.endWrite();
    }
    
}