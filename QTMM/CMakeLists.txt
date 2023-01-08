cmake_minimum_required(VERSION 3.19)

project(QTMM)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif ()

add_executable(QTMM main.cpp mainform.ui mainform.h mainform.cpp initdialog.ui initdialog.h initdialog.cpp
                    CIGLWidget.cpp CIGLWidget.h
                    ../CI/CITrackBall.cpp ../CI/CITrackBall.h ../CI/CISG.cpp ../CI/CISG.h
                    ../CI/CICubeMap.cpp ../CI/CICubeMap.h ../CI/CIMsg.cpp ../CI/CIMsg.h
                    ../MM/MMSG.cpp ../MM/MMSG.h ../MM/MMGrid.cpp ../MM/MMGrid.h)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Xml Widgets OpenGL REQUIRED)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/../)
target_link_libraries(QTMM ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} Qt5::Widgets Qt5::Gui Qt5::Xml Qt5::Core Qt5::OpenGL)
