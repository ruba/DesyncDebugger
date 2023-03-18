#pragma once

#include "SDL.h"

#include "VideoFrame/Common.h"
#include "VideoFrame/VideoStream.h"

class FrameSource
{
public:
    struct Options
    {
        const char* filename;

        uint32_t numberOfVideoFrames;
        uint32_t width;
        uint32_t height;
        uint32_t frameOffset;
    };
public:
    virtual ~FrameSource(){};

    virtual bool Init(Options const& options) = 0;
    virtual void Update() = 0;
    virtual void Render(SDL_Renderer* renderer, SDL_Texture* backbuffer, SDL_Texture* lockableBackbuffer) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsFirstFrame() = 0;
};

class VideoStreamFrameSource : public FrameSource
{
public:
    bool Init(Options const& options);
    void Update();
    void Render(SDL_Renderer* renderer, SDL_Texture* backbuffer, SDL_Texture* lockableBackbuffer);
    void Shutdown();
    bool IsFirstFrame();
protected:
    VideoStream* InitVideoStream(const char* filename, uint32_t width, uint32_t height);
private:
    uint32_t _videoFrameIdx = 0;
    uint32_t _videoFrameCounter = 0;
    uint32_t _playableNumberOfVideoFrames = 0;

    uint32_t _videoFrameIdxOffset;
    uint32_t _numberOfVideoFrames;
    uint32_t _frameWidth;
    uint32_t _frameHeight;

    VideoStream* _videoStream = nullptr;
};

class RenderFrameSource : public FrameSource
{
public:
    bool Init(Options const& options);
    void Update();
    void Render(SDL_Renderer* renderer, SDL_Texture* backbuffer, SDL_Texture* lockableBackbuffer);
    void Shutdown();
    bool IsFirstFrame();
protected:
    float _renderingDelay = 15.0f;
    float _phase = 0.0f;
};
