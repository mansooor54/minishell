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

NAME = minishell

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
INCLUDES = -I./include -I./libft -I$(HOME)/.brew/opt/readline/include
LDFLAGS = -L$(HOME)/.brew/opt/readline/lib -lreadline -L./libft -lft

# Libft
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# Source files
SRC_DIR = src
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/lexer/lexer.c \
       $(SRC_DIR)/lexer/lexer_utils.c \
       $(SRC_DIR)/parser/parser.c \
       $(SRC_DIR)/parser/parser_pipeline.c \
       $(SRC_DIR)/expander/expander.c \
       $(SRC_DIR)/executor/executor.c \
       $(SRC_DIR)/executor/executor_pipeline.c \
       $(SRC_DIR)/executor/redirections.c \
       $(SRC_DIR)/builtins/builtins.c \
       $(SRC_DIR)/builtins/builtin_echo.c \
       $(SRC_DIR)/builtins/builtin_cd.c \
       $(SRC_DIR)/builtins/builtin_pwd.c \
       $(SRC_DIR)/builtins/builtin_env.c \
       $(SRC_DIR)/builtins/builtin_export.c \
       $(SRC_DIR)/builtins/builtin_unset.c \
       $(SRC_DIR)/builtins/builtin_exit.c \
       $(SRC_DIR)/utils/environment.c \
       $(SRC_DIR)/utils/environment_utils.c \
       $(SRC_DIR)/utils/utils.c \
       $(SRC_DIR)/signals/signals.c

# Object files
OBJ_DIR = obj
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
RESET = \033[0m

# Rules
all: $(LIBFT) $(NAME)

$(LIBFT):
	@echo "$(YELLOW)Compiling libft...$(RESET)"
	@make -C $(LIBFT_DIR)

$(NAME): $(OBJS) $(LIBFT)
	@echo "$(GREEN)Linking $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)$(NAME) created successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(GREEN)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Removing object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@make -C $(LIBFT_DIR) clean

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
