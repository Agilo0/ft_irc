/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAux.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:48:05 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/10 11:42:47 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Channel *Server::moveCreateChannel(const std::string &channelName){
	//if we have the name on the list, we send the channel
	for(size_t i = 0; i < _channels.size(); i++){
		if (_channels[i].getName() == channelName)
			return &_channels[i];
	}
	//if not, we add this new name on the vector
	_channels.push_back(Channel(channelName));
	return &_channels.back();
}