# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+         #
#                                                +#+#+#+#+#+   +#+             #
#    Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#              #
#    Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
INCLUDES = -I. -I./libft 
LDFLAGS = -lreadline -L./libft -lft

# Try to detect Homebrew readline and ncurses on macOS; otherwise fall back
READLINE_PREFIX := $(shell brew --prefix readline 2>/dev/null)
NCURSES_PREFIX  := $(shell brew --prefix ncurses  2>/dev/null)

ifeq ($(READLINE_PREFIX),)
    # Linux or no Homebrew detected
    CFLAGS  +=
    LDFLAGS += -lreadline -lhistory -lncurses
else
    CFLAGS  += -I$(READLINE_PREFIX)/include
    LDFLAGS += -L$(READLINE_PREFIX)/lib -lreadline -lhistory
    ifneq ($(NCURSES_PREFIX),)
        LDFLAGS += -L$(NCURSES_PREFIX)/lib -lncurses
    else
        # Some macOS setups still need ncurses from system
        LDFLAGS += -lncurses
    endif
endif

# Libft
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# Source files
SRC_DIR = src
SRCS = main.c \
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
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

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

$(OBJ_DIR)/%.o: %.c
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
