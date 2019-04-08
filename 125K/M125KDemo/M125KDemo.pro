#-------------------------------------------------
#
# Project created by QtCreator 2016-09-23T16:40:01
#
#-------------------------------------------------

QT       += core gui\
        sql\
        serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = M125KDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tools.cpp \
    database.cpp \
    recordtablemodel.cpp

HEADERS  += mainwindow.h \
    tools.h \
    database.h \
    recordtablemodel.h

FORMS    += mainwindow.ui

win32: LIBS += -L$$PWD/lib/ -lM125Dll

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

DISTFILES += \
    lib/libM125Dll.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lM125Dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lM125Dlld
else:unix: LIBS += -L$$PWD/lib/ -lM125Dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libM125Dll.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libM125Dlld.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/M125Dll.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/M125Dlld.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libM125Dll.a
