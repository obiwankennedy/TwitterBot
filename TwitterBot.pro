
CONFIG += c++11

template = app
SOURCES += main.cpp Twitter.cpp TwitterBot.cpp json.cpp


HEADERS += Twitter.h TwitterBot.h json.h credential.h

TARGET = twitterBot
#-L/home/renaud/application/mine/TwitterFollowBot/libtwitcurl/libtwitcurl.a \
#-L$$PWD/libtwitcurl/build/ -ltwitcurl \

unix:!macx: LIBS += -L/usr/lib/x86_64-linux-gnu/ -lcurl

INCLUDEPATH += $$PWD/libtwitcurl \
                $$PWD/libtwitcurl/libtwitcurl/include \



DEPENDPATH += $$PWD/libtwitcurl

include($$PWD/libtwitcurl/libtwitcurl/libtwitcurl.pri)
include($$PWD/diceparser/diceparser.pri)
