QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

TRANSLATIONS += \
    UIApp_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../CVisionLib/release/ -lCVisionLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../CVisionLib/debug/ -lCVisionLib
else:unix: LIBS += -L$$OUT_PWD/../CVisionLib/ -lCVisionLib

INCLUDEPATH += $$PWD/../CVisionLib
DEPENDPATH += $$PWD/../CVisionLib
