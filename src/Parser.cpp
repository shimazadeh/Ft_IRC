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

int    Parser::check_for_cmd(int c, std::vector<bool>& closecon, std::vector<struct pollfd>& _fds)
{
	std::string					buf;
	size_t						pos(0);

	while((pos = _user->_rbuff.find("\n")) != std::string::npos)
	{
		buf = (_user->_rbuff).substr(0, pos);
		if (buf.find("\r") != std::string::npos)
		{
			buf.replace(buf.find("\r"), 1, "");
			_user->_rbuff = (_user->_rbuff).substr(pos);
		}
		else
			_user->_rbuff = (_user->_rbuff).substr(pos + 1);
		fill_in_params(buf);
		execute(c, closecon, _fds);
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

void    Parser::execute(int c, std::vector<bool>& closecon, std::vector<struct pollfd>& _fds)
{
	if (_cmd == "")
		return ;
	else if (_cmd.compare("CAP") == 0 || _cmd.compare("cap") == 0)
		cap();
	else if (_cmd.compare("PASS") == 0 || _cmd.compare("pass") == 0)
		pass();
	else if (_cmd.compare("QUIT") == 0 || _cmd.compare("quit") == 0)
	{
		if (quit())
		{
			if (!(c % 2))
				--c;
			closecon[c] = true;
			closecon[c + 1] = true;
		}
	}
	else if (_cmd.compare("NICK") == 0 || _cmd.compare("nick") == 0)
		nick();
	else if (_cmd.compare("USER") == 0 || _cmd.compare("user") == 0)
		user();
	else if (_cmd.compare("PING") == 0 || _cmd.compare("ping") == 0)
		ping();
	else if (_cmd.compare("MODE") == 0 || _cmd.compare("mode") == 0)
		mode();
	else if (_cmd.compare("WHO") == 0 || _cmd.compare("who") == 0)
		who();
	else if (_cmd.compare("OPER") == 0 || _cmd.compare("oper") == 0)
		oper();
	else if (_cmd.compare("PART") == 0 || _cmd.compare("part") == 0)
		part();
	else if (_cmd.compare("TOPIC") == 0 || _cmd.compare("topic") == 0)
		topic();
	else if (_cmd.compare("NAMES") == 0 || _cmd.compare("names") == 0)
		names();
	else if (_cmd.compare("KICK") == 0 || _cmd.compare("kick") == 0)
		kick();
	else if (_cmd.compare("JOIN") == 0 || _cmd.compare("join") == 0)
		join();
	else if (_cmd.compare("PRIVMSG") == 0 || _cmd.compare("privmsg") == 0)
		privmsg();
	else if (_cmd.compare("NOTICE") == 0 || _cmd.compare("notice") == 0)
		notice();
	else if (_cmd.compare("KILL") == 0 || _cmd.compare("kill") == 0)
		kill(closecon, _fds);
	else
		(_user->_wbuff).append("421 " + _user->_nickname + " " + _cmd + " : Unknown command\n");
}

void Parser::cap()
{
	return ;
}

void    Parser::pass()
{
	if (_user->_regstat > 1)
		(_user->_wbuff).append("462 " + _user->_nickname + " :You may not reregister\n");
	else if (_param.size() < 1)
		(_user->_wbuff).append("461 " + _user->_nickname + " PASS :Not enough parameters\n");
	else if (!(*(_param.begin())).compare(_pass))
		_user->_regstat = 1;
	else
	{
		_user->_regstat = 0;
		(_user->_wbuff).append("464 " + _user->_nickname + " :Password incorrect\n");
	}
}

void    Parser::nick()//2
{
	if (_user->_regstat == 0)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 1)
		(_user->_wbuff).append("461 " + _user->_nickname + " NICK :Not enough parameters\n");
	else if (*(_param.begin()) == "")
		(_user->_wbuff).append("431 " + _user->_nickname + " :No nickname given\n");
	else if (!_tree->get_usernick().empty() && _tree->get_usernick().find(*(_param.begin())) != _tree->get_usernick().end())
		(_user->_wbuff).append("433 " + _user->_nickname + " :Nickname is already in use\n");
	else if (!_tree->get_usernick().empty() && _tree->get_channel().find(*(_param.begin())) != _tree->get_channel().end())
		(_user->_wbuff).append("433 " + _user->_nickname + " :Nickname is already in use\n");
	else
	{
		(_user->_wbuff).append(":" + _user->_nickname + " NICK " + *(_param.begin()) + "\n");
		if (_user->_nickname != "")
			_tree->get_usernick().erase(_user->_nickname);
		if (_user->_regstat != 3)
			_user->_regstat = 2;
		_user->_nickname = *(_param.begin());
		_tree->insert_by_nick(_user->_nickname, _user);
	}
}

void    Parser::user()
{
	char	nowtime[64];
	time_t	rawtime;

	time(&rawtime);
	strftime(nowtime, 64, "%Y/%m/%d %H:%M:%S", localtime(&rawtime));

	if (_user->_regstat == 3)
		(_user->_wbuff).append("462 " + _user->_nickname + " :You may not reregister\n");
	else if(_user->_regstat == 0 || _user->_regstat != 2)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 4 || *(_param.begin()) == "" || *(_param.begin() + 3) == "")
		(_user->_wbuff).append("461 " + _user->_nickname + " USER :Not enough parameters\n");
	else
	{
		_user->_regstat = 3;
		_user->_username = *(_param.begin());
		for (size_t i = 3; i < _param.size(); i++)
			(_user->_realname).append(*(_param.begin() + i));
		(_user->_wbuff).append("001 " + _user->_nickname + " :Welcome to the IRC Network, " + _user->_nickname + "\n");
		(_user->_wbuff).append("002 " + _user->_nickname + " :Your host is irssiserv, running version 1.0.\n");
		(_user->_wbuff).append("003 " + _user->_nickname + " :This server was created " + nowtime + "\n");
		(_user->_wbuff).append("004 " + _user->_nickname + " :irssiserv 1.0 o." + "\n");
	}
}

