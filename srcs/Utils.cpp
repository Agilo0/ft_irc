/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 12:15:51 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/09 12:52:00 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "Utils.hpp"
#include "Server.hpp"


//this functio will handle to convert all \n into \r\n, so everything will have the IRC format
std::string convertResponse(const std::string &response){
	std::string finalResponse;

	finalResponse.reserve(response.size() + 2); //stores the spaces that we need
	for (size_t i = 0; i < response.size(); i++){
		if (response[i] == '\n'){
			if (i == 0 || response[i - 1] !=  '\r')
				finalResponse += '\r';
		}
		finalResponse += response[i];//add the current char to the final string
	}
	if (finalResponse.empty() || finalResponse[finalResponse.size() - 1] != '\n' \
	|| (finalResponse.size() > 1 && finalResponse[finalResponse.size() - 2] != '\r')){
		finalResponse += "\r\n";
	}
	return finalResponse;
}