#ifndef _TREE_HPP
#define _TREE_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "murmur_hash.hpp"

auto hash_func = [](const std::pair<std::string, int>& p) {
    return murmur_hash_32(p.first.cstr(), p.first.length(), p.second);
};

class Tree
{
    private:
        std::unordered_map<std::pair<int, std::string>, User, decltype(hashfunc)>   _m;

    public:
        Tree(): _m(hashfunc) {};
        ~Tree();
        
        User& operator[](const std::string& nickname) 
        {
            for (auto& p : _m) {
                if (p.second.get_nickname() == nickname) {
                    return p.second;
                }
            }
            throw std::runtimeerror("Nickname not found");
        }

        User& operator[](int fd) {
            for (auto& p : _m) {
                if (p.second.get_fd() == fd) {
                    return p.second;
                }
            }
            throw std::runtime_error("FD not found");
        }
        
        void    insert(const std::string &nick_name, int fd)
        {
            User user(nick_name, fd);
            _m[{nick_name, fd}] = user;
        }
        void    add_buffer(std::string _buff, int flag, int fd, std::string _nick); // find the USER in the map & adds the read or write buffer to our USER
        {

        }
        
        void    remove_buffer()
        {}
};

#endif
