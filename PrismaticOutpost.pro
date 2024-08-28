QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Conan 2.0 manual integration ################################################################################
CONAN_INCLUDEPATH = $$system(conan inspect . --format=json | jq -r '.include_paths[]' | tr '\n' ' ')          #
CONAN_LIBS = $$system(conan inspect . --format=json | jq -r '.lib_paths[]' | sed 's/^/-L/' | tr '\n' ' ')     #
CONAN_BINDIRS = $$system(conan inspect . --format=json | jq -r '.bin_paths[]' | sed 's/^/-L/' | tr '\n' ' ')  #
CONAN_LIBS += $$system(conan inspect . --format=json | jq -r '.libs[]' | sed 's/^/-l/' | tr '\n' ' ')         #
                                                                                                              #
INCLUDEPATH += $$CONAN_INCLUDEPATH                                                                            #
LIBS += $$CONAN_LIBS $$CONAN_BINDIRS                                                                          #
###############################################################################################################


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    databasemanager.cpp \
    main.cpp \
    prismaticoutpost.cpp \
    script.cpp \
    scripteditor.cpp \
    toolwindow.cpp

HEADERS += \
    databasemanager.h \
    prismaticoutpost.h \
    script.h \
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

DISTFILES += \
    conanfile.txt
