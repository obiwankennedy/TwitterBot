#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>

#include "Twitter.h"
#include "TwitterBot.h"
#include "credential.h"
#include "twitterbotadaptor.h"

enum LOG_STATE {DEBUG,INFO,WARNING,CRITICAL,FATAL};

int g_logMini = CRITICAL;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
      QByteArray localMsg = msg.toLocal8Bit();
      QString time = QDateTime::currentDateTime().toString("dd/mm/yyy - hh:mm:ss");
      switch (type)
      {
      case QtDebugMsg:
          fprintf(stderr, "Debug: [%s] - %s (%s:%u, %s)\n",time.constData(), localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtInfoMsg:
          fprintf(stderr, "Info: [%s] - %s (%s:%u, %s)\n",time.constData(), localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtWarningMsg:
          fprintf(stderr, "Warning: [%s] - %s (%s:%u, %s)\n",time.constData(), localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtCriticalMsg:
          fprintf(stderr, "Critical: [%s] - %s (%s:%u, %s)\n",time.constData(), localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtFatalMsg:
          fprintf(stderr, "Fatal: [%s] - %s (%s:%u, %s)\n",time.constData(), localMsg.constData(), context.file, context.line, context.function);
          abort();
      }
}


int main(int argc, char *argv[])
{

    qInstallMessageHandler(myMessageOutput);
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

    g_logMini = INFO;

    TwitterBot bot(t);
    new TwitterBotAdaptor(&bot);


    QDBusConnection connection = QDBusConnection::sessionBus();
    bool rel = connection.registerService("org.rolisteam.twitterbot");
    rel = connection.registerObject("/",&bot);

    return app.exec();

}
