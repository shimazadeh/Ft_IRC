#include "../inc/Tree.hpp"

Tree::Tree() {};
Tree::~Tree() {};

std::map<std::string, Channel>  &Tree::get_channel(){return (_chto_channel);}
std::map<std::string, User*>    &Tree::get_usernick(){return (_user_to_nick);}
std::map<int, User*>            &Tree::get_userfd(){return (_user_to_fd);}

void    Tree::insert_by_fd(int    fd)
{
	User    *user = new User("", fd);
	_user_to_fd.insert(std::make_pair(fd, user));
}

void Tree::insert_by_nick(std::string& nickname, User* user)
{
	_user_to_nick.insert(make_pair(nickname, user));
}

void    Tree::insert(const std::string chan_name)
{
	Channel chan(chan_name);

	_chto_channel.insert(std::make_pair(chan_name, chan));
}

User    *Tree::find_usr_by_nickname(std::string& nickname)
{
	if ((_user_to_nick.find(nickname) != _user_to_nick.end()))
		return((_user_to_nick.find(nickname))->second);
	return NULL;
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
	std::cout << _user_to_nick.size() << std::endl;
	std::cout << _user_to_fd.size() << std::endl;
	std::cout << "check2: " << user._fd << std::endl;
	_user_to_nick.erase(user._nickname);
	_user_to_fd.erase(user._fd);
	user.erase_me_from_allchannel();

	std::cout << _user_to_nick.size() << std::endl;
	std::cout << _user_to_fd.size() << std::endl;

	return ;
}

void    Tree::erase_channel(std::string   _name)
{
	_chto_channel.erase(_name);
}
