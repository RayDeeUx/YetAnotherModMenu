cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
	set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(YetAnotherModMenu VERSION 1.0.0)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED
    src/Manager.cpp
    src/Manager.hpp
    src/Utils.cpp
    src/Utils.hpp

    src/eclipse/config.hpp
    src/eclipse/eclipse.hpp
    src/eclipse/labels.hpp
    src/eclipse/modules.hpp
    src/eclipse/components.hpp
    src/eclipse/eclipseIntegration.cpp

    src/MusicDownloadManager.cpp
    src/FMODAudioEngine.cpp
    src/CCMotionStreak.cpp
    src/PlayerObject.cpp
    src/HardStreak.cpp
    src/PauseLayer.cpp
    src/PlayLayer.cpp
    src/MenuLayer.cpp
    # Add any extra C++ source files here
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})
