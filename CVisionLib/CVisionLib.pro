QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += CVISIONLIB_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    UI/imageview.cpp \
    cvisionlib.cpp

HEADERS += \
    CVisionLib_global.h \
    UI/imageview.h \
    cvisionlib.h

TRANSLATIONS += \
    CVisionLib_zh_CN.ts

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$(OPENCV_SDK_DIR)/x64/vc15/lib/ -lopencv_world451
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(OPENCV_SDK_DIR)/x64/vc15/lib/ -lopencv_world451d

INCLUDEPATH += $$(OPENCV_SDK_DIR)/include/opencv2
