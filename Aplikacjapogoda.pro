QT += core gui widgets charts network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_CHARTS_LIB
INCLUDEPATH += D:/QT/6.8.3/msvc2022_64/include
LIBS += -LD:/QT/6.8.3/msvc2022_64/lib


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    oknodanych.cpp \
    oknostacji.cpp \
    plikijson.cpp

HEADERS += \
    mainwindow.h \
    oknodanych.h \
    oknostacji.h \
    plikijson.h

FORMS += \
    mainwindow.ui \
    oknodanych.ui \
    oknostacji.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
