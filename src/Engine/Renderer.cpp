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
        framebuffer     = Calloc<ScreenRow>(screen_y*2, MemType::GFX);
        if (!framebuffer) {
            Logger.Crit("(Renderer) No DMA memory!!!");
            return;
        }
        renderbuffer    = &framebuffer[screen_y];
        
        tft.init();
        tft.initDMA();
    }
    void ExitRenderer() {
        if (tft.dmaBusy()) tft.dmaWait();

        tft.deInitDMA();

        Free(framebuffer);
        Free(renderbuffer);
    }

    // HELPER FUNCTIONS --------------
    inline bool DataWithinBounds(const screen_pos x, const screen_pos y, const screen_pos sx, const screen_pos sy) {
        return ((x+sx <= screen_x) && (y+sy <= screen_y)) /* upper bounds */
                && (x >= 0 && y >= 0);                  /* lower bounds */
    }
    inline void SwapBuffers() {
        ScreenRow* tempbuffer = renderbuffer;
        renderbuffer = framebuffer;
        framebuffer = tempbuffer;
    }
    inline void DrawData(const screen_pos x, const screen_pos y, const colourID* data, const screen_pos sx, const screen_pos sy) {
        // later add bounds checking ;-; (old, see DrawWithBoubds)
        for (screen_pos i = y; i < y+sy; i++)
            for (screen_pos j = x; j < x+sx; j++) {
                colourID stencil = *(data++);
                if (stencil) 
                    framebuffer[i][j] = ColourPalette[stencil];
            }
    }
    inline void DrawDataStride(const screen_pos x, screen_pos y, const colourID* data, const screen_pos sx, const screen_pos sy, const screen_pos stride) { // yes, one line diff, idc
        for (screen_pos i = y; i < y+sy; i++) {
            for (screen_pos j = x; j < x+sx; j++) {
                colourID stencil = *(data++);
                if (stencil) 
                    framebuffer[i][j] = ColourPalette[stencil];
            }
            data += stride;
        }
    }
    inline void DrawWithBounds(const screen_pos x, const screen_pos y, const colourID* data, const screen_pos sx, const screen_pos sy) {
        // did someone ask, safety?
        // I hate clipping 
        
        // most of this code hopes that the compiler can optimise this inline (I HOPE)

        const screen_pos bx = (x < 0) ? 0 : x;
        const screen_pos by = (y < 0) ? 0 : y;
        const screen_pos ex = (x+sx > screen_x) ? screen_x : x+sx;
        const screen_pos ey = (y+sy > screen_y) ? screen_y : y+sy;


        // be as lazy as possible
        if (bx >= ex || by >= ey) return;

        const screen_pos clipx = bx - x;
        const screen_pos clipy = by - y;

        const screen_pos clipsx = ex - bx;
        const screen_pos clipsy = ey - by;

        /*
            data overview:
            [xxxx]
            [xxAA]
            [xxAA]
            xxxx (sx*clipy (1))
            xx + clipx 
        */
        const colourID* clipdata = data + (clipy*sx) + clipx;
        
        const screen_pos stride = sx - clipsx;

        DrawDataStride(bx, by, clipdata, clipsx, clipsy, stride);
    }

    // Engine.hpp promises ---------
    // (like any good war criminal)

    // clear frame buffer 
    void ClearFrame(const colour col) {
        for (screen_pos i = 0U; i < screen_y; i++)
            for (screen_pos j = 0U; j < screen_x; j++)
                framebuffer[i][j] = col;
    }


    // same logic... this is why I love macros, but they don't fit in namespaces...
    void DrawSprite(const SpritePos& pos, const SpriteData& data) {
        if (DataWithinBounds(pos.x, pos.y, data.width, data.height))
            DrawData(pos.x, pos.y, data.pxlData, data.width, data.height);
        else 
            DrawWithBounds(pos.x, pos.y, data.pxlData, data.width, data.height);
    }
    void DrawSprite(const screen_pos x, const screen_pos y, const SpriteData& data) { // this function may instead fetch data once (if the compiler doesn't optimise enough)
        if (DataWithinBounds(x, y, data.width, data.height))
            DrawData(x, y, data.pxlData, data.width, data.height);
        else 
            DrawWithBounds(x, y, data.pxlData, data.width, data.height);
    }
    void DrawSprite(const screen_pos x, const screen_pos y, const colourID* sprite, const screen_pos s) {
        if (DataWithinBounds(x, y, s, s))
            DrawData(x, y, sprite, s, s);
        else 
            DrawWithBounds(x, y, sprite, s, s);
    }
    void DrawSprite(const screen_pos x, const screen_pos y, const colourID* sprite, const screen_pos sprite_sx, const screen_pos sprite_sy) {
        if (DataWithinBounds(x, y, sprite_sx, sprite_sy))
            DrawData(x, y, sprite, sprite_sx, sprite_sy);
        else 
            DrawWithBounds(x, y, sprite, sprite_sx, sprite_sy);
    }
    void DrawSpriteStride(const screen_pos x, screen_pos y, const colourID* sprite, const screen_pos sprite_sx, const screen_pos sprite_sy, const screen_pos stride) {
        // no mem/bounds check, func is inherently unsafe
        DrawDataStride(x, y, sprite, sprite_sx, sprite_sy, stride);
    }

    void PushFrame() {
        // aparently pushImageDMA does this, but if we want our own logic, then this is prob good to do manually
        if (tft.dmaBusy()) tft.dmaWait();

        // Newton's Negative Fourth law
        SwapBuffers();

        tft.startWrite();
        tft.pushImageDMA( 0, 0, screen_x, screen_y, &renderbuffer[0][0]);
        tft.endWrite();
    }
    
}