
# include global  config file
include( ../config_global.pri )


VER_MAJ = 2012
VER_MIN = 2
VER_PAT = 27
VER_CMP = 1

VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}.$${VER_CMP}

#For UDT
win32:win32-g++:DEFINES += __MINGW__

# define some usefull values
QMAKE_TARGET_COMPANY	= "He Hui"
QMAKE_TARGET_PRODUCT	= "Management System"
QMAKE_TARGET_DESCRIPTION    = "Crossplatform Tools Based On Qt Of LGPL Edition"
QMAKE_TARGET_COPYRIGHT	= "Copyright (C) 2007 - 2010 He Hui"

APP_ORG = "He Hui"
APP_AUTHOR = "He Hui"
APP_AUTHOR_EMAIL = "hehui@hehui.org"
APP_LICENSE = "LGPL"

# define variable for source code
#DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\"" \
DEFINES	*= "APP_VERSION=\"\\\"$${VERSION}\\\"\"" \
        "APP_ORG=\"\\\"$${APP_ORG}\\\"\"" \
        "APP_ORG_DOMAIN=\"\\\"$${PACKAGE_DOMAIN}\\\"\"" \
        "APP_AUTHOR=\"\\\"$${APP_AUTHOR}\\\"\"" \
        "APP_AUTHOR_EMAIL=\"\\\"$${APP_AUTHOR_EMAIL}\\\"\"" \
         "APP_LICENSE=\"\\\"$${APP_LICENSE}\\\"\"" \
        "APP_COPYRIGHTS=\"\\\"$${QMAKE_TARGET_COPYRIGHT}\\\"\""
