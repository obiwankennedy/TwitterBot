/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: 
 *
 * qdbusxml2cpp is Copyright (C) 2016 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "twitterbotadaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class TwitterBotAdaptor
 */

TwitterBotAdaptor::TwitterBotAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

TwitterBotAdaptor::~TwitterBotAdaptor()
{
    // destructor
}

void TwitterBotAdaptor::quit()
{
    // handle method call local.TwitterBot.quit
    QMetaObject::invokeMethod(parent(), "quit");
}

void TwitterBotAdaptor::sendTwit(const QString &msg)
{
    // handle method call local.TwitterBot.sendTwit
    QMetaObject::invokeMethod(parent(), "sendTwit", Q_ARG(QString, msg));
}

