#pragma once

#include "SDL.h"

#include "FrameSource.h"

#include "VideoFrame/Common.h"
#include "VideoFrame/VideoStream.h"
#include "VideoFrame/VideoFrameFilter.h"

#include "CalibrationView.h"

class App
{
public:
    bool Run(FrameSource& frameSource, VideoFrameFilter& filter);
    bool Init(uint2 windowSize);
    void Shutdown();
protected:
    void DrawOverlay(VideoFrameFilter& filter, std::vector<VideoFrameFilter::Target> const& targets, bool showSearchArea);
    void DrawUI(VideoFrameFilter& filter);
private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _backBuffer;
    SDL_Texture* _lockableBackBuffer;
    SDL_GLContext _glContext;

    uint2 _windowSize;
    uint8_t* _filterFrameBuffer;

    std::vector<VideoFrameFilter::Target> _targets;

    bool _showProfiler;
    bool _showCalibration;
    bool _showScanRows;
    bool _showTargets;

    bool _quit;

    CalibrationView _calibrationView;
};
