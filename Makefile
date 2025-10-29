# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/29 15:23:52 by malmarzo          #+#    #+#              #
#    Updated: 2025/10/29 15:23:52 by malmarzo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRCS = \
	src/main.c \
	src/utils.c \
	src/env.c \
	src/prompt.c \
	src/parser.c \
	src/builtins.c \
	src/exec.c \
	src/signals.c

OBJS = $(SRCS:.c=.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror

READLINE_PREFIX = $(PWD)/vendor/readline

INCLUDES = -I$(READLINE_PREFIX)/include
LDFLAGS  = -L$(READLINE_PREFIX)/lib -lreadline -lhistory

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

%.o: %.c minishell.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
