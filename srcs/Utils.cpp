/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 12:15:51 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/12 12:29:25 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


/*	FORMATING RESPONSES	*/
//this function will handle to convert all \n into \r\n, so everything will have the IRC format
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

//this function makes sure that we are sending the proper response
void Server::sendResponse(int clientFd, const std::string &response){
	std::string formatResponse = convertResponse(response);

	if (send(clientFd, formatResponse.c_str(), formatResponse.size(), 0) == -1){
		std::cerr << RED << "Error sending response to: " << clientFd << " client." << NC << std::endl;
	}
}

//split all the tokens for proper parsing
std::vector<std::string> Utils::split(const std::string &str, char delimiter){
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while(std::getline(ss, token, delimiter))
		tokens.push_back(token);
	return tokens;
}