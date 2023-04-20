#include "../inc/Parser.hpp"

Parser::Parser(Tree& tree, std::string password)
{
	_tree = &tree;
	_cmd = "";
	_user = NULL;
	_pass = password;
}

Parser::~Parser()
{
}

void    Parser::change_user(User    *username)
{
	_user = username;
}

User    *Parser::get_user(){return (_user);}

int    Parser::check_for_cmd(bool& closecon, std::vector<struct pollfd>& _fds)
{
	std::string					buf;
	size_t						pos(0);

	while((pos = _user->_rbuff.find("\r\n")) != std::string::npos)
	{
		buf = (_user->_rbuff).substr(0, pos);
		_user->_rbuff = (_user->_rbuff).substr(pos + 2);
		fill_in_params(buf);
		execute(closecon, _fds);
	}
	return 0;
}

void    Parser::fill_in_params(std::string  buf)
{
	std::string::size_type prev_pos = 0, pos = 0;

	_param.clear();
	_cmd = "";
	while((pos = buf.find(32 , pos)) != std::string::npos || (pos = buf.find(9, pos)) != std::string::npos)
	{
		std::string substring( buf.substr(prev_pos, pos-prev_pos) );

		_param.push_back(substring);
		prev_pos = ++pos;
	}
	_param.push_back(buf.substr(prev_pos, pos-prev_pos));

	_cmd = *(_param.begin());
	_param.erase(_param.begin());
}

std::vector<std::string>    Parser::custom_split(std::string buf)
{
	std::string::size_type       prev_pos = 0, pos = 0;
	std::vector<std::string>     res;


	while((pos = buf.find(',' , pos)) != std::string::npos)
	{
		std::string substring( buf.substr(prev_pos, pos-prev_pos) );

		res.push_back(substring);
		prev_pos = ++pos;
	}
	res.push_back(buf.substr(prev_pos, pos-prev_pos));
	return (res);
}

void    Parser::execute(bool& closecon, std::vector<struct pollfd>& _fds)
{
	if (_cmd == "")
		return ;
	else if (_cmd.compare("CAP") == 0)
		cap();
	else if (_cmd.compare("PASS") == 0)
		pass();
	else if (_cmd.compare("QUIT") == 0)
	{
		if (quit())
			closecon = true;
	}
	else if (_cmd.compare("NICK") == 0)
		nick();
	else if (_cmd.compare("USER") == 0)
		user();
	else if (_cmd.compare("PING") == 0)
		ping();
	else if (_cmd.compare("OPER") == 0)
		oper();
	else if (_cmd.compare("PART") == 0)
		part();
	else if (_cmd.compare("TOPIC") == 0)
		topic();
	else if (_cmd.compare("INVITE") == 0)
		invite();
	else if (_cmd.compare("KICK") == 0)
		kick();
	else if (_cmd.compare("JOIN") == 0)
		join();
	else if (_cmd.compare("PRIVMSG") == 0)
		privmsg();
	else if (_cmd.compare("NOTICE") == 0)
		notice();
	else if (_cmd.compare("KILL") == 0)
		kill(_fds);
	else
		(_user->_wbuff).append("Error 421: No such command: " + _cmd + "\n");
}

void Parser::cap()
{
	return ;
}

void    Parser::pass()//1
{
	if (_user->_regstat > 1)
	{
		std::cout << "PASS\n" << std::endl;
		(_user->_wbuff).append(_user->_nickname + " :You may not reregister\n");
	}
	else if (_param.size() != 1)
	{
		(_user->_wbuff).append(_user->_nickname + " PASS :Not enough parameters\n");
	}
	else if (!(*(_param.begin())).compare(_pass))
	{
		_user->_regstat = 1;
	}
	else
	{
		_user->_regstat = 0;
		(_user->_wbuff).append(_user->_nickname + " :Password incorrect\n");
	}
}

