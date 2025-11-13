# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#              #
#    Updated: 2025/11/13 11:58:13 by malmarzo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
INCLUDES = -I. -I./libft

# Try to detect readline via pkg-config (Linux) or Homebrew (macOS).
READLINE_CFLAGS := $(shell pkg-config --cflags readline 2>/dev/null)
READLINE_LDFLAGS := $(shell pkg-config --libs readline 2>/dev/null)

ifeq ($(READLINE_CFLAGS)$(READLINE_LDFLAGS),)
READLINE_PREFIX := $(shell brew --prefix readline 2>/dev/null)
  ifneq ($(READLINE_PREFIX),)
READLINE_CFLAGS = -I$(READLINE_PREFIX)/include
READLINE_LDFLAGS = -L$(READLINE_PREFIX)/lib -lreadline
  else
READLINE_CFLAGS =
READLINE_LDFLAGS = -lreadline
  endif
endif

CFLAGS += $(READLINE_CFLAGS)
LDFLAGS = $(READLINE_LDFLAGS) -L./libft -lft
# Libft
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# Source files
SRC_DIR = src
SRCS = main.c \
       $(SRC_DIR)/builtins/builtins.c \
       $(SRC_DIR)/builtins/builtin_echo.c \
       $(SRC_DIR)/builtins/builtin_cd.c \
       $(SRC_DIR)/builtins/builtin_cd_utils.c \
       $(SRC_DIR)/builtins/builtin_pwd.c \
       $(SRC_DIR)/builtins/builtin_env.c \
       $(SRC_DIR)/builtins/builtin_export.c \
       $(SRC_DIR)/builtins/builtin_unset.c \
       $(SRC_DIR)/builtins/builtin_exit.c \
       $(SRC_DIR)/environment/env.c \
       $(SRC_DIR)/environment/env_node.c \
       $(SRC_DIR)/environment/env_utils.c \
       $(SRC_DIR)/environment/env_array.c \
       $(SRC_DIR)/expander/expander_pipeline.c \
       $(SRC_DIR)/expander/expander_core.c \
       $(SRC_DIR)/expander/expander_utils.c \
       $(SRC_DIR)/expander/expander_quotes.c \
       $(SRC_DIR)/expander/expander_vars.c \
       $(SRC_DIR)/expander/expander.c \
       $(SRC_DIR)/executor/executor.c \
       $(SRC_DIR)/executor/executor_commands.c \
       $(SRC_DIR)/executor/executor_external.c \
       $(SRC_DIR)/executor/executor_path.c \
       $(SRC_DIR)/executor/executor_child_fds.c \
       $(SRC_DIR)/executor/executor_child_run.c \
       $(SRC_DIR)/executor/executor_pipeline_loop.c \
       $(SRC_DIR)/executor/executor_pipeline_run.c \
       $(SRC_DIR)/executor/executor_pipeline_utils.c \
       $(SRC_DIR)/executor/executor_pipeline.c \
       $(SRC_DIR)/executor/executor_error.c \
       $(SRC_DIR)/executor/executor_path_search.c \
       $(SRC_DIR)/executor/executor_path_utils.c \
       $(SRC_DIR)/executor/executor_redir_heredoc.c \
       $(SRC_DIR)/executor/executor_redir_io.c \
       $(SRC_DIR)/executor/executor_redirections.c \
       $(SRC_DIR)/lexer/lexer.c \
       $(SRC_DIR)/lexer/lexer_utils.c \
       $(SRC_DIR)/lexer/lexer_operator.c \
       $(SRC_DIR)/lexer/lexer_operator_type.c \
       $(SRC_DIR)/lexer/lexer_unclose.c \
       $(SRC_DIR)/parser/parser.c \
       $(SRC_DIR)/parser/parser_utils.c \
       $(SRC_DIR)/parser/parser_pipeline.c \
       $(SRC_DIR)/parser/parser_error.c \
       $(SRC_DIR)/parser/parser_syntax_check.c \
       $(SRC_DIR)/parser/parser_syntax_print.c \
       $(SRC_DIR)/parser/parser_check_token.c \
       $(SRC_DIR)/utils/utils.c \
       $(SRC_DIR)/signals/signals.c \
       $(SRC_DIR)/history/history.c \
       $(SRC_DIR)/history/builtin_history.c \
       $(SRC_DIR)/core/reader.c \
       $(SRC_DIR)/core/read_logical_line.c \
       minishell_logo.c

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

leak: all
	valgrind --leak-check=full \
	--show-leak-kinds=all --track-fds=all --trace-children=yes ./$(NAME)

.PHONY: all clean fclean re
