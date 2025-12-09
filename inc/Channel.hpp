/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 11:01:19 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/09 12:37:53 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

//#include "Utils.hpp"
#include "Channel.hpp"
#include "Server.hpp"


class Channel{
	private:
		std::string	_name;
		std::string _topic;
		std::string _key;//optional key to join the channel
		int			_maxUsers;
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
		~Channel();
		std::string getName() const;
		void setTopic(const std::string &topic);
		void hasTopic();
		
		void setModeI(bool active);
		void isModeI();
};


#endif