void    Parser::nick()//2
{
	if (_user->_regstat == 0)
	{
		std::cout << "NICK\n" << std::endl;
		(_user->_wbuff).append(":You have not registered\n");
	}
	else if (_param.size() != 1)
	{
		(_user->_wbuff).append(_user->_nickname + " NICK :Not enough parameters\n");
	}
	else if (*(_param.begin()) == "")
	{
		(_user->_wbuff).append(":No nickname given\n");
	}
	else if (!_tree->get_usernick().empty() && _tree->get_usernick().find(*(_param.begin())) != _tree->get_usernick().end())
	{
		(_user->_wbuff).append(_user->_nickname + " :Nickname is already in use\n");
	}
	else if (!_tree->get_usernick().empty() && _tree->get_channel().find(*(_param.begin())) != _tree->get_channel().end())
	{
		(_user->_wbuff).append(_user->_nickname + " :Nickname is already in use\n");
	}	
	else
	{
		(_user->_wbuff).append("You changed your nickname to " + *(_param.begin()) + "\n");
		if (_user->_nickname != "")
			_tree->get_usernick().erase(_user->_nickname);
		std::cout << "map size: " << _tree->get_usernick().size() << ", user_fd: " << _tree->get_userfd().size() << std::endl;
		if (_user->_regstat != 3)
			_user->_regstat = 2;
		_user->_nickname = *(_param.begin());

		_tree->insert_by_nick(_user->_nickname, _user);
	}
}

void    Parser::user()
{
	if (_user->_regstat == 0 || _user->_regstat != 2)
	{
		std::cout << "USER\n" << std::endl;
		(_user->_wbuff).append(":You have not registered\n");
	}
	else if (_user->_regstat == 3)
	{
		(_user->_wbuff).append(_user->_nickname + " :You may not reregister\n");
	}
	else if (_param.size() != 4 || *(_param.begin()) == "" || *(_param.begin() + 3) == "")
	{
		(_user->_wbuff).append(_user->_nickname + " USER :Not enough parameters\n");
	}
	//else if (*(_param.begin() + 1) == "0" || *(_param.begin() + 2) == "*")
	//	(_user->_wbuff).append("USER: error: invalid parameters!\n");
	else
	{
		_user->_regstat = 3;
		_user->_username = *(_param.begin());
		for (size_t i = 3; i < _param.size(); i++)
			(_user->_realname).append(*(_param.begin() + i));
		(_user->_wbuff).append(":irssiserv 001 " + _user->_nickname + " :Welcome to the IRC Network, " + _user->_nickname + "!" + _user->_username + "@" + (*(_param.begin() + 2)) + "\n");
		(_user->_wbuff).append(_user->_nickname + " :Your host is irssiserv 001, running version 3\n");
		(_user->_wbuff).append(_user->_nickname + " irssiserv 001 1 0 0\n");

	}
}

void    Parser::ping()
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered\n");
	else if (_param.size() != 1 || *(_param.begin()) == "")
		(_user->_wbuff).append(_user->_nickname + " PING :Not enough parameters\n");
	else
		(_user->_wbuff).append("PONG " + *(_param.begin()) + "\n");
}

void    Parser::oper()//we choose to use our own password
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered\n");
	else if (_param.size() != 2)
		(_user->_wbuff).append(_user->_nickname + " OPER :Not enough parameters\n");
	else if (!(*(_param.begin())).compare(_pass))
		(_user->_wbuff).append(_user->_nickname + " :Password incorrect\n");
	// else if (!(*(_param.begin() + 1)).compare(_user->_username))
	// 	(_user->_wbuff).append("OPER: error: incorrect username!\n");
	else
	{
		if (_user->_nickname == (*(_param.begin() + 1)))
		{
			_user->_opstat = 1;
			(_user->_wbuff).append(":You are now an IRC operator\n");
		}
		// else
		// 	(_user->_wbuff).append("OPER: error: You can only OPER yourself\n");
	}
}

