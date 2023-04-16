#ifndef _SERVER_HPP
#define _SERVER_HPP

#include "ft_irc.hpp"
#include "Tree.hpp"
#include "Parser.hpp"

class Tree;
class Parser;
class Server
{
    public:
        Server(const char*  pass, const int port):_pass(pass), _port(port), _tree(Tree()), _par(Parser(_tree, _pass))
        {
            memset(&_addr, 0, sizeof(_addr));
            _addr.sin6_family      = AF_INET6;
            memcpy(&_addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
            _addr.sin6_port        = htons(_port);

            _ret = 1;
            _closscon = false;
            memset(&_tmpfd, 0, sizeof(_tmpfd));
        }

        ~Server();

        int    check_revents(int i, short &_revents)
        {
            if (!_revents)
                return (0);
            if (!i && _revents == POLLIN)
                return (1);
            if (_revents == POLLIN)
                return (2);
            if (_revents == POLLOUT)
                return (3);
             if (_revents == POLLIN & POLLOUT)
                return (4);
            return (5);
        }

        void    set_up()
        {
            int on = 1;

            _tmpfd.fd = socket(AF_INET6, SOCK_STREAM, 0);
            if (_tmpfd.fd < 0)
            {
                perror("socket() failed");
                serv_exit(1);
            }

            _fds.push_back(_tmpfd);

            _ret = setsockopt(_fds[0].fd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
            if (_ret < 0)
            {
                perror("setsockopt() failed");
                serv_exit(1);
            }

            _ret = ioctl(_fds[0].fd, FIONBIO, (char *)&on);
            if (_ret < 0)
            {
                perror("ioctl() failed");
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

        void    loop()
        {
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
                for (int i = 0; i < _fds.size(); ++i)
                {
                    _ret = check_revents(i, _fds[i].revents);
                    switch(_ret)
                    {
                        case 1:
                            handle_lsocket_read();
                            break ;
                        case 2:
                        case 4:
                            handle_client_read(i);
                            break;
                        case 5:
                            std::cout << "Poll(), incorrect revents" << std::endl;
                            serv_exit(1);
                        default:
                            break ;
                    }
                    if (_closscon)
                    {
                        close(_fds[i].fd);
                        _fds.erase(_fds.begin() + i);
                        _closscon = false;
                    }
                }
                for (int i = 1; i < _fds.size(); ++i)
                {
                    _ret = check_revents(i, _fds[i].revents);
                    switch(_ret)
                    {
                        case 3:
                        case 4:
                            handle_client_write(i);
                            break ;
                        case 5:
                            std::cout << "Poll(), incorrect revents" << std::endl;
                            serv_exit(1);
                        default:
                            break ;
                    }
                    if (_closscon)
                    {
                        close(_fds[i].fd);
                        _fds.erase(_fds.begin() + i);
                        _closscon = false;
                    }
                }
            }

        }

        void    serv_exit(int _exitcode)
        {
            for (int i = 0; i < _fds.size(); i++)
            {
                if(_fds[i].fd >= 0)
                    close(_fds[i].fd);
            }
            exit(_exitcode);
        }

        void    handle_client_read(int &i)
        {
			_par.change_user(_tree.find_usr_by_fd(_fds[i].fd));
			while (!_closscon)
			{
				memset(_buffer, 0, BUFFER_SIZE);
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
					printf("  Connection closed\n");
					_closscon = true;
				}
				if (_ret <= 0)
					memcpy(_buffer, "QUIT\r\n", 6);
				(_par.get_user())->_rbuff.append(_buffer);
			}
            _par.execute();
        }

        void    handle_client_write(int &i)
		{
			User* user_ref = _tree.find_usr_by_fd(_fds[i].fd);
			_ret = send(_fds[i].fd, &(user_ref->_wbuff)[0], user_ref->_wbuff.size(), 0);
			if (_ret == -1)
			{
				_closscon = true;
				memcpy(_buffer, "QUIT\r\n", 6);
				_par.change_user(user_ref);
				user_ref->_rbuff.append(_buffer);
				_par.execute();
				return ;
			}
			memmove(&(user_ref->_wbuff)[0], &(user_ref->_wbuff)[_ret], user_ref->_wbuff.size() - _ret);
		}

        void    handle_lsocket_read()
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
                _tmpfd.events = POLLIN & POLLOUT;
                _fds.push_back(_tmpfd);
		    }
        }


    private:
        Tree                        _tree;//stores Users their fds and channels
        Parser                      _par;//

        std::vector<struct pollfd>  _fds;
        struct sockaddr_in6         _addr;

        const char                  *_pass;
        const int                   _port;
        int                         _ret;
        bool                        _closscon;

        struct pollfd               _tmpfd;
        char                       _buffer[BUFFER_SIZE];
};
#endif
