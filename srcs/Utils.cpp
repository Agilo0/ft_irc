/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 12:15:51 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/01 17:37:38 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* FORMATING RESPONSES*/







/* parsing */

int Utils::check_port(char *port){
    int i = 0;
    long res = 0;

    while (port[i])
    {
        if (!isdigit(static_cast<unsigned char>(port[i])))
            throw std::runtime_error("Error: Invalid port: needs only digits.");
        res = res * 10 + (port[i] - '0');
        i++;
    }
    if (res <= 1023 || res > 65535){
        throw std::runtime_error("Error: Recommended port for IRC: 6667.");
    }
    return (static_cast<int>(res));
}

bool Utils::check_password(const std::string &password){
    if (password.empty() || password.size() > 25)
        return (false);
    for (size_t i = 0; i < password.size(); i++){
        if (password[i] < 33 || password[i] > 126)
            return false;
    }
    return true;
}
