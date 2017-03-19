#ifndef TWITTER_H
#define TWITTER_H

#include "json.h" 
#include "libtwitcurl/twitcurl.h"

#include <QString>

struct Account
{
	Account(std::string id_in) //creates Account with id
		: id(id_in) {} 

	std::string id;
	std::string name; //twitter handle
	
	bool following; //true when I follow them
	bool follower; //true when they follow me
};

class Twitter
{
public:
    Twitter(std::string CK, std::string CS, std::string AT, std::string ATS,std::string userName,std::string passwd);
    void tweet(std::string & tweet,std::string & idreply); //sends a tweet

    QString getReceivedMessage(std::string & sinceId);
    QString search(QString str);
    void retwitteById(QString id);
    QString getLimitRate();
private:
	//my account
    twitCurl m_acct; //updated in ctor
    std::string m_urlString;
};







#endif
