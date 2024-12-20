QT += widgets
CONFIG += c++17

# Include paths
INCLUDEPATH += /usr/include/opencv4

# Link OpenCV libraries
LIBS += `pkg-config --libs opencv4`

# Source and header files
SOURCES += main.cpp \
           CameraApp.cpp \
           fproc.cpp 
           

HEADERS += CameraApp.h \
           fproc.h