
#include "libtwitcurl/twitcurl.h"

#include "json.h"

#include <iostream>
#include <cstdlib>

#include <vector>
#include <queue>

#include <ctime>
#include <chrono>
#include <thread>

//-----------------------------------------------------------------------------------
//									DECLARATIONS
//-----------------------------------------------------------------------------------

struct Account { 

	Account(std::string id_in)
		: id(id_in) {}

	std::string id;
	std::string name;
	
	bool following;
	std::time_t follow;
	
	bool follower;
	std::time_t remove;
};

//UNUSED
/*
struct AccountComp { //sort so that oldest is on top
	bool operator()(Account & lhs, Account & rhs){
		return lhs.remove > rhs.remove;
	}
};
*/

class TwitterFollowBot {
public:
	TwitterFollowBot(std::string CK, std::string CS, std::string AT, std::string ATS);
	void loop();
	void end();

private:
	void get_accounts_to_follow();
	void add_new_follows();
	
	void sift();
	void manage();
	void move(int i);
	void delay();
	
	bool is_follower(Account & a);
	bool is_friend(Account & a);
	
	void unfollow(Account & a);
	void follow(Account & a);
	void tweet(std::string & tweet);
	
	twitCurl acct; //my account
	
	std::queue<Account> to_follow; //accounts to follow
	std::queue<Account> just_followed; //accounts just followed
	
	//UNUSED
	//std::priority_queue<Account, std::vector<Account>, AccountComp> to_unfollow; //to be unfollowed
	
	std::string ACCOUNTS[8] = { //accounts Skylar, Cassie, and Hope retweet
		"shutupmikeginn", //TODO
		"DanMentos", //TODO
		"jonnysun", //TODO
		"KeetPotato", 
		"dubstep4dads", 
		"BuckyIsotope", //TODO
		"Fred_delicious", //TODO
		"pixelatedboat" //TODO
	};
	
	const int FOLLOW_LIMIT = 2000;
	const int ADD_LIMIT = 5;
	const int REMOVE_LIMIT = 3;
	const int TIME = 16;
};

//-----------------------------------------------------------------------------------
//									DEFENITIONS
//-----------------------------------------------------------------------------------

TwitterFollowBot::TwitterFollowBot( 
	std::string CK, std::string CS, 
	std::string AT, std::string ATS){ //--------------------------------------------- ctor
	
	this->acct.getOAuth().setConsumerKey(CK);
	this->acct.getOAuth().setConsumerSecret(CS);
	
	this->acct.getOAuth().setOAuthTokenKey(AT);
	this->acct.getOAuth().setOAuthTokenSecret(ATS); 
	
	this->acct.oAuthAccessToken();
	
	std::cout << "Logged In\n";

}

void TwitterFollowBot::loop(){ //---------------------------------------------------- loop
	
	this->get_accounts_to_follow(); //initial setup
	
	//while I havent run into account limit && queue != empty
	while(!to_follow.empty()){ 
	
		//output
		std::cout << "List\tFollow\n";
		std::cout << to_follow.size() << "\t" 
			<< just_followed.size() << "\n";
		
		//sift for accounts followed hours ago
		this->sift();
		
		//unfollow as many as 3 accounts
		//this->manage();
		
		//follow 3 accounts
		this->add_new_follows();
		
		//wait 16 minutes
		this->delay();
	}
}

void TwitterFollowBot::delay(){
	std::cout << "============================================\n";
	const std::time_t future = std::time(nullptr)+this->TIME*60;
	std::cout << "          " << std::asctime(std::localtime(&future));
	std::cout << "============================================\n";
	std::this_thread::sleep_for(std::chrono::minutes(TIME));
	/*for(int i = 0; i < 16*4; ++i){
		std::cout << "=";
		std::this_thread::sleep_for(std::chrono::seconds(15));
	}
	std::cout << "\n";*/
}


