cmake_minimum_required(VERSION 3.16)

project(DesyncDebugger)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(SDL_TEST_ENABLED_BY_DEFAULT OFF)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(ThirdParty/VideoProcessing/src/VideoFrame)
add_subdirectory(ThirdParty/SDL)
add_subdirectory(ThirdParty/SDL_image)
add_subdirectory(ThirdParty/imgui_cmake)
add_subdirectory(ThirdParty/eigen)

add_executable(DesyncDebugger main.cpp 
    App.cpp 
    App.h 
    FrameSource.cpp
    FrameSource.h 
    CPUProfiler.cpp
    CPUProfiler.h
    ProfilerView.cpp
    ProfilerView.h
    CalibrationView.cpp
    CalibrationView.h
)

target_include_directories(DesyncDebugger PUBLIC ThirdParty/SDL/include)
target_include_directories(DesyncDebugger PUBLIC ThirdParty/SDL_image)
target_include_directories(DesyncDebugger PUBLIC ThirdParty/VideoProcessing/src)
target_include_directories(DesyncDebugger PUBLIC ThirdParty/imgui)
target_include_directories(DesyncDebugger PUBLIC ThirdParty/eigen)

find_package(OpenGL REQUIRED)
target_link_libraries(DesyncDebugger ${OPENGL_LIBRARIES})

target_link_libraries(DesyncDebugger VideoFrame)
target_link_libraries(DesyncDebugger SDL2-static)
target_link_libraries(DesyncDebugger SDL2_image)
target_link_libraries(DesyncDebugger imgui)
