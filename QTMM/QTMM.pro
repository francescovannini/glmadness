TEMPLATE	= app 
LANGUAGE	= C++

CONFIG	+= qt warn_on release debug opengl exceptions

win32:LIBS	+= c:/code/glew/lib/glew32.lib
INCLUDEPATH	+= .. C:/CODE/glew/include

HEADERS	+= CIGLWidget.h \
	../CI/CISG.h \
	../CI/CICubeMap.h \
	../CI/CITrackBall.h \
	../vcg/Point3.h \
	../vcg/Matrix44.h \
	../MM/MMSG.h \
	../MM/MMGrid.h
SOURCES	+= main.cpp \
	CIGLWidget.cpp \
	../CI/CITrackBall.cpp \
	../CI/CISG.cpp \
	../CI/CICubeMap.cpp \
	../CI/CIMsg.cpp \
	../MM/MMSG.cpp \
	../MM/MMGrid.cpp
FORMS	= mainform.ui \
	initdialog.ui
IMAGES	= images/filenew \
	images/fileopen \
	images/filesave \
	images/home.png \
	images/exit.bmp \
	images/arrow.bmp \
	images/track.bmp \
	images/updown.bmp





























































































































































































































unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
win32 {
  UI_DIR = ui
  MOC_DIR = moc
  OBJECTS_DIR = obj
}