void TwitterFollowBot::get_accounts_to_follow(){ //---------------------------------- get_acct_to_follow
	
	std::string response;
	std::string nextCursor("");
	std::string searchUser(ACCOUNTS[7]);

	if(acct.followersIdsGet(nextCursor, searchUser)){
		this->acct.getLastWebResponse(response);
		Followers f = Followers(response, this->FOLLOW_LIMIT);
		
		for(unsigned i = 0; i < f.size(); ++i){
			this->to_follow.push(f[i]);
		}
	}
	else exit(0);
}


void TwitterFollowBot::add_new_follows(){ //----------------------------------------- add_new_follows
	//add three more people
	for(int added = 0; added < this->ADD_LIMIT;){
		//if they have a relation to me
		if(is_friend(to_follow.front()))
			to_follow.pop();
		//else they dont
		else { 
			//follow them
			this->follow(to_follow.front());
			//add current timestamp to follow
			to_follow.front().follow = std::time(nullptr);
			//add them to just followed
			just_followed.push(to_follow.front());
			//remove them from to_follow
			to_follow.pop();
			//increment added
			++added;
		}
	}
}

void TwitterFollowBot::sift(){ //---------------------------------------------------- sift
	unsigned comp = this->ADD_LIMIT * 60/this->TIME * 18;
	if(just_followed.size() > comp){
		for(int i = 0; i < this->REMOVE_LIMIT; ++i){
			this->unfollow(just_followed.front());
			just_followed.pop();
		}
	}
	/*
	int checked = 0;
	//go through just_followed
	for(unsigned i = 0; i < just_followed.size(); ++i){
		//if they were followed a while ago (3 hours)
		if((double)((std::time(nullptr) - just_followed[i].follow) / (60*60*3)) >= 1){
			this->unfollow(just_followed[i]);
		}
		//if they follow me back
		else if(checked < 6 && is_follower(just_followed[i])){
			++checked;
			move(i);
		}
	}
	*/
}
/*
//UNUSED
void TwitterFollowBot::move(int i){ //----------------------------------------------- move
	//set a remove time
	just_followed[i].remove = std::time(nullptr);
	//add them to to_unfollow
	to_unfollow.push(just_followed[i]);
	//remove
	just_followed.erase(just_followed.begin() + i);
}

//UNUSED
void TwitterFollowBot::manage(){ //-------------------------------------------------- manage
	for(int i = 0; !to_unfollow.empty() && i < this->ADD_LIMIT; ++i){
		Account & a = const_cast<Account&>(to_unfollow.top());
		this->unfollow(a);
		to_unfollow.pop();
	}
}

//UNUSED
bool TwitterFollowBot::is_follower(Account & a){ //---------------------------------- is_follower
	std::string response;
	
	if(this->acct.friendshipShow(a.id,true)){
		this->acct.getLastWebResponse(response);
		User u = User(response);
		
		a.name = u.get_friendship().name;
		a.following = u.get_friendship().following;
		a.follower = u.get_friendship().follower;
	}
	else exit(0);
	
	std::cout << std::boolalpha 
		<< "Follow Status (" << a.name << "): " 
		<< a.follower << "\n";
	
	return a.follower;

}
*/
bool TwitterFollowBot::is_friend(Account & a){ //------------------------------------ is_friend
	std::string response;
	
	if(this->acct.friendshipShow(a.id,true)){
		this->acct.getLastWebResponse(response);
		User u = User(response);
		
		a.name = u.get_friendship().name;
		a.following = u.get_friendship().following;
		a.follower = u.get_friendship().follower;
	}
	else exit(0);
	
	std::cout << std::boolalpha 
		<< "Friend Status (" << a.name << "): " 
		<< (a.following || a.follower) << "\n";
	
	return a.following || a.follower;
}

void TwitterFollowBot::unfollow(Account & a){ //------------------------------------- unfollow
	std::cout << "Unfollowed: " << a.name << "\n";
	this->acct.friendshipDestroy(a.id, true);
}

void TwitterFollowBot::follow(Account & a){ //--------------------------------------- follow
	std::cout << "Followed: " << a.name << "\n";
	this->acct.friendshipCreate(a.id, true);
}

void TwitterFollowBot::tweet(std::string & tweet){ //-------------------------------- tweet
	this->acct.statusUpdate(tweet);
}


