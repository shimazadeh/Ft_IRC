#ifndef _utils_hpp
#define _utils_hpp

#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <deque>
#include <list>
#include <map>
#include <vector>

class User
{
	public:

	private:
		std::string			nickname;
		std::string			user_name;
		std::string			real_name;
		std::list<Channel*>	_channels;
};

class Channel
{
	public:

	private:
		std::deque<User*>	_users;
		std::deque<User*>	_invites;
		std::deque<User*>	_blocked;


};

class Ban_User
{
	private:

	public:
		Ban_User(/* args */);
		~Ban_User();
};




#endif
