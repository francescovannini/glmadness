cmake_minimum_required(VERSION 3.16)
project(SDLMM)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

add_executable(SDLMM main.cpp
        ../bmf/BMF_font.c
        ../CI/CISG.cpp ../CI/CISG.h ../CI/CIMd2.cpp ../CI/CIMd2.h ../CI/CIMd2Normals.cpp
        ../CI/CITrackBall.cpp ../CI/CITrackBall.h ../CI/CICubeMap.cpp ../CI/CICubeMap.h ../CI/CIMsg.cpp ../CI/CIMsg.h
        ../MM/MMSG.cpp ../MM/MMSG.h ../MM/MMGrid.cpp ../MM/MMGrid.h)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Xml Widgets OpenGL REQUIRED)
include_directories(${SDL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../)
target_link_libraries(SDLMM ${SDL_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} Qt5::Widgets Qt5::Gui Qt5::Xml Qt5::Core Qt5::OpenGL)