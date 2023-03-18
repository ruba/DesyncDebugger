#include "App.h"

#include "CPUProfiler.h"

#include "ProfilerView.h"
#include "CalibrationView.h"

#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_sdlrenderer.h"

bool App::Run(FrameSource& frameSource, VideoFrameFilter& filter)
{
    bool pauseScene = false;
    bool pauseForOneFrame = false;

    float timestamp = 0.0f;

    float screenBufferPitch = _windowSize.x * 4;

    std::vector<uint32_t> pixels;
    pixels.resize(_windowSize.x * _windowSize.y);

    VideoFrameFilter::Options filterOptions = filter.GetOptions();
    _calibrationView.SetIRCalibrationPoints(filterOptions.calibrationPoints);

    while (!_quit)
    {
        SDL_Event event;

        if (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
            {
                _quit = true;
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_SPACE:
                    pauseScene = !pauseScene;
                    break;
                case SDL_SCANCODE_RIGHT:
                    pauseForOneFrame = true;
                    pauseScene = false;
                    break;
                }
            }
        }

        if (!pauseScene)
        {
            frameSource.Update();
        }

        frameSource.Render(_renderer, _backBuffer, _lockableBackBuffer);

        SDL_SetRenderTarget(_renderer, NULL);
        SDL_RenderCopy(_renderer, _backBuffer, NULL, NULL);

        SDL_RenderReadPixels(_renderer, nullptr, SDL_PIXELFORMAT_RGBA8888, &pixels[0], screenBufferPitch);

        for (uint32_t x = 0; x < _windowSize.x; x++)
        {
            for (uint32_t y = 0; y < _windowSize.y; y++)
            {
                _filterFrameBuffer[x + y * _windowSize.x] = uint8_t((pixels[x + y * _windowSize.x] >> 16));
            }
        }

        if (!pauseScene)
        {
            CPU_PROFILER_SCOPE(FilterProcess)

            VideoFrameRaw rendererFrame(_filterFrameBuffer, _windowSize.x, _windowSize.y, timestamp);
            filter.ProcessFrame(rendererFrame, _targets);

            timestamp += 1.0f;
        }

        DrawOverlay(filter, _targets, true);

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame(_window);
        ImGui::NewFrame();

        DrawUI(filter);

        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        SDL_RenderPresent(_renderer);

        if (!pauseScene)
        {
            CPUProfiler::Update();
        }

        if (pauseForOneFrame)
        {
            pauseForOneFrame = false;
            pauseScene = true;
        }
    }

    float2 calibrationPoints[4];
    _calibrationView.GetIRCalibrationPoints(calibrationPoints);
    filter.SetCalibrationPoints(calibrationPoints);

    matrix3f calibrationMatrix;
    matrix3f invCalibrationMatrix;
    if (_calibrationView.GetIRCalibrationMatrix(calibrationMatrix, invCalibrationMatrix))
    {
        filter.SetCalibrationMatrix(calibrationMatrix, invCalibrationMatrix);
    }

    return true;
}

bool App::Init(uint2 windowSize)
{
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_OVERRIDE);

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Failed to initialise SDL\n";
        return false;
    }

    _windowSize = windowSize;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    _window = SDL_CreateWindow("VideoFrameViewer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowSize.x, windowSize.y,
        SDL_WINDOW_OPENGL | SDL_RENDERER_PRESENTVSYNC);

    if (_window == nullptr)
    {
        SDL_Log("Could not create a window: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

    if (_renderer == nullptr)
    {
        SDL_Log("Could not create a renderer: %s", SDL_GetError());
        return false;
    }

    SDL_RendererInfo renderInfo;
    if (SDL_GetRendererInfo(_renderer, &renderInfo) == 0)
    {
        SDL_Log("Renderer driver [%s] has been created", renderInfo.name);
    }

    uint32_t filterFrameBufferSize = windowSize.x * windowSize.y * sizeof(char);
    _filterFrameBuffer = static_cast<uint8_t*>(malloc(filterFrameBufferSize));

    _backBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, int(windowSize.x), int(windowSize.y));

    _lockableBackBuffer = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, int(windowSize.x), int(windowSize.y));

    _glContext = SDL_GL_CreateContext(_window);

    SDL_GL_MakeCurrent(_window, _glContext);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    if (!ImGui_ImplSDL2_InitForOpenGL(_window, _glContext))
    {
        SDL_Log("Could not create OpenGL renderer for SDL2");
        return false; 
    }

    if (!ImGui_ImplSDLRenderer_Init(_renderer))
    {
        SDL_Log("Could not create OpenGL3 renderer for ImGui");
        return false;   
    }

    _quit = false;
    _showProfiler = true;
    _showScanRows = false;

    return true;
}

