#include <iostream>
#include <memory.h>

#include "SDL.h"
#undef main

#include "VideoFrame/VideoStream.h"
#include "VideoFrame/VideoFrameFilter.h"
#include "VideoFrame/VideoFrameFilterConfig.h"

#include "App.h"

const uint32_t screenWidth = 1920;
const uint32_t screenHeight = 1080;

const uint32_t numberOfVideoFrames = 1807;
const uint32_t videoFrameIdxOffset = 0;

const uint32_t filePathBufferSize = 255;
char filepath[filePathBufferSize];

VideoStreamFrameSource::Options sourceOptions = {
    "../videostream.raw",
    numberOfVideoFrames,
    screenWidth,
    screenHeight,
    videoFrameIdxOffset
};

const VideoFrameFilter::Algorithm filterAlgorithm = VideoFrameFilter::Algorithm::RowScan;
const uint32_t filterThreshold = 254;
const uint32_t filterCellSize = 50;
const uint32_t filterNumberOfRowsToAnalyze = 100;
const uint32_t filterTargetMinimalSize = 1;
const std::bitset<16> filterRFID{ "1100100100011011" };

VideoFrameFilter::Options filterOptions = {
    filterAlgorithm,
    filterNumberOfRowsToAnalyze,
    filterTargetMinimalSize,
    filterThreshold,
    filterCellSize,
    {float2(0.0f,0.0f), float2(100.0f, 0.0f), float2(100.0f, 100.0f), float2(0.0f, 100.0f)}
};

enum class FrameInput
{
    VideoStream,
    Render
};

FrameInput frameInput = FrameInput::Render;

void ParseCmdLineArguments(int argc, char** argv, VideoStreamFrameSource::Options& options)
{
    if (argc <= 1)
    {
        SDL_Log("No arguments provided. Enabling rendering mode");
        return;
    }

    if (SDL_sscanf(argv[1], "-videopath=%s", filepath) == 0)
    {
        SDL_Log("Cmdline arguments are invalid. Enabling rendering mode");
        return;
    }

    frameInput = FrameInput::VideoStream;
    options.filename = filepath;
}

int main(int argc, char** argv)
{
    App app;

    if (!app.Init(uint2(screenWidth, screenHeight)))
    {
        return -1;
    }

    ParseCmdLineArguments(argc, argv, sourceOptions);

    FrameSource* frameSource = nullptr;
    
    if (frameInput == FrameInput::VideoStream)
    {
        frameSource = new VideoStreamFrameSource();
    }
    else
    {
        frameSource = new RenderFrameSource();
    }

    if (!frameSource->Init(sourceOptions))
    {
        SDL_Log("Can't init frame source");
        return false;
    }

    VideoFrameFilterConfig::Load("filter_config.json", filterOptions);

    VideoFrameFilter frameFilter(filterOptions);
    
    app.Run(*frameSource, frameFilter);

    VideoFrameFilterConfig::Save("filter_config.json", frameFilter.GetOptions());

    frameSource->Shutdown();
    delete frameSource;

    app.Shutdown();
}
