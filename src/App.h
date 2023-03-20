#pragma once

#include "Common.h"
#include "AppUI.h"

#include "SDL3/SDL.h"

class App
{
public:
    bool Run(AppUI* AppUI);
    bool Init(uint2 windowSize);
    void Shutdown();
protected:
    void DrawUI(AppUI* AppUI);
private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _backBuffer;
    SDL_Texture* _lockableBackBuffer;
    SDL_GLContext _glContext;

    uint2 _windowSize;

    bool _showProfiler;
    bool _showScanRows;
    bool _showTargets;

    bool _quit;
};
