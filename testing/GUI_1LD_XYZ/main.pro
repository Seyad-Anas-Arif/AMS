QT += widgets
CONFIG += c++17

# Include paths
INCLUDEPATH += /usr/include/opencv4

TARGET = SLD_XYZ

# Link OpenCV libraries
LIBS += `pkg-config --libs opencv4`

# Source and header files
SOURCES += main.cpp 
           
           

#HEADERS += 
         