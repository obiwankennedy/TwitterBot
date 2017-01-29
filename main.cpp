#include <QCoreApplication>

#include <QDebug>

#include "Twitter.h"
#include "TwitterBot.h"
#include "credential.h"

int main(int argc, char *argv[]){


    QCoreApplication app(argc,argv);

    std::ios_base::sync_with_stdio(false);
    qDebug() <<ACCESS_TOKEN  << ACCESS_TOKEN_SECRET ;
    Twitter t = Twitter(CONSUMER_KEY,
                        CONSUMER_SECRET,
                        accesToken,
                        accesTokenSecret,
                        s_username,
                        s_password);

    TwitterBot bot(t);


    return app.exec();

}
