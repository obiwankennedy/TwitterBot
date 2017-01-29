#ifndef TwitterBot_H
#define TwitterBot_H

#include "libtwitcurl/twitcurl.h"

#include "Twitter.h"

#include <iostream>
#include <cstdlib>

#include <vector>
#include <queue>
#include <set>

#include <ctime> 
#include <chrono>
#include <thread>

#include <QObject>
#include <QTimer>
#include <QEvent>

#include "diceparser.h"

//-----------------------------------------------------------------------------------
//									DECLARATIONS
//-----------------------------------------------------------------------------------
class CommandDice
{
public:
    QString idMsg() const;
    void setIdMsg(const QString &idMsg);

    QString user() const;
    void setUser(const QString &user);

    QString cmd() const;
    void setCmd(const QString &cmd);

    QString result() const;
    void setResult(const QString &result);


private:
    QString m_idMsg;
    QString m_user;
    QString m_cmd;
    QString m_result;
};

class TwitterBot : public QObject
{
    Q_OBJECT
public:
    TwitterBot(Twitter & t_in);

    void retwitte();
public slots:
    void searchTwitJDR();
protected:
    QString diceToText(ExportedDiceResult &dice, bool highlight);
    bool rollCmd(CommandDice* cmd);
    void saveSettings();
    void readSettings();
private slots:
    void readTwit();
    void searchTwit();
    void filterRollMsg();
    void sendTwittAnswer(CommandDice* cmd);

private:
	//Twitter
    Twitter & m_twitterManager;
    QTimer* m_timer;
    QString m_lastReceivedTwit;
    QString m_lastReceivedTwitJDR;
    qreal m_lastAnsweredTwit;

    QStringList m_result;
    QStringList m_resultJdr;
    DiceParser* m_diceparser;
    QList<CommandDice*> m_cmdToRun;
    QList<CommandDice*> m_results;
    bool m_init;
};
#endif