bool    Parser::quit()
{
	if (_param.size() < 1)
	{
		(_user->_wbuff).append(_user->_nickname + " QUIT :Not enough parameters\n");
		return (0);
	}
	else
	{
		std::vector<Channel*>   ch_tmp = _user->_channels;
		std::string		reason;

		std::cout << "IM HERE" << std::endl;
		for (size_t i = 0; i < _param.size() - 1; i++)
			reason.append(*(_param.begin() + i) + " ");
		reason.append(*(_param.begin() + _param.size() - 1) + "\n");
		_user->erase_me_from_allchannel(_user->_channels);
		_tree->erase_user(*_user);
		for (size_t i = 0; i < ch_tmp.size(); i++)
			ch_tmp[i]->send_message_all_members("QUIT: " + _user->_nickname + " has left the channel " + reason + "\n");
		return (1);
	}
}

void    Parser::part()
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered PART\n");
	else if (_param.size() < 2 || *(_param.begin()) == "" || *(_param.begin() + 1) == "")
		(_user->_wbuff).append(_user->_nickname + " PART :Not enough parameters\n");
	else
	{
		std::vector<std::string>    params = custom_split(*(_param.begin()));

		for (size_t i = 0; i < params.size(); i++)
		{
			std::map<std::string, Channel>::iterator    it = _tree->get_channel().find(*(params.begin() + i));

			if (it == _tree->get_channel().end())
				(_user->_wbuff).append((*(params.begin() + i)) + " :No such channel\n");
			else if (!(it->second).is_member(_user->_nickname))
				(_user->_wbuff).append((*(_param.begin() + i)) + " :You're not on that channel\n");
			else
			{
				std::string reason; 

				for (size_t i = 1; i < _param.size() - 1; i++)
					reason.append(*(_param.begin() + i) + " ");
				reason.append(*(_param.begin() + _param.size() - 1) + "\n");
				it->second.erase_members(*_user);
				it->second.send_message_all_members("PART: " + _user->_nickname + " has left the channel " + reason + "\n");
			}
			if (it->second.size() == 0)
				_tree->erase_channel(*(_param.begin()));
		}
	}
}

void    Parser::topic()
{
	std::map<std::string, Channel>::iterator it = _tree->get_channel().find(*(_param.begin()));

	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered\n");
	else if (_param.size() < 1 || _param.size() > 2 || *(_param.begin()) == "")
		(_user->_wbuff).append(_user->_nickname + " TOPIC :Not enough parameters\n");
	else if (!it->second.is_member(_user->_nickname))
		(_user->_wbuff).append((*(_param.begin())) + " :You're not on that channel\n");
	else if (it != _tree->get_channel().end())
	{
		if (*(_param.begin() + 1) == "")
		{
			if (it->second.get_topic() == "")
				(_user->_wbuff).append((*(_param.begin())) + " :No topic is set\n");
			else
				(_user->_wbuff).append("TOPIC: current topic of the channel is : " + it->second.get_topic() + "\n");
		}
		else if (_user->_opstat != 1)// change the condition to channel mode admin
			(_user->_wbuff).append((*(_param.begin())) + " :You're not channel operator\n");
		else
			it->second.send_message_all_members("TOPIC: the topic of the channel has been changed to " + (*(_param.begin() + 1)) + "\n");
		it->second.get_topic() = *(_param.begin() + 1);
	}
	else
		(_user->_wbuff).append((*(_param.begin())) + " :No such channel\n");

}

