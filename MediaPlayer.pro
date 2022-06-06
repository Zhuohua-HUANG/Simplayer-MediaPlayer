QT       += core gui
QT += multimediawidgets
QT += opengl
QT += gui
QT += network \
      xml \
      multimedia \
      widgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Widget/VideoFrameDisplay.cpp \
    Widget/VideoSlider.cpp \
    Widget/videowidget.cpp \
    audioimage.cpp \
    getaudio.cpp \
    main.cpp \
    mainwindow.cpp \
    reversedecode.cpp \
    reversedisplay.cpp \
    seekFrame.cpp \
    utils.cpp

HEADERS += \
    Widget/VideoFrameDisplay.h \
    Widget/VideoSlider.h \
    Widget/videowidget.h \
    audioimage.h \
    getaudio.h \
    mainwindow.h \
    reversedecode.h \
    reversedisplay.h \
    seekFrame.h \
    utils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ImageResource.qrc


win32: {
    FFMPEG_HOME=$$PWD/ffmpeg
#    SDL_HOME=$$PWD/SDL2-2.0.20
    #设置 ffmpeg 的头文件
    INCLUDEPATH += $$FFMPEG_HOME/include
#    INCLUDEPATH += $$SDL_HOME/include
    #设置导入库的目录一边程序可以找到导入库
    # -L ：指定导入库的目录
    # -l ：指定要导入的 库名称
    LIBS +=  -L$$FFMPEG_HOME/lib \
             -lavcodec \
             -lavdevice \
             -lavfilter \
            -lavformat \
            -lavutil \
            -lpostproc \
            -lswresample \
            -lswscale
    GLFW_HOME=$$PWD/glfw
    INCLUDEPATH+=$$GLFW_HOME/include
    DEPENDPATH += $$GLFW_HOME/include
    LIBS += -lOpenGL32
#    LIBS += -L$$GLFW_HOME/lib-vc2017 -lglfw3dll
#    LIBS +=  -L$$SDL_HOME/lib/x64 -lSDL2 -lSDL2main -lSDL2test

}