void App::Shutdown()
{
    free(_filterFrameBuffer);

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyTexture(_lockableBackBuffer);
    SDL_DestroyTexture(_backBuffer);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);

    SDL_Quit();
}

void App::DrawOverlay(VideoFrameFilter& filter, std::vector<VideoFrameFilter::Target> const& targets, bool showSearchArea)
{
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);

    if (_showTargets)
    {
        for (auto const& target : targets)
        {
            VideoFrameFilter::Rect const& bounds = target.bounds;

            SDL_Rect targetBounds = {
                static_cast<int>(bounds.minX),
                static_cast<int>(bounds.minY),
                static_cast<int>(bounds.maxX - bounds.minX),
                static_cast<int>(bounds.maxY - bounds.minY)
            };

            if (target.rfidIsValid)
            {
                SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 0);
            }
            else
            {
                SDL_SetRenderDrawColor(_renderer, 0, 0, 255, 0);
            }

            SDL_RenderDrawRect(_renderer, &targetBounds);

            SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 0);
            SDL_RenderDrawLine(_renderer, target.center.x - 3, target.center.y, target.center.x + 3, target.center.y);
            SDL_RenderDrawLine(_renderer, target.center.x, target.center.y - 3, target.center.x, target.center.y + 3);

            if (showSearchArea)
            {
                SDL_Rect searchBounds =
                {
                    static_cast<int>(target.minBounds.x),
                    static_cast<int>(target.minBounds.y),
                    static_cast<int>(target.maxBounds.x - target.minBounds.x),
                    static_cast<int>(target.maxBounds.y - target.minBounds.y)
                };

                SDL_SetRenderDrawColor(_renderer, 0, 255, 0, 0);
                SDL_RenderDrawRect(_renderer, &searchBounds);
            }
        }
    }


    if (_showScanRows)
    {
        SDL_Rect scanRowArea =
        {
            static_cast<int>(0),
            static_cast<int>(filter.GetCurrentScanRow()),
            static_cast<int>(_windowSize.x),
            static_cast<int>(filter.GetScanRowsCountPerFrame())
        };

        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 64);
        SDL_RenderFillRect(_renderer, &scanRowArea);
    }
}

void App::DrawUI(VideoFrameFilter& filter)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                _quit = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Profiler"))
            {
                _showProfiler = !_showProfiler;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("FilterSettings"))
    {
        int threshold = static_cast<int>(filter.GetThreshold());
        if (ImGui::DragInt("Threshold", &threshold, 1.0f, 0, 255))
        {
            filter.SetThreshold(threshold);
        }

        ImGui::LabelText("Targets", "%d", _targets.size());

        ImGui::SameLine();

        if (ImGui::Button("Clear"))
        {
            filter.ClearTrackedTargets();
            _targets.clear();
        }

        if (ImGui::Checkbox("Show scan rows", &_showScanRows));
        if (ImGui::Checkbox("Show targets", &_showTargets));
    }
    ImGui::End();

    if (_showProfiler)
    {
        ProfilerView::Show(_showProfiler);
    }

    if (_showCalibration)
    {
        _calibrationView.Show(_showCalibration, _renderer);
    }
}
