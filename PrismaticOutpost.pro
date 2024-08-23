include(path/to/vcpkg/scripts/buildsystems/vcpkg.cmake)

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QT += kddockwidgets
CONFIG += kddockwidgets_qt6  # or kddockwidgets_qt5 for Qt 5

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    databasemanager.cpp \
    itemwindow.cpp \
    main.cpp \
    menuwindow.cpp \
    prismaticoutpost.cpp \
    scripteditor.cpp \
    toolwindow.cpp

HEADERS += \
    databasemanager.h \
    itemwindow.h \
    menuwindow.h \
    prismaticoutpost.h \
    scripteditor.h \
    toolwindow.h

TRANSLATIONS += \
    PrismaticOutpost_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target