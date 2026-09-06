/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <BinF/Engine/Renderer.hpp>
#include <BinF/Engine/Palette.hpp>
#include <BinF/Engine/Internal.hpp>

#if BINF_PLATFORM != DESKTOP_SDL
#include <TFT_eSPI.h>
#else 
#include <SDL3/SDL.h>
#endif

namespace BinF::Engine {
    #if BINF_PLATFORM != DESKTOP_SDL
    TFT_eSPI tft = TFT_eSPI();
    #else
    SDL_Window*     Window      = nullptr;
    SDL_Renderer*   Renderer    = nullptr;
    SDL_Texture*    FrameText   = nullptr;
    #endif

    using ScreenRow = colour[screen_x];

    // frame buffer memory
    ScreenRow* framebuffer = nullptr;
    ScreenRow* renderbuffer = nullptr;

    // RESOURCE MANAGEMENT -----------
    void InitRenderer() {
        framebuffer     = Calloc<ScreenRow>(screen_y*2, MemType::GFX);
        if (!framebuffer) {
            Logger.Crit("(Renderer) Not enough DMA memory!!!");
            return;
        }
        renderbuffer    = &framebuffer[screen_y];
        
        #if BINF_PLATFORM != DESKTOP_SDL
        #if BINF_PLATFORM == FRI3D2026
        tft.setRotation(3);
        #endif
        tft.init();
        tft.initDMA();
        #if BINF_PLATFORM == FRI3D2024
        tft.writecommand(TFT_MADCTL);
        tft.writedata(TFT_MAD_BGR | TFT_MAD_MV);
        #endif
        tft.setSwapBytes(true);
        tft.fillScreen(TFT_BLACK);
        #else

        // (Tommy)Init Video System
        SDL_Init(SDL_INIT_VIDEO);
        
        
        Window = SDL_CreateWindow("BinF | Desktop", screen_x, screen_y, SDL_WINDOW_RESIZABLE);
        SDL_SetWindowMinimumSize(Window, screen_x, screen_y);

        Renderer =  SDL_CreateRenderer(Window, nullptr);
        FrameText = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, screen_x, screen_y);

        SDL_SetTextureScaleMode(FrameText, SDL_SCALEMODE_PIXELART);
        SDL_SetRenderLogicalPresentation(Renderer, screen_x, screen_y, SDL_LOGICAL_PRESENTATION_LETTERBOX);

        #endif
    }
    void ExitRenderer() {
        #if BINF_PLATFORM != DESKTOP_SDL
        if (tft.dmaBusy()) tft.dmaWait();

        tft.deInitDMA();
        #else

        SDL_DestroyTexture(FrameText);
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyWindow(Window);

        #endif

        Free(framebuffer);
        Free(renderbuffer);
    }

    #if BINF_PLATFORM != DESKTOP_SDL
    SPIClass& GetSPI() {
        return tft.getSPIinstance();
    }
    #endif

    // HELPER FUNCTIONS --------------
    inline bool DataWithinBounds(const screen_pos x, const screen_pos y, const screen_pos sx, const screen_pos sy) {
        return ((x+sx <= screen_x) && (y+sy <= screen_y)) /* upper bounds */
                && (x >= 0 && y >= 0);                  /* lower bounds */
    }
    inline static void SwapBuffers() {
        ScreenRow* tempbuffer = renderbuffer;
        renderbuffer = framebuffer;
        framebuffer = tempbuffer;
    }
    inline static void DrawData(const screen_pos x, const screen_pos y, const colourID* data, const screen_pos sx, const screen_pos sy) {
        // later add bounds checking ;-; (old, see DrawWithBoubds)
        for (screen_pos i = y; i < y+sy; i++)
            for (screen_pos j = x; j < x+sx; j++) {
                colourID stencil = *(data++);
                if (stencil) 
                    framebuffer[i][j] = ColourPalette[stencil];
            }
    }
    inline static void DrawDataStride(const screen_pos x, screen_pos y, const colourID* data, const screen_pos sx, const screen_pos sy, const screen_pos stride) { // yes, one line diff, idc
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
        ReqTime();
        Logger.Info("(Renderer) Starting Clear Frame...");
        for (screen_pos i = 0U; i < screen_y; i++)
            for (screen_pos j = 0U; j < screen_x; j++)
                framebuffer[i][j] = col;
        Logger.Info("(Renderer) Clearing took %u seconds", ReqTime());
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
        #if BINF_PLATFORM != DESKTOP_SDL
        // aparently pushImageDMA does this, but if we want our own logic, then this is prob good to do manually
        if (tft.dmaBusy()) tft.dmaWait();
        tft.endWrite();
        #endif

        // Newton's Negative Fourth law
        SwapBuffers();

        #if BINF_PLATFORM != DESKTOP_SDL
        tft.startWrite();
        #endif
        
        #if BINF_PLATFORM == FRI3D2024
        tft.pushImageDMA( 0, 0, screen_x, screen_y, &renderbuffer[0][0] );
        #elif BINF_PLATFORM == FRI3D2026
        tft.pushImageDMA( 13, 0, screen_x, screen_y, &renderbuffer[0][0] );
        #elif BINF_PLATFORM == DESKTOP_SDL
        
        SDL_UpdateTexture(FrameText, nullptr, &renderbuffer[0][0], screen_x * sizeof(colour));
        
        // Drawnion Particle
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);

        SDL_RenderTexture(Renderer, FrameText, nullptr, nullptr);
        SDL_RenderPresent(Renderer);


        #else
        #error [BinF] Engine Renderer not supported for platform
        #endif

    }

    void WaitForSPI() {
        #if BINF_PLATFORM != DESKTOP_SDL
        tft.endWrite();
        #endif  
    }
    
}