void    Parser::ping()
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append("462 " + _user->_nickname + " :You may not reregister\n");
	else if (_param.size() < 1 || *(_param.begin()) == "")
		(_user->_wbuff).append("461 " + _user->_nickname + " PING :Not enough parameters\n");
	else
		(_user->_wbuff).append("PONG " + *(_param.begin()) + "\n");
}

void    Parser::oper()//we choose to use our own password
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 2 || *(_param.begin()) == "" || *(_param.begin() + 1) == "")
		(_user->_wbuff).append("461 " + _user->_nickname + " OPER :Not enough parameters\n");
	else if ((*(_param.begin() + 1)).compare(_pass))
		(_user->_wbuff).append("464 " + _user->_nickname + " OPER :Password incorrect\n");
	else if ((*(_param.begin())).compare(_user->_nickname))
		(_user->_wbuff).append("OPER: error: You can only OPER yourself\n");
	else
	{
		_user->_opstat = 1;
		(_user->_wbuff).append("381 " + _user->_nickname + " :You are now an IRC operator\n");
	}
}

bool    Parser::quit()
{
	std::vector<Channel*>   ch_tmp = _user->_channels;
	std::string		reason = "";

	if (_param.size() != 0)
	{
		for (size_t i = 0; i < _param.size() - 1; i++)
			reason.append(*(_param.begin() + i) + " ");
		reason.append(*(_param.begin() + _param.size() - 1));
	}
	for (size_t i = 0; i < ch_tmp.size(); i++)
		ch_tmp[i]->send_message_all_members(":" + _user->_nickname + "!" + _user->_username + "@localhost QUIT " + _user->_nickname + " :has left the channel " + reason + "\n", "");
	_user->erase_me_from_allchannel(_user->_channels);
	return (1);
}

void	Parser::who()
{
	return ;
}


void    Parser::part()
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 1 || *(_param.begin()) == "")
		(_user->_wbuff).append("461 " + _user->_nickname + " PART :Not enough parameters\n");
	else
	{
		std::vector<std::string>    params = custom_split(*(_param.begin()));

		for (size_t i = 0; i < params.size(); i++)
		{
			std::map<std::string, Channel>::iterator    it = _tree->get_channel().find(*(params.begin() + i));

			if (it == _tree->get_channel().end())
				(_user->_wbuff).append("403 " + _user->_nickname + " " + (*(params.begin() + i)) + " :No such channel\n");
			else if (!(it->second).is_member(_user->_nickname))
				(_user->_wbuff).append("442 " + _user->_nickname + " " + (*(params.begin() + i)) + " :You're not on that channel\n");
			else
			{
				std::string reason;

				if (_param.size() > 1)
				{
					for (size_t i = 1; i < _param.size() - 1; i++)
						reason.append(*(_param.begin() + i) + " ");
					reason.append(*(_param.begin() + _param.size() - 1) + "\n");
				}
				it->second.send_message_all_members(":" + _user->_nickname + "!" + _user->_username + "@localhost PART " + it->second.get_name() + " :has left the channel: " + reason + "\n", "");
				it->second.erase_members(*_user);
				for (std::vector<Channel*>::iterator i = (_user->_channels).begin(); i != (_user->_channels).end(); i++)
				{
					if (!(((*i)->get_name()).compare(*(_param.begin()))))
					{
						_user->_channels.erase(i);
						break;
					}
				}
				if (it->second.size() == 0)
					_tree->erase_channel(*(params.begin() + i));
			}
		}
	}
}

