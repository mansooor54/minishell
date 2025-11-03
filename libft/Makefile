# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: student <student@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/02 00:00:00 by student           #+#    #+#              #
#    Updated: 2025/11/02 00:00:00 by student          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCS = ft_strlen.c \
       ft_strdup.c \
       ft_strchr.c \
       ft_strjoin.c \
       ft_substr.c \
       ft_strcmp.c \
       ft_strcpy.c \
       ft_isalnum.c \
       ft_calloc.c \
       ft_atoi.c \
       ft_itoa.c \
       ft_putchar_fd.c \
       ft_putnbr_fd.c \
       ft_split.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@ar rcs $(NAME) $(OBJS)
	@echo "libft.a created successfully!"

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
