TEMPLATE = app
CONFIG += console release debug opengl exceptions
INCLUDEPATH += .. c:\code\SDL-1.2.7\include
LIBPATH += c:\code\SDL-1.2.7\lib
win32:LIBS +=  sdlmain.lib sdl.lib c:/code/glew/lib/glew32.lib

# Input
HEADERS += ../vcg/Matrix44.h ../vcg/Point3.h ../vcg/Point4.h ../vcg/Utility.h ../CI/CISG.h ../bmf/BMF_font.h \
	../CI/CICubeMap.h \
	../CI/CISG.h 
	
SOURCES += main.cpp \
	../bmf/BMF_font.c \
	../CI/CISG.cpp \
	../CI/CIMd2.cpp \
	../CI/CIMd2normals.cpp \
	../CI/CITrackBall.cpp \
	../CI/CICubeMap.cpp \
	../CI/CIMsg.cpp \
	../MM/MMSG.cpp \
	../MM/MMGrid.cpp
LANGUAGE	= C++ 