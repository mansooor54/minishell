# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/21 11:26:40 by malmarzo          #+#    #+#              #
#    Updated: 2024/12/30 23:45:43 by malmarzo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

CC = cc
CFLAGS = -Wall -Wextra -Werror -I.

# Function categories
FTIS = isalpha isdigit isalnum isascii isprint isspace
FTMEM = memset bzero memcpy memmove memchr memcmp
FTSTR = strlen strlcpy strlcat strchr strrchr strncmp strnstr
FTTO = toupper tolower atoi itoa
FTALLOC = calloc strdup
FTPUT = putchar_fd putstr_fd putendl_fd putnbr_fd
FTSTR2 = substr strjoin strtrim split strmapi striteri
FTEXTRA = strcpy strcmp

# Source files
SRC = $(addsuffix .c, $(addprefix ft_, $(FTIS))) \
      $(addsuffix .c, $(addprefix ft_, $(FTMEM))) \
      $(addsuffix .c, $(addprefix ft_, $(FTSTR))) \
      $(addsuffix .c, $(addprefix ft_, $(FTTO))) \
      $(addsuffix .c, $(addprefix ft_, $(FTALLOC))) \
      $(addsuffix .c, $(addprefix ft_, $(FTPUT))) \
      $(addsuffix .c, $(addprefix ft_, $(FTSTR2))) \
      $(addsuffix .c, $(addprefix ft_, $(FTEXTRA)))

OBJ = $(SRC:.c=.o)

# Colors for output
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@echo "\n$(GREEN)Compiling libft...$(RESET)"
	@ar rc $(NAME) $(OBJ)
	@ranlib $(NAME)
	@echo "$(GREEN)libft compiled successfully!$(RESET)"

%.o: %.c
	@printf "$(YELLOW)Generating libft objects... %-33.33s\r$(RESET)" $@
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning libft objects...$(RESET)"
	@rm -f $(OBJ)

fclean: clean
	@echo "$(RED)Cleaning libft archive...$(RESET)"
	@rm -f $(NAME)

re: fclean all

norm:
	@norminette $(SRC) *.h

.PHONY: all clean fclean re norm
