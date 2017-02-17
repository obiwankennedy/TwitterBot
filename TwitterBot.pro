
CONFIG += c++11

QT += dbus

template = app
SOURCES += main.cpp Twitter.cpp TwitterBot.cpp json.cpp twitterbotadaptor.cpp


HEADERS += Twitter.h TwitterBot.h json.h credential.h twitterbotadaptor.h

TARGET = twitterBot
#-L/home/renaud/application/mine/TwitterFollowBot/libtwitcurl/libtwitcurl.a \
#-L$$PWD/libtwitcurl/build/ -ltwitcurl \

unix:!macx: LIBS += /usr/lib/x86_64-linux-gnu/libcurl.so

INCLUDEPATH += $$PWD/libtwitcurl \
                $$PWD/libtwitcurl/libtwitcurl/include \



DEPENDPATH += $$PWD/libtwitcurl

include($$PWD/libtwitcurl/libtwitcurl/libtwitcurl.pri)
include($$PWD/diceparser/diceparser.pri)

OTHER_FILES += *.xml
