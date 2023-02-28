#ifndef _utils_hpp
#define _utils_hpp

#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <string>

class Users
{
	public:

	private:
		std::string	nickname;


};
typedef struct User_struct
{
	Users			_user;
	User_struct		*next;

}		User_struct_t;

class Channel
{
	public:

	private:
		User_struct		*list;

};


#endif
