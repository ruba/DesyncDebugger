#include <iostream>
#include <memory.h>

#include "SDL3/SDL.h"
#undef main

#include "App.h"

const uint32_t screenWidth = 1920;
const uint32_t screenHeight = 1080;

void ParseCmdLineArguments(int argc, char** argv/*, VideoStreamFrameSource::Options& options*/)
{
    if (argc <= 1)
    {
        SDL_Log("No arguments provided. Enabling rendering mode");
        return;
    }
}

int main(int argc, char** argv)
{
    App app;

    if (!app.Init(uint2(screenWidth, screenHeight)))
    {
        return -1;
    }

    ParseCmdLineArguments(argc, argv);

    app.Run();

    app.Shutdown();
}
