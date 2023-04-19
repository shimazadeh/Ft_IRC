#include "../inc/Channel.hpp"

Channel::Channel(std::string name)
{
	_chname = name;
}
Channel::~Channel(){}

std::string Channel::get_name(){return (_chname);}

std::string &Channel::get_topic(){return (_topic);}

std::vector<User*>     Channel::get_members(){return (_members);}

int         Channel::size(){return (_members.size());}

void    Channel::add_member(User &user)
{
	_members.push_back(&user);
	user._channels.push_back(this);
}

void    Channel::add_ban(std::string _username)
{
	_ban.push_back(_username);
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

bool    Channel::erase_ban(std::string _username)
{
	for (iterator_string i = _ban.begin(); i < _ban.end(); i++)
	{
		if ((*i).compare(_username))
		{
			_ban.erase(i);
			return true;
		}
	}
	return false;
}

bool    Channel::is_member(std::string _username)
{
	for (size_t i = 0; i != _members.size(); i++)
	{
		if (!_members[i]->_nickname.compare(_username))
			return true;
	}
	return false;
}

bool    Channel::is_ban(std::string _username)
{
	for (iterator_string i = _ban.begin(); i != _ban.end(); i++)
	{
		if (!(*i).compare(_username))
			return true;
	}
	return false;
}

bool    Channel::is_oper(std::string _username)
{
	for (iterator_user i = _opers.begin(); i != _opers.end(); i++)
	{
		if (!(*i)->_nickname.compare(_username))
			return true;
	}
	return false;
}

void    Channel::send_message_all_members(std::string _msg)
{
for (iterator_user it = _members.begin(); it != _members.end(); it++)
	((*it)->_wbuff).append(_msg);
}

std::string Channel::print_members()
{
	std::string	res;
	std::vector<User*>	membs = get_members();

	for (int i = 0; i != membs.size() - 1; i++)
		res.append(membs[i]->_username + ", ");

	res.append(membs[size() - 1]->_username + "\n");

	return (res);
}
