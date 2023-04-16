#include "../inc/User.hpp"

User::User(int fd):_fd(fd){}
User::User(std::string nick_name, int fd): _nickname(nick_name), _fd(fd){}
User::~User(){}

void    User::erase_me_from_allchannel(std::vector<Channel*>  &_chan)
{
	for (size_t i = 0; i < _chan.size(); ++i)
		_chan[i]->erase_user(*this);
	_chan.clear();
}

void    User::erase_me_from_allchannel(void)
{
	for (size_t i = 0; i < _channels.size(); ++i)
		_channels[i]->erase_user(*this);
	_channels.clear();
}

std::string User::find_channel(std::string    chaname)
{
	for (std::vector<Channel*>::iterator i = _channels.begin(); i != _channels.end(); i++)
	{
		if (!((*i)->get_name().compare(chaname)))
			return ((*i)->get_name());
	}
	return "";
}

