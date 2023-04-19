#include "../inc/Server.hpp"

Server::Server(const char*  pass, const int port):  _tree(Tree()), _par(Parser(_tree, pass)),_pass(pass), _port(port)
{
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin6_family      = AF_INET6;
	memcpy(&_addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	_addr.sin6_port        = htons(_port);

	_ret = 1;
	_closscon = false;
	memset(&_tmpfd, 0, sizeof(_tmpfd));
	memset(&_buffer, 0, sizeof(char) * BUFFER_SIZE);
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
					memcpy(_buffer, "QUIT unexpected_quit\n", 22);
					_par.change_user(user_ref);
					user_ref->_rbuff.append(_buffer);
					if (!_par.check_for_cmd())
						_par.execute(_closscon);
					std::cout << "FUCCCCC\n";
					break ;
				default:
					break ;
			}
			if (_closscon)
			{
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
				_fds.erase(_fds.begin() + i - 1);
				_closscon = false;
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
					memcpy(_buffer, "QUIT unexpected_quit\n", 22);
					_par.change_user(user_ref);
					user_ref->_rbuff.append(_buffer);
					if (!_par.check_for_cmd())
						_par.execute(_closscon);
					break ;
				default:
					break ;
			}
			if (_closscon)
			{
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
				_fds.erase(_fds.begin() + i + 1);
				_closscon = false;
			}
		}
		for (size_t i = 1; i < _fds.size(); i += 2)
		{
			if (((_tree.find_usr_by_fd(_fds[i].fd))->_wbuff).empty())
			{
				_fds[i].events = 0;
			}
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
	_par.change_user(_tree.find_usr_by_fd(_fds[i].fd));

	while (!_closscon)
	{
		memset(_buffer, 0, sizeof(char) * BUFFER_SIZE);
		_ret = recv(_fds[i].fd, _buffer, BUFFER_SIZE, 0);
		if (_ret < 0)
		{
			if (errno == EWOULDBLOCK)
				break ;
			perror("  recv() failed");
			_closscon = true;
		}
		if (_ret == 0)
		{
			std::cout << "  Connection closed" << std::endl;
			_closscon = true;
		}
		if (_ret <= 0)
			memcpy(_buffer, "QUIT\n", 6);
		(_par.get_user())->_rbuff.append(_buffer);
	}
	if (!_par.check_for_cmd())
		_par.execute(_closscon);
}

void    Server::handle_client_write(size_t &i)
{
	User* user_ref = _tree.find_usr_by_fd(_fds[i].fd);
	_ret = send(_fds[i].fd, &(user_ref->_wbuff)[0], user_ref->_wbuff.size(), 0);
	if (_ret == -1)
	{
		memcpy(_buffer, "QUIT\n", 6);
		_par.change_user(user_ref);
		user_ref->_rbuff.append(_buffer);
		if (!_par.check_for_cmd())
			_par.execute(_closscon);
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
	}
}