void    Parser::topic()
{
	std::map<std::string, Channel>::iterator it = _tree->get_channel().find(*(_param.begin()));

	if (_user->_regstat != 3)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 1 || *(_param.begin()) == "")
		(_user->_wbuff).append("461 " + _user->_nickname + " TOPIC :Not enough parameters\n");
	else if (it != _tree->get_channel().end() && !it->second.is_member(_user->_nickname))
		(_user->_wbuff).append("442 " + _user->_nickname + " " + (*(_param.begin())) + " :You're not on that channel\n");
	else if (it != _tree->get_channel().end())
	{
		if (*(_param.begin() + 1) != "")
		{
			it->second.send_message_all_members("332 " + _user->_nickname + " " + (*(_param.begin())) + " :" + (*(_param.begin() + 1)) + "\n", "");
			it->second.get_topic() = *(_param.begin() + 1);
		}
		else
		{
			if (it->second.get_topic() == "")
				(_user->_wbuff).append("331 " + _user->_nickname + " " + (*(_param.begin())) + " :No topic is set\n");
			else
			{
				(_user->_wbuff).append("332 " + _user->_nickname + " " + (*(_param.begin())) + " :" + (*(_param.begin() + 1)) + "\n");
				it->second.get_topic() = *(_param.begin() + 1);
			}
		}
	}
	else
		(_user->_wbuff).append("403 " + _user->_nickname + " " + (*(_param.begin())) + " :No such channel\n");

}

void    Parser::kick()
{
	std::string channelName;
	std::string userToKick;
	std::map<std::string, Channel>::iterator it = _tree->get_channel().find((*_param.begin()));

	if (_user->_regstat != 3)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 2  || *(_param.begin()) == "" || *(_param.begin() + 1) == "")//limiting one user per kick our choise
		(_user->_wbuff).append("461 " + _user->_nickname + " KICK :Not enough parameters\n");
	else if (it == _tree->get_channel().end())
		(_user->_wbuff).append("403 " + _user->_nickname + " " + (*(_param.begin())) + " :No such channel\n");
	else if (!_user->_opstat)
		(_user->_wbuff).append("481 " + _user->_nickname + " :Permission Denied - You're not an IRC operator\n");
	else if (!(it->second.is_member(*(_param.begin() + 1))))
		(_user->_wbuff).append("441 " + _user->_nickname + " " + (*(_param.begin() + 1)) + " " + it->second.get_name() + " :They aren't on that channel\n");
	else if (it != _tree->get_channel().end())
	{
		std::string	comment;

		if (_param.size() > 2)
		{
			for (size_t i = 2; i < _param.size() - 1; i++)
				comment.append(*(_param.begin() + i) + " ");
			comment.append(*(_param.begin() + _param.size() - 1));
		}

		if (it->second.is_member(*(_param.begin() + 1)))
		{
			it->second.send_message_all_members(":" + _user->_nickname + "!" + _user->_username + "@localhost KICK " + (*(_param.begin())) + " " + (*(_param.begin() + 1)) + " " + comment + "\n", "");
			it->second.erase_members(*(_tree->find_usr_by_nickname(*(_param.begin() + 1))));
			if (it->second.size() == 0)
				_tree->erase_channel(*(_param.begin()));
		}
		else
			(_user->_wbuff).append("401 " + _user->_nickname + " " + (*(_param.begin() + 1)) + " :No such nick/channel\n");
	}
}

