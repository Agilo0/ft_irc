# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alounici <alounici@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/13 16:35:30 by yanaranj          #+#    #+#              #
#    Updated: 2025/12/19 20:23:48 by alounici         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NC = \033[0m
RED = \e[1;91m
GREEN = \e[1;92m
YELLOW = \e[1;93m
BLUE = \e[1;94m


NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -I ./inc/
RM = rm -f

#directories
SRCSDIR = ./srcs/
OBJDIR = ./objs/

#files
SRCS_FILES = Server.cpp Client.cpp Channel.cpp \
	ServerAux.cpp ServerCommands.cpp Utils.cpp ../main.cpp \

#sources & objects files
SRCS = $(addprefix $(SRCSDIR), $(SRCS_FILES))
OBJS = $(SRCS:$(SRCSDIR)%.cpp=$(OBJDIR)%.o)

HEADER = ./inc/Server.hpp ./inc/Utils.hpp .inc/Client.hpp

#compile .cpp to .o
$(OBJDIR)%.o: $(SRCSDIR)%.cpp | $(OBJSDIR) #$(HEADER) Makefile
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling... $(patsubst $(DIR_BUILD)%,%,$@)"
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN)[✔] $(NAME) compiled successfully!$(NC)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)


clean:
	@$(RM) $(OBJS)
	@echo "$(RED)objects had been removed$(NC)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Bye!🫰 $(NC)"

re: fclean all

.PHONY: all clean fclean re