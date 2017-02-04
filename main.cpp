#include <QCoreApplication>

#include <QDebug>

#include "Twitter.h"
#include "TwitterBot.h"
#include "credential.h"
#include "twitterbotadaptor.h"

int main(int argc, char *argv[]){


    QCoreApplication app(argc,argv);
    app.setApplicationName("TwitterBot");

    std::ios_base::sync_with_stdio(false);
    qDebug() <<ACCESS_TOKEN  << ACCESS_TOKEN_SECRET ;
    Twitter t = Twitter(CONSUMER_KEY,
                        CONSUMER_SECRET,
                        accesToken,
                        accesTokenSecret,
                        s_username,
                        s_password);

    TwitterBot bot(t);
    new TwitterBotAdaptor(&bot);


    QDBusConnection connection = QDBusConnection::sessionBus();
    bool rel = connection.registerService("org.rolisteam.twitterbot");
    rel = connection.registerObject("/",&bot);

    return app.exec();

}
