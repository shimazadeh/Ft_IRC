#include "../inc/Tree.hpp"

Tree::Tree() {};
Tree::~Tree() {};

std::map<std::string, Channel>  Tree::get_channel(){return (_chto_channel);}
std::map<std::string, User*>    Tree::get_usernick(){return (_user_to_nick);}
std::map<int, User*>            Tree::get_userfd(){return (_user_to_fd);}

void    Tree::insert(const std::string &nick_name, int    &fd)
{
	User    *user = new User(nick_name, fd);
	_user_to_nick.insert(make_pair(nick_name, user));
	_user_to_fd.insert(std::make_pair(fd, user));
}

void    Tree::insert(const std::string chan_name)
{
	Channel chan;

	_chto_channel.insert(std::make_pair(chan_name, chan));
}

User    *Tree::find_usr_by_nickname(std::string& nickname)
{
	return((_user_to_nick.find(nickname))->second);
}

User    *Tree::find_usr_by_fd(int& fd)
{
	return(_user_to_fd.find(fd)->second);
}

std::map<std::string, Channel>::iterator Tree::find_channel(std::string& channel)
{
	return((_chto_channel.find(channel)));
}

void    Tree::erase_user(User &user)
{
	_user_to_nick.erase(user._nickname);
	_user_to_fd.erase(user._fd);
	user.erase_me_from_allchannel();

	return ;
}

void    Tree::erase_channel(std::string   _name)
{
	_chto_channel.erase(_name);
}
