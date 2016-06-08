
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

UserList::UserList(std::string & data_in, int limit)
	: data(data_in) { //------------------------------------------------------------- ctor
	
	size_t last = this->data.find('[') + 1;
	size_t end = this->data.find(']');
	
	for(size_t next = this->data.find(',', last); 
		limit > 0 && next < end && next != std::string::npos;
		next = this->data.find(',', last)){

		this->ids.push_back(this->data.substr(last, next - last));
		last = next + 1;
		--limit;
	} 
}

UserList::UserList(std::string & data_in)
	: data(data_in) { //------------------------------------------------------------- ctor
	
	std::cout << this->data << std::endl;
	
	size_t last = this->data.find('[') + 1;
	size_t end = this->data.find(']');
	
	for(size_t next = this->data.find(',', last); 
		next < end && next != std::string::npos;
		next = this->data.find(',', last)){

		this->ids.push_back(this->data.substr(last, next - last));
		last = next + 1;
	} 
}

std::string & UserList::operator[](int i){ //--------------------------------------- operator[]
	return ids[i];
}

std::vector<std::string> & UserList::list(){ //------------------------------------- list
	return this->ids;
}

unsigned UserList::size(){ //------------------------------------------------------- size
	return ids.size();
}


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

User::User(std::string data_in) //--------------------------------------------------- ctor
	: data(data_in) {
	
	std::string meta, info;
	int first, last;
	int past = this->data.find("target");
	
	meta = "screen_name\":\"";
	first = this->data.find(meta,past) + meta.length();
	last = this->data.find('"',first);
	this->ship.name = this->data.substr(first, last - first);
	
	meta = "following\":";
	first = this->data.find(meta,past) + meta.length();
	last = this->data.find(',',first);
	info = this->data.substr(first, last - first);
	this->ship.following = (info == "true") ? true : false;
	
	meta = "followed_by\":";
	first = this->data.find(meta,past) + meta.length();
	last = this->data.find(',',first);
	info = this->data.substr(first, last - first);
	this->ship.follower = (info == "true") ? true : false;
}

Friendship & User::get_friendship(){ //---------------------------------------------- get_friendship
	return this->ship;
}





