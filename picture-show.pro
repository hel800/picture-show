#-------------------------------------------------
#
# Project created by QtCreator 2012-02-18T12:13:45
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = picture-show
TEMPLATE = app

SOURCES += main.cpp\
    mainwidget.cpp \
    loadimage.cpp \
    animation.cpp \
    settingsdialog.cpp \
    exif.cpp \
    loaddirectory.cpp \
    overlaybase.cpp \
    overlayhelp.cpp \
    overlaymenu.cpp \
    loadlibrary.cpp

HEADERS  += mainwidget.h \
    loadimage.h \
    animation.h \
    settingsdialog.h \
    readExif.h \
    exif.h \
    global.h \
    loaddirectory.h \
    overlaybase.h \
    overlayhelp.h \
    overlaymenu.h \
    loadlibrary.h

FORMS    += mainwidget.ui \
    settingsdialog.ui

RESOURCES += \
    resource.qrc

#RC_FILE = picture-show_icon.rc

TRANSLATIONS = picture-show_en.ts