void	Parser::mode()
{
	std::map<std::string, User*>::iterator		it_n = _tree->_user_to_nick.find(*(_param.begin()));
	std::map<std::string, Channel>::iterator	it_c = _tree->_chto_channel.find(*(_param.begin()));

	if (it_n == _tree->_user_to_nick.end() && it_c == _tree->_chto_channel.end())
		(_user->_wbuff).append("401 " + _user->_nickname + " " + (*(_param.begin())) + " :No such nick/channel\n");
	else if (it_n !=_tree->_user_to_nick.end())
	{
		if (!_user->_username.compare(*(_param.begin())))
			(_user->_wbuff).append("502 " + _user->_nickname + " :Cant change mode for other users\n");
		else
		{
			(_user->_wbuff).append("221 " + _user->_nickname + " ");
			if (_user->_opstat)
				(_user->_wbuff).append("o");
			(_user->_wbuff).append("\n");
		}
	}
	else
		(_user->_wbuff).append("324 " + _user->_username + " " + it_c->second.get_name() + "\n");
}

void    Parser::join()
{
	std::vector<std::string>		channels = custom_split(*(_param.begin()));

	if (_user->_regstat != 3)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 1 || *(_param.begin()) == "")
		(_user->_wbuff).append("461 " + _user->_nickname + " JOIN :Not enough parameters\n");
	else
	{
		for (size_t i = 0; i != channels.size(); i++)
		{
			std::map<std::string, Channel>::iterator it = _tree->get_channel().find(*(channels.begin()+ i));
			std::string								 msg;

			if (it != _tree->get_channel().end() && it->second.is_member(_user->_nickname))
				return ;
			else
			{
				if (it == _tree->get_channel().end())
				{
					_tree->insert(*(channels.begin() + i));
					it = _tree->get_channel().find(*(channels.begin() + i));
				}
				it->second.add_member(*_user);
				msg.append(":" + _user->_nickname + "!" + _user->_username + "@localhost JOIN " + it->second.get_name() + "\n");
				msg.append(":" + _user->_nickname + "!" + _user->_username + "@localhost 332 " + _user->_nickname + " " + it->second.get_name() + " :" + it->second.get_topic() + "\n");
				msg.append(":" + _user->_nickname + "!" + _user->_username + "@localhost 353 "  + _user->_nickname + " = " +  it->second.get_name()  + " : " + it->second.print_members());
				msg.append(":" + _user->_nickname + "!" + _user->_username + "@localhost 366 "  + _user->_nickname + " " +  it->second.get_name()  + " :End of /NAMES list\n");
				it->second.send_message_all_members(msg, "");
			}
		}
	}
}

void    Parser::privmsg()
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 2)
		(_user->_wbuff).append("461 " + _user->_nickname + " PRIVMSG :Not enough parameters\n");
	else if ( *(_param.begin()) == "")
		(_user->_wbuff).append("441 " + _user->_nickname + " :No recipient given (PRIVMSG)\n");
	else if (*(_param.begin() + 1) == "")
		(_user->_wbuff).append("412 " + _user->_nickname + " :No text to send\n");
	else
	{
		std::vector<std::string>    targets = custom_split(*(_param.begin()));

		for (size_t i = 0; i < targets.size(); i++)
		{
			User* tmp = _tree->find_usr_by_nickname(*(targets.begin() + i));
			std::string		msg;

			for (size_t i = 1; i != _param.size() - 1; i++)
				msg.append(*(_param.begin() + i) + " ");
			msg.append(*(_param.begin() + _param.size() - 1));

			if (tmp)
				(tmp->_wbuff).append(_user->_nickname + ": " + msg + "\n");
			else
			{
				std::map<std::string, Channel>::iterator tmp2 = _tree->find_channel(*(targets.begin() + i));

				if (tmp2 == _tree->get_channel().end())
					(_user->_wbuff).append("401 " + _user->_nickname + " " + (*(_param.begin())) + " :No such nick/channel\n");
				else if (!tmp2->second.is_member(_user->_nickname))
					(_user->_wbuff).append("442 " + _user->_nickname + " " + (*(_param.begin())) + " :You're not on that channel\n");
				else
					tmp2->second.send_message_all_members(":" + _user->_nickname + " PRIVMSG " + (*(_param.begin())) + " " + msg + "\n", _user->_nickname);
			}
		}
	}
}

