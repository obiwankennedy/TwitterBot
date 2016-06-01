
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
	Account(std::string id_in) //creates Account with id
		: id(id_in) {} 

	std::string id;
	std::string name; //twitter handle
	
	bool following; //true when they follow me
	std::time_t follow; //timestamp of when they followed me (unused)
	
	bool follower; //true when I follow them 
	std::time_t remove; //timestamp of when I followed them (unused)
};

class TwitterFollowBot {
public:
	TwitterFollowBot(std::string CK, std::string CS, std::string AT, std::string ATS);
	void loop(std::string & user); //follow bot loop
	void usage(); //checks the rate of usage
	void tweet(std::string & tweet); //sends a tweet
	void record(); 
	void clean();
	
	//void end();

private:
	void get_accounts_to_follow(std::string & user); //creates to_follow array
	void add_follows(); //follow several people
	void remove_follows(); //remove several people

	void delay(int t); //print time of next update
	bool is_follower(Account & a); //checks if user follows me
	bool is_friend(Account & a); //checks if user has relation to me
	void unfollow(Account & a); //unfollows a user
	void follow(Account & a); //follows a user
	
	twitCurl acct; //my account
	
	std::queue<Account> to_follow; //accounts to follow
	std::queue<Account> just_followed; //accounts just followed
	
	const int FOLLOW_LIMIT = 2000;	//up to 5000 for team follow back
	const int ADD_LIMIT = 3; 		//people added per cycle
	const int REMOVE_LIMIT = 3;		//people removed per cycle
	const int TIME = 16; 			//minutes spent between cycle runs
	const int REMOVE_TIME = 14;		//hours spent following before remove
	
	int new_follows = 0; //counts users that have followed back at end of cycle
	int users_done = 0; //counts users gone through cycle
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
	
	std::cout << "Logged In" << std::endl;

}

//-----------------------------------------------------------------------------------
//									LOOP FUNCTIONS
//-----------------------------------------------------------------------------------

void TwitterFollowBot::loop(std::string & user){ //---------------------------------- loop
	
	this->get_accounts_to_follow(user); //initial setup
	
	//while I havent run into account limit && queues != empty
	while(!to_follow.empty() || !just_followed.empty()){ 
	
		//output
		std::cout << "List\tFollow" << std::endl;
		std::cout << to_follow.size() << "\t" 
			<< just_followed.size() << std::endl;
		
		//sift for accounts followed hours ago
		if(!just_followed.empty())
			this->remove_follows();
		
		//follow accounts
		if(!to_follow.empty())
			this->add_follows();
		
		if(this->users_done > 0)
			std::cout << "Success Rate: " 
				<< (double)(new_follows/users_done) << std::endl;
		
		//wait
		this->delay(this->TIME);
	}
}

void TwitterFollowBot::delay(int t){ //---------------------------------------------- delay
	std::cout << "============================================" << std::endl;
	const std::time_t future = std::time(nullptr) + t*60;
	std::cout << "          " << std::asctime(std::localtime(&future));
	std::cout << "============================================" << std::endl;
	std::this_thread::sleep_for(std::chrono::minutes(TIME));
}

void TwitterFollowBot::get_accounts_to_follow(std::string & user){ //---------------- get_acct_to_follow
	std::string response;
	std::string nextCursor("");
	std::string searchUser(user);
	//if request works
	if(acct.followersIdsGet(nextCursor, searchUser)){
		//get the last response
		this->acct.getLastWebResponse(response);
		//use JSON to make a list of followers
		UserList f = UserList(response, this->FOLLOW_LIMIT);
		//add them all to to_follow
		for(unsigned i = 0; i < f.size(); ++i){
			this->to_follow.push(f[i]);
		}
	}
	else exit(0);
	
	std::cout << "Added " << to_follow.size() 
		<< " accounts from user " << user << std::endl;
}

void TwitterFollowBot::add_follows(){ //--------------------------------------------- add_follows
	//add more people
	for(int added = 0; added < this->ADD_LIMIT && !to_follow.empty();){ 
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

void TwitterFollowBot::remove_follows(){ //------------------------------------------ remove_follows
	//after a certian number of follows and time
	unsigned comp = 100; //this->ADD_LIMIT * 60/this->TIME * this->REMOVE_TIME;
	//if it has exceeded that limit

	if(just_followed.size() > comp || to_follow.empty()){ 
		for(int i = 0; i < this->REMOVE_LIMIT; ++i){
			this->unfollow(just_followed.front());
			just_followed.pop();
		}
	}
}

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
	
	return a.following;
}

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
	
	if(a.following || a.follower)
		std::cout << "Passed: " << a.name << std::endl;
	
	return a.following || a.follower;
}

void TwitterFollowBot::unfollow(Account & a){ //------------------------------------- unfollow
	this->users_done += 1;
	if(this->is_follower(a)){
		this->new_follows += 1;
		std::cout << "Unfollowed [X]: " << a.name << std::endl;
	}
	else
		std::cout << "Unfollowed [ ]: " << a.name << std::endl;
	
	this->acct.friendshipDestroy(a.id, true);
}

void TwitterFollowBot::follow(Account & a){ //--------------------------------------- follow
	std::cout << "Followed: " << a.name << std::endl;
	this->acct.friendshipCreate(a.id, true);
}

//-----------------------------------------------------------------------------------
//							OTHER COMMAND LINE FUNCTIONS
//-----------------------------------------------------------------------------------

void TwitterFollowBot::tweet(std::string & tweet){ //-------------------------------- tweet
	if(tweet.length() > 140){
		std::cout << "Error: Tweet is " 
			<< tweet.length() << " characters." << std::endl;
		return;
	}
	std::cout << "Tweeted: " << tweet << std::endl;
	this->acct.statusUpdate(tweet);
}

void TwitterFollowBot::usage(){ //--------------------------------------------------- rate
	std::string response;
	if(this->acct.accountRateLimitGet()){
		this->acct.getLastWebResponse(response);
		std::cout << response << "\n";
	}
}

void TwitterFollowBot::record(){ //---------------------------- record
	std::string response;
	std::string nextCursor("");
	std::string searchUser("PeterJFlan");

	std::ofstream file;
	file.open("FOLLOWS.TXT");
	int size = 0;

	//if request works
	if(acct.friendsIdsGet(nextCursor, searchUser)){
		//get the last response
		this->acct.getLastWebResponse(response);
		//use JSON to make a list of who I follow
		UserList f = UserList(response, this->FOLLOW_LIMIT);
		//record them all to text file
		size = f.size();
		for (unsigned i = 0; i < f.size(); ++i){
			file << f[i] << std::endl;
		}
	}
	else exit(0);
	
	file.close();

	std::cout << "Recorded: " << size << std::endl;
}


void TwitterFollowBot::clean(){ //--------------------------------------------------- clean
	//read FOLLOW.TXT into unordered_map
	//use get_accounts_to_follow to get everyone I follow
	//while !to_follow.empty()
		//if !map.count(to_follow.front().id))
			//unfollow
		//to_follow.pop()
		
	//std::cout << "Unfollowed " << u << " accounts";
}

