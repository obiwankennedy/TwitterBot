#include "TwitterBot.h"
#include <QDebug>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QSettings>
#include <QtDebug>
#include <QCoreApplication>

#define TWITTER_LIMIT 140

//------------------------------------------------------------------ ctor
TwitterBot::TwitterBot(Twitter & t_in)
        : m_twitterManager(t_in),m_timer(new QTimer(this)),m_lastReceivedTwit("0"),m_init(false),m_init2(false)
{
    m_diceparser = new DiceParser();

    m_diceparser->insertAlias(new DiceAlias("d[-1-1]","DF",true,true),0);

    m_timer->setInterval(2000*60);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(searchTwit()));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(searchTwitJDR()));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(checkMustSendRecordedMsg()));
    m_timer->start();
    readSettings();


    if(m_messagesToSend.isEmpty())
    {
        m_messagesToSend.insertMulti(0,"Lancer les dés sur twitter? facile: commencer vos msg par: #roll\n Suivi d'une commande. Plus d'info: http://rolisteam.org/fr/node/723");
        m_messagesToSend.insertMulti(0,"Suivez l'actualité du #JDR grâce à @DiceParser, un robot qui vous veut du bien");
        m_messagesToSend.insertMulti(0,"Je gère les commandes de dés comme dans @Rolisteam");
        m_messagesToSend.insertMulti(0,"Le blog de mon créateur: http://blog.rolisteam.org/ (Just in case)");
        m_messagesToSend.insertMulti(0,"@Rolisteam en action : https://www.youtube.com/watch?v=FdURVRDyu-Y&list=PLBSt0cCTFfS5fi3v1LtB9sfeA8opY-Ge1 ");
        m_messagesToSend.insertMulti(0,"Une fiche #L5R : Créer une copie https://docs.google.com/spreadsheets/d/1SjabaCDLnElG-dQt8zSdCD25hxT7lgN7MFsAiRubTyc/edit?usp=sharing ");
        m_messagesToSend.insertMulti(0,"Une fiche #Cops : Créer une copie https://docs.google.com/spreadsheets/d/12uTg0GtqPFKwQi8u5Lcn0cseelDj1HTEME60gorPR6A/edit?usp=sharing ");
        m_messagesToSend.insertMulti(0,"Si vous voulez voir mon code source, c'est ici: https://github.com/obiwankennedy/TwitterBot #Voyeurs");
    }
}

TwitterBot::~TwitterBot()
{
    delete m_diceparser;
}

void TwitterBot::readTwit()
{
    qInfo() << "Read Twit:" << m_result;
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
        qDebug() << "########" << m_resultJdr.contains(result2) ;
        if(!m_resultJdr.contains(result2))
        {
            m_resultJdr << result2;
            retwitte();
        }
    }
}
void TwitterBot::retwitte()
{

    qDebug() << m_resultJdr.size();
    for(auto jsonFile : m_resultJdr)
    {
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.toLatin1());
       // qDebug() << doc.toJson();
        QJsonObject rootObj = doc.object();
        QJsonArray arrayStatues = rootObj["statuses"].toArray();

        for(int i = arrayStatues.count()-1; i>=0;--i)
        {
            QJsonValue msg = arrayStatues[i];
            QJsonObject msgObj = msg.toObject();
            QString idStr = msgObj["id_str"].toString();
            //qDebug() << idStr << m_lastReceivedTwitJDR << "i=" <<i;
            if(idStr>m_lastReceivedTwitJDR)
            {
                m_lastReceivedTwitJDR = idStr;
                if(m_init2)
                {
                    QString userId = rootObj["user"].toObject().value("id").toString();
                    qInfo() << "retwit:" << rootObj["text"].toString() << userId;
                    if(!msgObj["text"].toString().contains("roll20"))
                    {
                        if(userId!="809467286599761920")
                        {
                            m_twitterManager.retwitteById(idStr);
                        }
                    }
                }
            }
        }

    }
    m_resultJdr.clear();
    m_init2 = true;
}
void TwitterBot::quit()
{
    qInfo() << "Application is about to close";
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
            QJsonObject user = msgObj["user"].toObject();
            if(idStr>m_lastReceivedTwit)
            {
                QString val = msgObj["text"].toString();
                m_lastReceivedTwit = idStr;
                if((m_init)&&(val.startsWith("#roll")))
                {
                    val = val.remove(0,5);
                    val = val.trimmed();
                    CommandDice* cmd = new CommandDice();
                    qInfo() << "Found Command Dice:" << val;
                    m_cmdToRun.append(cmd);
                    cmd->setCmd(val);
                    cmd->setIdMsg(idStr);
                    cmd->setUser(user["screen_name"].toString());
                    if(rollCmd(cmd))
                    {
                        qInfo() << "Valid Command" << val;
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
    if(!cmd->result().isEmpty())
    {
        QString result = cmd->result();
        QString ads;
        if(!cmd->user().isEmpty())
        {
            ads = "@%2 %1, powered by @Rolisteam";
        }
        else
        {
            ads = "%2%1, powered by @Rolisteam";
        }
        ads=ads.arg(result).arg(cmd->user());
        std::string adsCppString;
        std::string msgIdCppString;
        if(ads.size()<TWITTER_LIMIT)
        {
            adsCppString = ads.toStdString();
            msgIdCppString = cmd->idMsg().toStdString();
            m_twitterManager.tweet(adsCppString,msgIdCppString );
            qInfo() << "Whole Send Answer by Twit:" << QString::fromStdString(adsCppString) << "to:" << cmd->user();
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
                      qInfo() << "Send Answer by Twit split 1: " << QString::fromStdString(adsCppString);
                 }
                 else
                 {
                     adsCppString = ads.left(TWITTER_LIMIT).toStdString();
                     msgIdCppString = cmd->idMsg().toStdString();

                     m_twitterManager.tweet(adsCppString, msgIdCppString );
                     ads = ads.remove(0,TWITTER_LIMIT);
                     qInfo() << "Send Answer by Twit split 2: " << QString::fromStdString(adsCppString);
                 }
             }
        }
        saveSettings();
    }
}
void TwitterBot::sendTwit(QString msg)
{
    std::string adsCppString;
    std::string resp = "";
    if(msg.size()<TWITTER_LIMIT)
    {
        adsCppString = msg.toStdString();
        m_twitterManager.tweet(adsCppString,resp);
        qInfo() << "[slot] Send Twit:" << QString::fromStdString(adsCppString);
    }
}
#include <QDateTime>
#include <random>
void TwitterBot::checkMustSendRecordedMsg()
{
    QDateTime now = QDateTime::currentDateTime();
    if(m_previous.isValid())
    {
        if(now.time().hour()==15 && m_previous.time().hour()==14)
        {
            QStringList list = m_messagesToSend.values(0);
            if(list.isEmpty())
            {
                qDebug() << "Error! list empty";
            }
            else if(list.size()==1)
            {
                QString value = list.first();
                sendTwit(value);
                QStringList list2 = m_messagesToSend.values(1);
                for(QString str : list2)
                {
                    m_messagesToSend.insertMulti(0,str);
                }
                m_messagesToSend.remove(1);
            }
            else
            {
                std::random_device rd;
                std::uniform_int_distribution<int> dist(0, list.size()-1);
                int randomNumber = dist(rd);
                QString msg = list.at(randomNumber);
                list.removeOne(msg);
                sendTwit(msg);
                m_messagesToSend.insertMulti(1,msg);
                m_messagesToSend.remove(0);
                for(auto str : list)
                {
                    m_messagesToSend.insertMulti(0,str);
                }
            }
        }
    }
    m_previous = now;

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
