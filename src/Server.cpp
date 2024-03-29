#include "../inc/Server.hpp"

Server::Server(const char*  pass, const int port):  _tree(Tree()), _par(Parser(_tree, pass)),_pass(pass), _port(port)
{
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin6_family      = AF_INET6;
	memcpy(&_addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	_addr.sin6_port        = htons(_port);

	_ret = 1;
	memset(&_tmpfd, 0, sizeof(_tmpfd));
	memset(&_buffer, 0, sizeof(char) * BUFFER_SIZE);
	_closscon.push_back(false);
}

Server::~Server()
{

}

int    Server::check_revents(int i, short &_revents)
{
	if (!_revents)
		return (0);
	if (!i && _revents == POLLIN)
		return (1);
	if (_revents == POLLIN)
		return (2);
	if (_revents == POLLOUT)
		return (3);
	return (4);
}

void    Server::set_up()
{
	int on = 1;

	_tmpfd.fd = socket(AF_INET6, SOCK_STREAM, 0);

	if (_tmpfd.fd < 0)
	{
		perror("socket() failed");
		serv_exit(1);
	}

	_fds.push_back(_tmpfd);
	_ret = setsockopt(_fds[0].fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (_ret < 0)
	{
		perror("setsockopt() failed");
		serv_exit(1);
	}

	_ret = fcntl(_fds[0].fd, F_SETFL, O_NONBLOCK);
	if (_ret < 0)
	{
		perror("fcntl() failed");
		serv_exit(1);
	}
	_ret = bind(_fds[0].fd, (struct sockaddr *)&_addr, sizeof(_addr));
	if (_ret < 0)
	{
		perror("bind() failed");
		serv_exit(1);
	}

	_ret = listen(_fds[0].fd, 32);
	if (_ret < 0)
	{
		perror("listen() failed");
		serv_exit(1);
	}
}

void    Server::loop()
{
	User* user_ref(NULL);
	_fds[0].events = POLLIN;
	while(true)
	{
		_ret = poll(&_fds[0], _fds.size(), TIMEOUT * 60 * 1000);
		if (_ret < 0)
		{
			perror("poll() failed");
			serv_exit(1);
		}
		if (_ret == 0)
		{
			std::cout << "poll() timed out. End program." << std::endl;
			serv_exit(0);
		}
		for (size_t i = 0; i < _fds.size(); i += 2)
		{
			if (!_closscon[i])
			{
				_ret = check_revents(i, _fds[i].revents);
				switch(_ret)
				{
					case 1:
						handle_lsocket_read();
						break ;
					case 2:
						handle_client_read(i);
						break;
					case 4:
						std::cout << "Poll(), incorrect revents" << std::endl;
						user_ref = _tree.find_usr_by_fd(_fds[i].fd);
						memcpy(_buffer, "QUIT unexpected_quit\r\n", 22);
						_par.change_user(user_ref);
						user_ref->_rbuff = _buffer;
						_par.check_for_cmd(i, _closscon, _fds);
						break ;
					default:
						break ;
				}
			}
		}
		for (size_t i = 1; i < _fds.size(); i += 2)
		{
			_ret = check_revents(i, _fds[i].revents);
			switch(_ret)
			{
				case 3:
					handle_client_write(i);
					break ;
				case 4:
					std::cout << "Poll(), incorrect revents" << std::endl;
					user_ref = _tree.find_usr_by_fd(_fds[i].fd);
					user_ref->_wbuff = "";
					_closscon[i] = true;
					_closscon[i - 1] = true;
					break ;
				default:
					break ;
			}
		}
		for (size_t i = 1; i < _closscon.size(); i += 2)
		{
			if (_closscon[i])
			{
				User* user = _tree.find_usr_by_fd(_fds[i].fd);
				if ((user->_wbuff).empty())
				{
					_tree.erase_user(*user);
					close(_fds[i].fd);
					_fds.erase(_fds.begin() + i + 1);
					_fds.erase(_fds.begin() + i);
					_closscon.erase(_closscon.begin() + i + 1);
					_closscon.erase(_closscon.begin() + i);
				}
			}
		}
		for (size_t i = 1; i < _fds.size(); i += 2)
		{
			if (((_tree.find_usr_by_fd(_fds[i].fd))->_wbuff).empty())
				_fds[i].events = 0;
			else
				_fds[i].events = POLLOUT;
		}
	}

}

void    Server::serv_exit(int _exitcode)
{
	for (size_t i = 0; i < _fds.size(); i += 2)
	{
		if(_fds[i].fd >= 0)
			close(_fds[i].fd);
	}
	exit(_exitcode);
}

void    Server::handle_client_read(size_t &i)
{
	bool close = false;
	_par.change_user(_tree.find_usr_by_fd(_fds[i].fd));

	while (!close)
	{
		memset(_buffer, 0, sizeof(char) * BUFFER_SIZE);
		_ret = recv(_fds[i].fd, _buffer, BUFFER_SIZE, 0);
		if (_ret < 0)
		{
			if (errno == EWOULDBLOCK)
				break ;
			perror("  recv() failed");
			close = true;
		}
		if (_ret == 0)
		{
			std::cout << "  Connection closed" << std::endl;
			close = true;
		}
		if (_ret <= 0)
		{
			memcpy(_buffer, "QUIT unexpected_quit\r\n", 22);
			(_par.get_user())->_rbuff = "";
			close = true;
		}
		(_par.get_user())->_rbuff.append(_buffer);
	}
	std::cout << "read -> " << (_par.get_user())->_rbuff << std::endl;
	_par.check_for_cmd(i, _closscon, _fds);
	if (close)
	{
		_closscon[i] = true;
		_closscon[i + 1] = true;
	}
}

void    Server::handle_client_write(size_t &i)
{
	User* user_ref = _tree.find_usr_by_fd(_fds[i].fd);
	std::cout << "wrote -> " << user_ref->_wbuff << std::endl;
	_ret = send(_fds[i].fd, &(user_ref->_wbuff)[0], user_ref->_wbuff.size(), 0);
	if (_ret == -1)
	{
		memcpy(_buffer, "QUIT unexpected_quit\r\n", 22);
		_par.change_user(user_ref);
		user_ref->_rbuff.append(_buffer);
		_par.check_for_cmd(i, _closscon, _fds);
		_closscon[i] = true;
		_closscon[i - 1] = true;
		return ;
	}
	user_ref->_wbuff = (user_ref->_wbuff).substr(_ret);
}

void    Server::handle_lsocket_read()
{
	_tmpfd.fd = 1;
	while (_tmpfd.fd != -1)
	{
		_tmpfd.fd = accept(_fds[0].fd, NULL, NULL);
		if (_tmpfd.fd < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				perror("  accept() failed");
				serv_exit(1);
			}
			break ;
		}
		_ret = fcntl(_tmpfd.fd, F_SETFL, O_NONBLOCK);
		if (_ret < 0)
		{
			perror("fcntl() failed");
			serv_exit(1);
		}
		_tmpfd.events = POLLOUT;
		_fds.push_back(_tmpfd);
		_tmpfd.events = POLLIN;
		_fds.push_back(_tmpfd);
		_tree.insert_by_fd(_tmpfd.fd);
		_closscon.push_back(false);
		_closscon.push_back(false);
	}
}
