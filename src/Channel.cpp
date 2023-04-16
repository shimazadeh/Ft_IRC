#include "../inc/Channel.hpp"

Channel::Channel(){}
Channel::~Channel(){}

std::string Channel::get_name(){return (_name);}

std::string &Channel::get_topic(){return (_topic);}

int         Channel::size(){return (_members.size());}

void    Channel::add_member(User &user)
{
	_members.push_back(&user);
}

void    Channel::add_ban(std::string name)
{
	_ban.push_back(name);
}

void    Channel::add_oper(User &user)
{
	_opers.push_back(&user);
}


bool    Channel::erase_user(User &user)//idk if all prototyppe has to  be changes to user*
{
	if (erase_members(user) || erase_opers(user) || erase_ban(user._nickname))
		return true;
	return false;
}

bool    Channel::erase_members(User &user)
{
for (iterator_user i = _members.begin(); i < _members.end(); i++)
{
	if (*i == &user)
	{
		_members.erase(i);
		return true;
	}
}
	return false;
}

bool    Channel::erase_opers(User &user)
{
for (iterator_user i = _opers.begin(); i < _opers.end(); i++)
{
	if (*i == &user)
	{
		_opers.erase(i);
		return true;
	}
}
	return false;
}

bool    Channel::erase_ban(std::string _name)
{
	for (iterator_string i = _ban.begin(); i < _ban.end(); i++)
	{
		if ((*i).compare(_name))
		{
			_ban.erase(i);
			return true;
		}
	}
	return false;
}

bool    Channel::is_member(std::string _name)
{
	for (iterator_user i = _members.begin(); i != _members.end(); i++)
	{
		if (!(*i)->_nickname.compare(_name))
			return true;
	}
	return false;
}

bool    Channel::is_ban(std::string _name)
{
	for (iterator_string i = _ban.begin(); i != _ban.end(); i++)
	{
		if (!(*i).compare(_name))
			return true;
	}
	return false;
}

bool    Channel::is_oper(std::string _name)
{
	for (iterator_user i = _opers.begin(); i != _opers.end(); i++)
	{
		if (!(*i)->_nickname.compare(_name))
			return true;
	}
	return false;
}

void    Channel::send_message_all_members(std::string _msg)
{
for (iterator_user it = _members.begin(); it != _members.end(); it++)
	((*it)->_wbuff).append(_msg);
}
