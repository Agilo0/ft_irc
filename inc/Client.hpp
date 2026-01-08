/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/08 12:55:33 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
// #include "Server.hpp"
#include "Channel.hpp"

enum Status
{
    NOT_AUTHENTICATED,
    AUTHENTICATED
};

class Client
{
private:
    int _clientFd;
    std::string _nickname;
    std::string _oldnick;
    std::string _username;
    std::string _realname;
    std::string _ip; // or host
    std::string _buffer;
    Status _status;
    bool hasNick, hasUser, hasPass;
    bool _toRemove;
    std::vector<Channel *> _channels;

public:
    Client();
    ~Client();

    // getters
    Status getStatus();
    int getClientFd() const; //<--socket FD of the client
    std::string getUsername() const;
    std::string getNickname() const;
    std::string getClientIP() const;
    std::string &getBuff();
    std::string getNick() const;
    std::string getOldnick() const;
    Channel *getChannel(unsigned int index);

    // setters
    void setStatus(Status status);
    void setClientFd(int fd);
    void setClientIP(const std::string &newIP);
    void setPass();
    void setFirstNick(std::string nick);
    void setNewNick(std::string nick);
    void setRealName(std::string name);
    void setUser(std::string name);
    void setChannel(Channel *channel);

    // bools
    bool hasAll() const;
    bool hasPassw() const;
    bool hasNickname() const;
    bool hasUsername() const;
    bool isToRemove() const;
    bool isRegistered() const;

    // others
    void markForRevome();
    void addBuffer(const std::string &data);
    int quitChannel(std::string channel);
    std::string createMessage();
};

#endif
