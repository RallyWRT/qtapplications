# ##
TEMPLATE = lib
CONFIG += plugin
TARGET = usermanager

# VERSION = 2009.09.08.1055
QT += core \
    gui \
    sql

# include config file
include( ../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI
win32 {
    HHSharedLibs += HHSharedWindowsManagement
}
include(../../HHSharedLibs.pri)

# #
INCLUDEPATH += $$UI_DIR \
    $$PWD

# ##
UI_DIR = ./ui

# Input
HEADERS += usermanagerplugin.h \
    usermanagermainwindow.h \
    modifyuserinfo/modifyuserinfodialog.h
FORMS +=  usermanager.ui \
    modifyuserinfo/modifyuserinfodialog.ui
SOURCES += usermanagerplugin.cpp \
    usermanagermainwindow.cpp \
    modifyuserinfo/modifyuserinfodialog.cpp
RESOURCES += usermanager.qrc

win32 {
    HEADERS += settingsdialog/settingsdialog.h
    SOURCES += settingsdialog/settingsdialog.cpp
    FORMS += settingsdialog/settingsdialog.ui
    HHSharedLibs += HHSharedWindowsManagement
}


# define some usefull values
QMAKE_TARGET_PRODUCT = "User Manager"
QMAKE_TARGET_DESCRIPTION = "Crossplatform User Manager For Sitoy Based On Qt"