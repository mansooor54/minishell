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
INCLUDES = -I. -I./include -I./libft

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

HEADER = include/minishell.h

BUILTINS = builtin_cd_utils builtin_cd builtin_echo \
            builtin_env builtin_exit builtin_export \
            builtin_export_utils builtin_pwd builtin_unset builtins

CORE = read_logical_line read_logical_line_utils shell_loop shell_utils join_continuation

ENVIRONMENT = env_array env_node env_utils env

MAIN = main minishell_logo

PARSER = parser_check_token parser_error parser_pipeline \
          parser_syntax_check parser_syntax_print parser_syntax_utils \
          parser_utils parser

EXECUTOR = executor_child_fds executor_child_run \
            executor_commands executor_commands_utils executor_error executor_external \
            executor_external_utils executor_path_search executor_path_utils executor_path \
            executor_pipeline_loop executor_pipeline_run executor_pipeline_utils \
            executor_pipeline_helpers executor_pipeline executor_redir_heredoc \
            executor_redir_heredoc_utils executor_redir_io \
            executor_redirections executor_utils executor

EXPANDER = expander_core expander_pipeline expander_quotes \
            expander_utils expander_vars expander_utils_helpers

HISTORY = history history_utils history_load

LEXER = lexer_operator_type lexer_operator lexer_unclose \
         lexer_utils lexer

SIGNALS = signals

UTILS = utils utils_num ft_atoll

# Source files - FIXED: Using consistent SRCS variable
SRCS = $(addsuffix .c, $(addprefix src/builtins/, $(BUILTINS))) \
        $(addsuffix .c, $(addprefix src/core/, $(CORE))) \
        $(addsuffix .c, $(addprefix src/environment/, $(ENVIRONMENT))) \
        $(addsuffix .c, $(addprefix src/main/, $(MAIN))) \
        $(addsuffix .c, $(addprefix src/executor/, $(EXECUTOR))) \
        $(addsuffix .c, $(addprefix src/expander/, $(EXPANDER))) \
        $(addsuffix .c, $(addprefix src/history/, $(HISTORY))) \
        $(addsuffix .c, $(addprefix src/lexer/, $(LEXER))) \
        $(addsuffix .c, $(addprefix src/parser/, $(PARSER))) \
        $(addsuffix .c, $(addprefix src/signals/, $(SIGNALS))) \
        $(addsuffix .c, $(addprefix src/utils/, $(UTILS)))

# Object files directory
OBJ_DIR = obj

# FIXED: Changed from OBJ = $(SRC:c=o) to OBJS = $(SRCS:.c=.o)
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	@echo "\n"
	@make -C libft/
	@echo "\033[0;32mCompiling minishell..."
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)  # FIXED: Using LDFLAGS instead of LIBFT
	@echo "\n\033[0mDone !"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@printf "\033[0;33mGenerating minishell objects... %-33.33s\r" $@
	@${CC} ${CFLAGS} -c $< -o $@

clean:
	@echo "\033[0;31mCleaning libft..."
	@make clean -C libft/
	@echo "\nRemoving binaries..."
	@rm -rf $(OBJ_DIR)
	@echo "\033[0m"

fclean:
	@echo "\033[0;31mCleaning libft..."
	@make fclean -C libft/
	@echo "\nDeleting objects..."
	@rm -rf $(OBJ_DIR)
	@echo "\nDeleting executable..."
	@rm -f $(NAME)
	@echo "\033[0m"

re: fclean all

test: all
	./minishell

norm:
	norminette $(SRCS) $(HEADER)

leak: all
	valgrind --leak-check=full \
	--show-leak-kinds=all --track-fds=all --trace-children=yes ./$(NAME)

.PHONY: all clean fclean re
