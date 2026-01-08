# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/13 16:35:30 by yanaranj          #+#    #+#              #
#    Updated: 2026/01/08 13:07:58 by yaja             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NC = \033[0m
RED = \e[1;91m
GREEN = \e[1;92m
YELLOW = \e[1;93m
BLUE = \e[1;94m


NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -I ./inc/ -fsanitize=address
RM = rm -f

#directories
SRCSDIR = ./srcs/
OBJDIR = ./objs/

#files
SRCS_FILES = Channel.cpp Client.cpp Server.cpp ServerAuth.cpp ServerAux.cpp \
	ServerCommands.cpp Utils.cpp Notification.cpp ../main.cpp \


#sources & objects files
SRCS = $(addprefix $(SRCSDIR), $(SRCS_FILES))
OBJS = $(SRCS:$(SRCSDIR)%.cpp=$(OBJDIR)%.o)

HEADER = ./inc/Channel.hpp ./inc/Client.hpp .inc/Responses.hpp .inc/Server.hpp ./inc/Utils.hpp

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