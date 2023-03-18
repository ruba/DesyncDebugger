cmake_minimum_required(VERSION 3.16)

project(imgui)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

include_directories(../imgui)
include_directories(../SDL/include)

set (IMGUI_SRC 
    ../imgui/imgui.cpp
    ../imgui/imgui.h
    ../imgui/imgui_draw.cpp
    ../imgui/imgui_widgets.cpp
    ../imgui/imgui_tables.cpp
    ../imgui/backends/imgui_impl_sdlrenderer.cpp
    ../imgui/backends/imgui_impl_sdl.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC ../SDL/include)
target_include_directories(imgui PUBLIC ../imgui)
