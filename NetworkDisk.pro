QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
INCLUDEPATH+=./netapi/
INCLUDEPATH+=./md5/
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ckernel.cpp \
    logindialog.cpp \
    main.cpp \
    maindialog.cpp \
    test.cpp

HEADERS += \
    ckernel.h \
    common.h \
    logindialog.h \
    maindialog.h \
    test.h

FORMS += \
    logindialog.ui \
    maindialog.ui

include(./netapi/netapi.pri);
include(./md5/md5.pri);

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    resource.qrc

DISTFILES += \
    resouce/face/btn_avatar_a19.png