void    Parser::notice()
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 2)
		(_user->_wbuff).append("461 " + _user->_nickname + " NOTICE :Not enough parameters\n");
	else if ( *(_param.begin()) == "")
		(_user->_wbuff).append("441 " + _user->_nickname + " :No recipient given (NOTICE)\n");
	else if (*(_param.begin() + 1) == "")
		(_user->_wbuff).append("412 " + _user->_nickname + " :No text to send\n");
	else
	{
		std::vector<std::string>    targets = custom_split(*(_param.begin()));
		for (size_t i = 0; i < targets.size(); i++)
		{
			User* tmp =_tree->find_usr_by_nickname(*(targets.begin() + i));
			std::string		msg;

			for (size_t i = 1; i != _param.size() - 1; i++)
				msg.append(*(_param.begin() + i) + " ");
			msg.append(*(_param.begin() + _param.size() - 1));

			if (tmp)
				(tmp->_wbuff).append(_user->_nickname + ": " + msg + "\n");
			else
			{
				std::map<std::string, Channel>::iterator tmp2 = _tree->find_channel(*(targets.begin() + i));

				if (tmp2 == _tree->get_channel().end())
					(_user->_wbuff).append("401 " + _user->_nickname + " " + (*(targets.begin() + i)) + " :No such nick/channel\n");
				else if (!tmp2->second.is_member(_user->_nickname))
					(_user->_wbuff).append("442 " + _user->_nickname + " " + (*(targets.begin() + i)) + " :You're not on that channel\n");
				else
				{
					tmp2->second.send_message_all_members(":" + _user->_nickname + " NOTICE " + (*(_param.begin())) + " " + msg + "\n", _user->_nickname);
					(_user->_wbuff).append(":" + _user->_nickname + " NOTICE " + (*(targets.begin() + i)) + " :" + msg + "\n");
				}
			}
		}
	}
}

void	Parser::names()
{
	if (_user->_regstat != 3)
		(_user->_wbuff).append("451 " + _user->_nickname + " :You have not registered\n");
	else if (_param.size() < 1 || *(_param.begin()) == "")
		(_user->_wbuff).append("461 " + _user->_nickname + " NAMES :Not enough parameters\n");
	else
	{
		std::vector<std::string>    targets = custom_split(*(_param.begin()));
		std::string					msg;

		for (size_t i = 0; i != targets.size(); i++)
		{
			std::map<std::string, Channel>::iterator tmp2 = _tree->find_channel(*(targets.begin() + i));

			if (tmp2 == _tree->get_channel().end())
				(_user->_wbuff).append("401 " + _user->_nickname + " " + (*(targets.begin() + i)) + " :No such nick/channel\n");
			else
			{
				msg.append(tmp2->second.print_members());
				tmp2->second.send_message_all_members("353 " + _user->_nickname + " = " + tmp2->second.get_name() + " " + msg + "\n", "");
				tmp2->second.send_message_all_members("366 " + _user->_nickname + " " + tmp2->second.get_name() + " :End of /NAMES list\n", "");
			}
		}
	}
}

void   Parser::kill(std::vector<bool>& closecon, std::vector<struct pollfd>& _fds)
{
	if (_param.size() < 2 || *_param.begin() == "" || *(_param.begin() + 1) == "")
		(_user->_wbuff).append("461 " + _user->_nickname + " KILL :Not enough parameters\n");
	else if (!_user->_opstat)
		(_user->_wbuff).append("481 " + _user->_nickname + " :Permission Denied - You're not an IRC operator\n");
	else if (!_tree->find_usr_by_nickname(*(_param.begin())))
		(_user->_wbuff).append("401 " + _user->_nickname + " " + (*(_param.begin())) + " :No such nick/channel\n");
	else
	{
		std::string				comment;
		User* 					killed = _tree->find_usr_by_nickname(*(_param.begin()));
		std::vector<Channel*>   ch_tmp = killed->_channels;

		for (size_t i = 1; i != _param.size() - 1; i++)
			comment.append(*(_param.begin() + i) + " ");
		comment.append(*(_param.begin() + _param.size() - 1));
		for (size_t i = 0; i < ch_tmp.size(); i++)
			ch_tmp[i]->send_message_all_members(":" + killed->_nickname + "!" + killed->_username + "@localhost QUIT " + killed->_nickname + " :was killed by " + _user->_nickname + "\n", "");
		killed->erase_me_from_allchannel(killed->_channels);
		(killed->_wbuff).append(":" + _user->_nickname + "!" + _user->_username + "@localhost KILL " + *(_param.begin())  + "\n");
		killed->erase_me_from_allchannel(killed->_channels);
		int i = 0;
		while (_fds[i].fd != killed->_fd)
			++i;
		closecon[i] = true;
		closecon[i + 1] = true;
	}
}