void    Parser::invite()
{
	std::map<std::string, Channel>::iterator it = _tree->get_channel().find(*(_param.begin() + 1));

	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered\n");
	else if (_param.size() != 2 || *(_param.begin()) == "" || *(_param.begin() + 1) == "")
		(_user->_wbuff).append(_user->_nickname + " INVITE :Not enough parameters\n");
	else if (_tree->get_usernick().find(*(_param.begin())) == _tree->get_usernick().end())
		(_user->_wbuff).append((*(_param.begin())) + " :No such nick/channel\n");
	else if (!it->second.is_member(_user->_nickname))
		(_user->_wbuff).append((*(_param.begin() + 1)) + " :You're not on that channel\n");
	else if (it != _tree->get_channel().end())
	{
		if (it->second.size() < 1)
			(_user->_wbuff).append("INVITE: error: empty channel\n");
		else if (it->second.is_member(*(_param.begin())))
			(_user->_wbuff).append("INVITE: error: the user is already in the channel !\n");
		else
		{
			it->second.add_member(*(_tree->find_usr_by_nickname(*(_param.begin()))));
			(_user->_wbuff).append(_user->_nickname + " has invited " + *(_param.begin()) + " to the channel: " + it->second.get_name());
		}
	}
	else
		(_user->_wbuff).append((*(_param.begin() + 1)) + " :No such channel\n");
}

void    Parser::kick()
{
	std::string channelName;
	std::string userToKick;
	std::map<std::string, Channel>::iterator it = _tree->get_channel().find((*_param.begin()));

	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered\n");
	else if (_param.size() != 2 || *(_param.begin()) == "" || *(_param.begin() + 1) == "")//limiting one user per kick our choise
		(_user->_wbuff).append(_user->_nickname + " KICK :Not enough parameters\n");
	else if (it == _tree->get_channel().end())
		(_user->_wbuff).append((*(_param.begin())) + " :No such channel\n");
	else if (!it->second.is_member(_user->_nickname))
		(_user->_wbuff).append((*(_param.begin())) + " :You're not on that channel\n");
	else if (_user->_nickname == (*(_param.begin() + 1)))
		(_user->_wbuff).append("KICK: error: you cannot kick yourself out. Use PART command\n");
	else if (it != _tree->get_channel().end())
	{
		// if (it->second.is_oper(*(_param.begin() + 1)))
		// 	(_user->_wbuff).append("KICK: error: User cannot be kicked because he is an operator!");
		if (it->second.is_member(*(_param.begin() + 1)))
		{
			it->second.erase_user(*(_tree->find_usr_by_nickname(*(_param.begin() + 1))));
			it->second.send_message_all_members(*(_param.begin() + 1) + " is kicked out of " + it->second.get_name() + "\n");
		}
		else
			(_user->_wbuff).append((*(_param.begin() + 1)) + " :No such nick/channel\n");
	}
}

void    Parser::join()
{
	std::map<std::string, Channel>::iterator it = _tree->get_channel().find(_param[0]);//multiple channel ?
	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered\n");
	else if (_param.size() < 1 || *(_param.begin()) == "")
		(_user->_wbuff).append(_user->_nickname + " JOIN :Not enough parameters\n");
	else 
	{
		if (it == _tree->get_channel().end())
		{
			_tree->insert(*(_param.begin()));
			it = _tree->get_channel().find(_param[0]);
			(_user->_wbuff).append("JOIN: you created the channel: " + *(_param.begin()) + "\n");
		}
		if (it->second.is_ban(_user->_nickname))
			(_user->_wbuff).append(*(_param.begin()) + ":Cannot join that channel\n");
		else if (it->second.is_member(_user->_nickname))
			(_user->_wbuff).append("JOIN: error: you're already in the channel : " + it->second.get_name() + "\n");
		else
		{
			it->second.add_member(*_user);
			(_user->_wbuff).append("JOIN: you've joined the channel: " + it->second.get_name()) + "\n";
			if (it->second.get_topic() != "")
				(_user->_wbuff).append("JOIN: topic of the channel is : " + it->second.get_topic()) + "\n";
			(_user->_wbuff).append(it->second.print_members());
		}
	}
}

