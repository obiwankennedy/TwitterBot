#ifndef TWITTER_H
#define TWITTER_H

#include "json.h" 
#include "libtwitcurl/twitcurl.h"

struct Account { 
	Account(std::string id_in) //creates Account with id
		: id(id_in) {} 

	std::string id;
	std::string name; //twitter handle
	
	bool following; //true when I follow them
	bool follower; //true when they follow me
};

class Twitter {
public:
	Twitter(std::string CK, std::string CS, std::string AT, std::string ATS);
	void usage(); //checks the rate of usage
	void tweet(std::string & tweet); //sends a tweet
	bool is_follower(Account & a); //checks if user follows me
	bool is_friend(Account & a); //checks if user has relation to me
	void unfollow(Account & a); //unfollows a user
	void follow(Account & a); //follows a user

private:
	//my account
	twitCurl acct; //updated in ctor
};

Twitter::Twitter( 
	std::string CK, std::string CS, 
	std::string AT, std::string ATS){ //--------------------------------------------- ctor
	
	this->acct.getOAuth().setConsumerKey(CK);
	this->acct.getOAuth().setConsumerSecret(CS);
	
	this->acct.getOAuth().setOAuthTokenKey(AT);
	this->acct.getOAuth().setOAuthTokenSecret(ATS); 
	
	this->acct.oAuthAccessToken();
}

bool Twitter::is_follower(Account & a){ //---------------------------------- is_follower TODO
	std::string response;
	
	if(this->acct.friendshipShow(a.id,true)){
		this->acct.getLastWebResponse(response);
		User u = User(response);
		
		a.name = u.get_friendship().name;
		a.following = u.get_friendship().following;
		a.follower = u.get_friendship().follower;
	}
	else exit(0);
	
	return a.follower; //return if they follow me TODO
					   //must see what the JSON means
					   //and if I'm parsing the right data
}

bool Twitter::is_friend(Account & a){ //------------------------------------ is_friend
	std::string response;
	
	if(this->acct.friendshipShow(a.id,true)){
		this->acct.getLastWebResponse(response);
		User u = User(response);
		
		a.name = u.get_friendship().name;
		a.following = u.get_friendship().following;
		a.follower = u.get_friendship().follower;
	}
	else exit(0);
	
	if(a.following || a.follower)
		std::cout << "Passed: " << a.name << std::endl;
	
	return a.following || a.follower; //return if there is a relation
}

void Twitter::unfollow(Account & a){ //------------------------------------- unfollow
	if(this->is_follower(a)){
		std::cout << "Unfollowed [X]: " << a.name << std::endl;
	}
	else
		std::cout << "Unfollowed [ ]: " << a.name << std::endl;
	
	this->acct.friendshipDestroy(a.id, true);
}

void Twitter::follow(Account & a){ //--------------------------------------- follow
	std::cout << "Followed: " << a.name << std::endl;
	this->acct.friendshipCreate(a.id, true);
}

void Twitter::tweet(std::string & tweet){ //-------------------------------- tweet
	if(tweet.length() > 140){
		std::cout << "Error: Tweet is " 
			<< tweet.length() << " characters." << std::endl;
		return;
	}
	std::cout << "Tweeted: " << tweet << std::endl;
	this->acct.statusUpdate(tweet);
}

void Twitter::usage(){ //--------------------------------------------------- usage
	std::string response;
	if(this->acct.accountRateLimitGet()){
		this->acct.getLastWebResponse(response);
		std::cout << response << "\n";
	}
}

#endif
