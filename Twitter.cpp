
/*-----------------------------------------------------------------------------------

accounts Skylar, Cassie, and Hope retweet
    shutupmikeginn
    DanMentos
    jonnysun
    BuckyIsotope
    Fred_delicious
    pixelatedboat //NEXT

human sheep accounts
    fuckeveryword
    teamfollowback

-----------------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------------
//									INCLUDES
//-----------------------------------------------------------------------------------

#include <iostream>
#include <getopt.h>

#include "Twitter.h"
#include "TwitterBot.h"

#include <QDebug>



Twitter::Twitter(
        std::string CK, std::string CS,
        std::string AT, std::string ATS,std::string userName,std::string passwd)
{
    char tmpBuf[1024];
    std::string replyMsg;
    std::string tmpStr, tmpStr2;

    this->m_acct.setTwitterUsername(userName);
    this->m_acct.setTwitterPassword(passwd);
    this->m_acct.getOAuth().setConsumerKey(CK);
    this->m_acct.getOAuth().setConsumerSecret(CS);

  //  this->acct.getOAuth().setOAuthTokenKey(AT);
  //  this->acct.getOAuth().setOAuthTokenSecret(ATS);

    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn;

    oAuthTokenKeyIn.open( "twitterClient_token_key.txt" );
    oAuthTokenSecretIn.open( "twitterClient_token_secret.txt" );

    memset( tmpBuf, 0, 1024 );
    oAuthTokenKeyIn >> tmpBuf;
    myOAuthAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    oAuthTokenSecretIn >> tmpBuf;
    myOAuthAccessTokenSecret = tmpBuf;

    oAuthTokenKeyIn.close();
    oAuthTokenSecretIn.close();
    if( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
        // If we already have these keys, then no need to go through auth again
        //printf( "\nUsing:\nKey: %s\nSecret: %s\n\n", myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str() );
        qDebug() << "twitterClient:: twitCurl::accountVerifyCredGet web response:\n" << myOAuthAccessTokenKey.c_str() <<myOAuthAccessTokenSecret.c_str() ;


        this->m_acct.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        this->m_acct.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    }
    else
    {//get url
        std::string str;
        this->m_acct.oAuthRequestToken(str);
        m_urlString = str;

       // bool b = this->acct.oAuthHandlePIN(m_urlString);
        qDebug() << QString::fromStdString(m_urlString);
        if( std::string::npos != tmpStr.find( "1" ) )
        {
            /* Ask user to visit twitter.com auth page and get PIN */
            memset( tmpBuf, 0, 1024 );
            qDebug() << "Please visit this link in web browser and authorize this application:\n" << m_urlString.c_str();
            qDebug() << "Enter the PIN provided by twitter: ";
            std::string name_surname;
            std::getline( std::cin, name_surname );
            tmpStr = tmpBuf;
            this->m_acct.getOAuth().setOAuthPin( tmpStr );
        }
        else
        {
            /* Else, pass auth url to twitCurl and get it via twitCurl PIN handling */
            this->m_acct.oAuthHandlePIN( m_urlString );
        }
        this->m_acct.oAuthAccessToken();
        //this->acct.oAuthAccessToken();

        std::ofstream oAuthTokenKeyOut;
        std::ofstream oAuthTokenSecretOut;

        this->m_acct.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
        this->m_acct.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

        oAuthTokenKeyOut.open( "twitterClient_token_key.txt" );
        oAuthTokenSecretOut.open( "twitterClient_token_secret.txt" );

        oAuthTokenKeyOut.clear();
        oAuthTokenSecretOut.clear();

        oAuthTokenKeyOut << myOAuthAccessTokenKey.c_str();
        oAuthTokenSecretOut << myOAuthAccessTokenSecret.c_str();

        oAuthTokenKeyOut.close();
        oAuthTokenSecretOut.close();
    }

    if( this->m_acct.accountVerifyCredGet() )
    {
        this->m_acct.getLastWebResponse( replyMsg );
        //qDebug() << "twitterClient:: twitCurl::accountVerifyCredGet web response:\n" << replyMsg.c_str();
    }
    else
    {
        this->m_acct.getLastCurlError( replyMsg );
        //qDebug() << "twitterClient:: twitCurl::accountVerifyCredGet  error:\n" << replyMsg.c_str();
    }

   /* if( acct.search( "%23roll", "10" ) )
    {
        acct.getLastWebResponse( replyMsg );
         qDebug() << "twitterClient:: twitCurl::search  reply:\n" << replyMsg.c_str();
    }
    else
    {
        acct.getLastCurlError( replyMsg );
         qDebug() << "twitterClient:: twitCurl::search  error:\n" << replyMsg.c_str();
    }*/
    //this->acct.getOAuth().setOAuthPin("8461952");
}

void Twitter::tweet(std::string & tweet,std::string & idResp)
{ //-------------------------------- tweet
    if(tweet.length() > 140)
    {
        std::cout << "Error: Tweet is "
                  << tweet.length() << " characters." << std::endl;
        return;
    }
    std::cout << "Tweeted: " << tweet << std::endl;
    m_acct.statusUpdate(tweet,idResp);
}
void Twitter::retwitteById(QString id)
{
    m_acct.retweetById(id.toStdString());
}

QString Twitter::getReceivedMessage(std::string & str)
{
    if(this->m_acct.directMessageGet(str))
    {
        std::string result;
        this->m_acct.getLastWebResponse(result);
        return QString::fromStdString(result);
    }
    return QString();
}
QString Twitter::search(QString str)
{

    std::string cppstr = str.toStdString();
    if(this->m_acct.search(cppstr))
    {
        std::string result;
        this->m_acct.getLastWebResponse(result);
        return QString::fromStdString(result);
    }
    return QString();
}