void    Parser::privmsg()
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered\n");
	else if (_param.size() < 2)
		(_user->_wbuff).append(_user->_nickname + " PRIVMSG :Not enough parameters\n");
	else if ( *(_param.begin()) == "")
		(_user->_wbuff).append(":No recipient given (PRIVMSG)\n");
	else if (*(_param.begin() + 1) == "")
		(_user->_wbuff).append(":No text to send\n");
	else
	{
		std::vector<std::string>    targets = custom_split(*(_param.begin()));
		for (size_t i = 0; i < targets.size(); i++)
		{
			User* tmp =_tree->find_usr_by_nickname(*(targets.begin() + i));
			if (tmp)
				(tmp->_wbuff).append(_user->_nickname + ": " + *(_param.begin() + 1) + "\n");
			else
			{
				std::map<std::string, Channel>::iterator tmp2 = _tree->find_channel(*(targets.begin() + i));
				std::cout << tmp2->second.get_name() << std::endl;
				if (tmp2 == _tree->get_channel().end())
					(_user->_wbuff).append(*(_param.begin() + i) + " :No such nick/channel\n");
				else if (!tmp2->second.is_member(_user->_nickname))
					(_user->_wbuff).append(*(_param.begin() + 1) + " :You're not on that channel\n");
				else
				{
					(tmp2->second.send_message_all_members((*(_param.begin() + 1)) + "\n"));
				}
			}
		}
	}
}

void    Parser::notice()//dont really know the intention BUT we need to make it different from privmsg
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append(":You have not registered\n");
	else if (_param.size() < 2)
		(_user->_wbuff).append(_user->_nickname + " NOTICE :Not enough parameters\n");
	else if ( *(_param.begin()) == "")
		(_user->_wbuff).append(":No recipient given (NOTICE)\n");
	else if (*(_param.begin() + 1) == "")
		(_user->_wbuff).append(":No text to send\n");
	else
	{
		std::vector<std::string>    targets = custom_split(*(_param.begin()));
		for (size_t i = 0; i < targets.size(); i++)
		{
			User* tmp =_tree->find_usr_by_nickname(*(targets.begin() + i));
			if (tmp)
				(tmp->_wbuff).append(_user->_nickname + ": " + *(_param.begin() + 1) + "\n");
			else
			{
				std::map<std::string, Channel>::iterator tmp2 = _tree->find_channel(*(targets.begin() + i));
				if (tmp2 != _tree->get_channel().end() && tmp2->second.is_ban(_user->_nickname))
					(_user->_wbuff).append(*(_param.begin() + 1) + " :You're not on that channel\n");
				else if (tmp2 == _tree->get_channel().end())
					(_user->_wbuff).append(*(_param.begin() + i) + " :No such nick/channel\n");
				else
				{
					(tmp2->second.send_message_all_members((*(_param.begin() + 1)) + "\n"));
				}
			}
		}
	}
}

void   Parser::kill(std::vector<struct pollfd>& _fds)
{
	if (_param.size() != 2)
		(_user->_wbuff).append(_user->_nickname + " KILL :Not enough parameters\n");
	else if (*_param.begin() == "" || *(_param.begin() + 1) == "")
		(_user->_wbuff).append("KILL: error: empty parameters\n");
	else if (!_user->_opstat)
		(_user->_wbuff).append("KILL: error: you dont have operator privileges\n");
	else
	{
		int i = 0;
		while (_fds[i].fd != (_tree->find_usr_by_nickname(*(_param.begin()))->_fd))
			++i;
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i);
		_fds.erase(_fds.begin() + i - 1);
		std::vector<Channel*>   ch_tmp = _user->_channels;
		std::string		reason;

		_param.clear();
		_param.push_back("kill");
		reason.append(*(_param.begin()) + "\n");
		_user->erase_me_from_allchannel(_user->_channels);
		_tree->erase_user(*_user);
		for (size_t i = 0; i < ch_tmp.size(); i++)
			ch_tmp[i]->send_message_all_members("QUIT: " + _user->_nickname + " has left the channel " + reason + "\n");
	}
}


