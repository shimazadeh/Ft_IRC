#include "../inc/Channel.hpp"

Channel::Channel(std::string name)
{
	_chname = name;
	_topic = "";
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

bool    Channel::erase_user(User &user)//idk if all prototyppe has to  be changes to user*
{
	if (erase_members(user))
		return true;
	return false;
}

bool    Channel::erase_members(User &user)
{
	if (_members.size() == 0)
		return true;
	for (iterator_user i = _members.begin(); i != _members.end(); i++)
	{
		if (*i == &user)
		{
			_members.erase(i);
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

void    Channel::send_message_all_members(std::string _msg, std::string nickname)
{
	for (iterator_user it = _members.begin(); it != _members.end(); it++)
	{
		if (nickname != (*it)->_nickname)
			((*it)->_wbuff).append(_msg);
	}
}

std::string Channel::print_members()
{
	std::string	res;
	std::vector<User*>	membs = get_members();

	for (size_t i = 0; i != membs.size() - 1; i++)
		res.append(membs[i]->_nickname + " ");
	res.append(membs[size() - 1]->_nickname + "\n");
	return (res);
}
