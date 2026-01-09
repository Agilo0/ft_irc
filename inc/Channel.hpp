/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 11:01:19 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/09 23:14:46 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

//#include "Server.hpp"
//#include "Utils.hpp"

enum CommandType{
	PASS,
	NICK,
	USER,
	PART,
	//CAPEND,
	JOIN,
	WHO,
	PRIVMSG,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	QUIT,
	PING,
	UKNW
};


class Channel{
	private:
		std::string	_name;
		std::string _topic;
		std::string _key;//optional key to join the channel
		size_t		_maxUsers;
		bool		_iMode;//(-/= invite only)
		bool		_tMode;//(topic set by operators only)
		bool		_kMode;
		bool		_oMode;
		bool		_lMode;
		std::set<int> _clients;//all the clients that we have in the channel (store FDs)
		std::set<int> _operators;//operator is the one that has more privileges in the channel
		std::set<int> _invited;//clients that are invited to the channel
	
	public:
		Channel(const std::string &name);
		
		/*getters*/
		std::string getName() const;//get channel name
		std::string getTopic() const;
		std::string getKey() const;
		size_t		getMaxUsers() const;
		const std::set<int> &getClients() const;
		
		/*setters*/
		void setTopic(const std::string &topic);
		void setKey(const std::string &key);
		void setMaxUsers(int max);
		
		void setModeI(bool active);
		void setModeT(bool active);
		void setModeK(bool active);
		void setModeO(bool active);
		void setModeL(bool active);
		
		/*booleans*/
		bool hasTopic() const;
		bool isModeI() const;
		bool isModeT() const;
		bool isModeK() const;
		bool isModeO() const;
		bool isModeL() const;
		//bool hasKey() const;

		bool isEmpty() const;

		bool addClient(int fd, bool isOperator = false);
		bool isMember(int fd) const;
		bool isOperator(int fd) const;
		bool isInvited(int fd) const;

		//client management
		void removeClient(int fd);
		void addOperator(int fd);
		void removeOperator(int fd);
		void inviteClient(int fd);
		void removeInvite(int fd);

		//modes
		int manageModeChange(std::string mode, std::string arg, int targetFd);
		void manageK(bool sign, std::string arg);
		int manageO(bool sign, int targetFd);
		int manageL(bool sign, std::string arg);
		bool isStrictNumber(const std::string &s);
};


#endif