#ifndef JSON_H
#define JSON_H
#include <vector>
#include <string>
#include <iostream>

//-----------------------------------------------------------------------------------
//									FOLLOW LIST
//-----------------------------------------------------------------------------------

class UserList {
public:
	UserList(std::string & data_in, int limit);
	UserList(std::string & data_in);
	std::string & operator[](int i);
	std::vector<std::string> & list();
	unsigned size();

private:
	std::string data;
	std::vector<std::string> ids;
	
	//useless
	int next_cursor = 0;
	std::string next_cursor_str = "0";
	int previous_cursor = 0;
	std::string previous_cursor_str = "0";
};



//-----------------------------------------------------------------------------------
//									USER DATA
//-----------------------------------------------------------------------------------

struct Friendship {
	std::string name;
	bool following;
	bool follower;
};

class User {
public:
    User(std::string data_in);
    Friendship & get_friendship();

private:
    std::string data;
    Friendship ship;
};



#endif


