#include "TwitterBot.h"
#include <QDebug>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QSettings>

#define TWITTER_LIMIT 140

//------------------------------------------------------------------ ctor
TwitterBot::TwitterBot(Twitter & t_in)
        : m_twitterManager(t_in),m_timer(new QTimer(this)),m_lastReceivedTwit("0"),m_init(false),m_init2(false)
{
    m_diceparser = new DiceParser();
    m_timer->setInterval(5000);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(searchTwit()));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(searchTwitJDR()));
    m_timer->start();
    readSettings();
}

void TwitterBot::readTwit()
{
    qDebug() << "readTwit" << m_result;
}
void TwitterBot::searchTwit()
{
    QString result2 = m_twitterManager.search("%23roll");
    if(!result2.isEmpty())
    {
        if(!m_result.contains(result2))
        {
            m_result << result2;
            filterRollMsg();
        }
    }

}
void TwitterBot::searchTwitJDR()
{
    QString result2 = m_twitterManager.search("%23JDR&lang=fr");
    if(!result2.isEmpty())
    {
        if(!m_resultJdr.contains(result2))
        {
            m_resultJdr << result2;
            retwitte();
        }
    }
}
void TwitterBot::retwitte()
{
    for(auto jsonFile : m_resultJdr)
    {
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.toLatin1());
        QJsonObject rootObj = doc.object();
        QJsonArray arrayStatues = rootObj["statuses"].toArray();

        for(int i = arrayStatues.count()-1; i>=0;--i)
        {
            QJsonValue msg = arrayStatues[i];
            QJsonObject msgObj = msg.toObject();
            QString idStr = msgObj["id_str"].toString();
            if(idStr>m_lastReceivedTwitJDR)
            {
                m_lastReceivedTwitJDR = idStr;
                if(m_init2)
                {
                    QString userId = rootObj["user"].toObject().value("id").toString();
                    qDebug() << "retwitte" << rootObj["text"].toString();
                    if(userId!="809467286599761920")
                    {
                        m_twitterManager.retwitteById(idStr);
                    }
                }
            }
        }

    }
    m_init2 = true;
}
#include <QCoreApplication>
void TwitterBot::quit()
{
    qApp->exit(0);
}
void TwitterBot::filterRollMsg()
{
    for(auto jsonFile : m_result)
    {
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.toLatin1());
        QJsonObject rootObj = doc.object();
        QJsonArray arrayStatues = rootObj["statuses"].toArray();
        //qDebug() << arrayStatues;

        for(int i = arrayStatues.count()-1; i>=0;--i)
        {
            QJsonValue msg = arrayStatues[i];
            QJsonObject msgObj = msg.toObject();
            qreal id = msgObj["id"].toDouble();
            QString idStr = msgObj["id_str"].toString();
            if(idStr>m_lastReceivedTwit)
            {
                QString val = msgObj["text"].toString();
                m_lastReceivedTwit = idStr;
                if((m_init)&&(val.startsWith("#roll")))
                {
                    val = val.remove(0,5);
                    val = val.trimmed();
                    CommandDice* cmd = new CommandDice();
                    qDebug() << val;
                    m_cmdToRun.append(cmd);
                    cmd->setCmd(val);
                    cmd->setIdMsg(idStr);
                    if(rollCmd(cmd))
                    {
                        qDebug() << "Roll cmd great!!!!";
                        sendTwittAnswer(cmd);
                    }
                    delete cmd;
                }
            }
        }

    }
    m_init = true;
}
bool TwitterBot::rollCmd(CommandDice* cmd)
{
    if(m_diceparser->parseLine(cmd->cmd()))
    {
        m_diceparser->Start();
        if(!m_diceparser->getErrorMap().isEmpty())
        {
            qDebug() << "Error" << m_diceparser->humanReadableError()<< "\n";
            return false;
        }

        ExportedDiceResult list;
        bool homogeneous = true;
        m_diceparser->getLastDiceResult(list,homogeneous);
        QString diceText = diceToText(list,true);
        QString scalarText;
        QString str;

        if(m_diceparser->hasIntegerResultNotInFirst())
        {
            scalarText = QString("%1").arg(m_diceparser->getLastIntegerResult());
        }
        else if(!list.isEmpty())
        {
            scalarText = QString("%1").arg(m_diceparser->getSumOfDiceResult());
        }
        str = QString("%1, details:[%3 (%2)]").arg(scalarText).arg(diceText).arg(m_diceparser->getDiceCommand());

        if(m_diceparser->hasStringResult())
        {
            str = m_diceparser->getStringResult();
        }
        cmd->setResult(str);
        return true;
    }
    return false;
}
QString TwitterBot::diceToText(ExportedDiceResult& dice,bool highlight)
{
    QStringList resultGlobal;
    foreach(int face, dice.keys())
    {
           QStringList result;
           ListDiceResult diceResult =  dice.value(face);
           //patternColor = patternColorarg();
           foreach (HighLightDice tmp, diceResult)
           {
                QStringList diceListStr;
                QStringList diceListChildren;


                for(int i =0; i < tmp.getResult().size(); ++i)
                {
                    qint64 dievalue = tmp.getResult()[i];
                    QString prefix("%1");


                    if(i==0)
                    {
                        diceListStr << prefix.arg(QString::number(dievalue));
                    }
                    else
                    {
                        diceListChildren << prefix.arg(QString::number(dievalue));
                    }
                }
                if(!diceListChildren.isEmpty())
                {
                    diceListStr << QString("[%1]").arg(diceListChildren.join(' '));
                }

                result << diceListStr.join(' ');
               // qDebug() << result << tmp.first << tmp.second;
           }

           if(dice.keys().size()>1)
           {
              resultGlobal << QString(" d%2:(%1)").arg(result.join(',')).arg(face);
           }
           else
           {
               resultGlobal << result;
           }
    }
    return resultGlobal.join(' ');
}
void TwitterBot::sendTwittAnswer(CommandDice* cmd)
{
    qDebug() << "sendAwnser";
    if(!cmd->result().isEmpty())
    {
        QString result = cmd->result();
        QString ads("%1, powered by @Rolisteam");
        ads=ads.arg(result);
        std::string adsCppString;
        std::string msgIdCppString;
        if(ads.size()<TWITTER_LIMIT)
        {
            adsCppString = ads.toStdString();
            msgIdCppString = cmd->idMsg().toStdString();
            m_twitterManager.tweet(adsCppString,msgIdCppString );
        }
        else
        {

             while(!ads.isEmpty())
             {
                 if(ads.size()<TWITTER_LIMIT)
                 {
                      adsCppString = ads.toStdString();
                      msgIdCppString = cmd->idMsg().toStdString();
                      m_twitterManager.tweet(adsCppString,  msgIdCppString);
                      ads = "";
                 }
                 else
                 {
                     adsCppString = ads.left(TWITTER_LIMIT).toStdString();
                     msgIdCppString = cmd->idMsg().toStdString();

                     m_twitterManager.tweet(adsCppString, msgIdCppString );
                     ads = ads.remove(0,TWITTER_LIMIT);
                 }
             }
        }
        saveSettings();
    }
}

QString CommandDice::idMsg() const
{
    return m_idMsg;
}

void CommandDice::setIdMsg(const QString &idMsg)
{
    m_idMsg = idMsg;
}

QString CommandDice::user() const
{
    return m_user;
}

void CommandDice::setUser(const QString &user)
{
    m_user = user;
}

QString CommandDice::cmd() const
{
    return m_cmd;
}

void CommandDice::setCmd(const QString &cmd)
{
    m_cmd = cmd;
}

QString CommandDice::result() const
{
    return m_result;
}

void CommandDice::setResult(const QString &result)
{
    m_result = result;
}
void TwitterBot::saveSettings()
{
    QSettings setting("rolisteam","DiceParserBot");

    setting.setValue("lastReceivedId",m_lastReceivedTwit);
}
void TwitterBot::readSettings()
{
    QSettings setting("rolisteam","DiceParserBot");

    m_lastReceivedTwit = setting.value("lastReceivedId",m_lastReceivedTwit).toString();
}
