/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <signal.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"

/*
** Token types for lexer
** Used to identify different elements in the command line input
*/
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_HEREDOC,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_EOF
}	t_token_type;

/*
** Token structure
** Represents a single token from the lexer with its type and value
*/
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

/*
** Redirection structure
** Stores information about input/output redirections for a command
*/
typedef struct s_redir
{
	t_token_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

/*
** Command structure
** Contains command arguments and associated redirections
*/
typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;

/*
** Pipeline structure with logical operators
** Represents a pipeline of commands with optional logical operators (&&, ||)
*/
typedef struct s_pipeline
{
	t_cmd				*cmds;
	t_token_type		logic_op;
	struct s_pipeline	*next;
}	t_pipeline;

/*
** Environment variable structure
** Stores key-value pairs for environment variables
*/
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

/*
** Main shell data structure
** Contains all shell state including environment and exit status
*/
typedef struct s_shell
{
	t_env		*env;
	int			exit_status;
	int			should_exit;
}	t_shell;

/*
** Global shell instance for signal handling
** Required to access shell state from signal handlers
** This is the only global variable allowed by the subject
*/
extern t_shell	g_shell;

/* ========== LEXER ========== */
/*
** Tokenize input string into a linked list of tokens
*/
t_token		*lexer(char *input);

/*
** Create a new token with given type and value
*/
t_token		*create_token(t_token_type type, char *value);

/*
** Add a token to the end of the token list
*/
void		add_token(t_token **tokens, t_token *new_token);

/*
** Free all tokens in the list
*/
void		free_tokens(t_token *tokens);

/* ========== PARSER ========== */
/*
** Parse tokens into a pipeline structure
*/
t_pipeline	*parser(t_token *tokens);

/*
** Parse a single command from tokens
*/
t_cmd		*parse_command(t_token **tokens);

/*
** Parse redirections for a command
*/
t_redir		*parse_redirections(t_token **tokens);

/*
** Free entire pipeline structure
*/
void		free_pipeline(t_pipeline *pipeline);

/* ========== EXPANDER ========== */
/*
** Expand environment variables in pipeline commands
*/
void		expander(t_pipeline *pipeline, t_env *env);

/*
** Expand variables in a string ($VAR and $?)
*/
char		*expand_variables(char *str, t_env *env, int exit_status);

/*
** Get environment variable value by key
*/
char		*get_env_value(t_env *env, char *key);

/* ========== EXECUTOR ========== */
/*
** Execute a pipeline with logical operators
*/
void		executor(t_pipeline *pipeline, t_shell *shell);

/*
** Execute a single pipeline
*/
void		execute_pipeline(t_pipeline *pipeline, t_shell *shell);

/*
** Execute a single command
*/
void		execute_command(t_cmd *cmd, t_shell *shell);

/*
** Setup redirections for a command
*/
int			setup_redirections(t_redir *redirs);

/*
** Find executable in PATH or return absolute/relative path
*/
char		*find_executable(char *cmd, t_env *env);

/* ========== BUILTINS ========== */
/*
** Check if command is a builtin
*/
int			is_builtin(char *cmd);

/*
** Execute a builtin command
*/
int			execute_builtin(t_cmd *cmd, t_shell *shell);

/*
** Builtin: echo - print arguments with optional -n flag
*/
int			builtin_echo(char **args);

/*
** Builtin: cd - change directory (relative or absolute path)
*/
int			builtin_cd(char **args, t_env *env);

/*
** Builtin: pwd - print working directory
*/
int			builtin_pwd(void);

/*
** Builtin: export - set environment variables
*/
int			builtin_export(char **args, t_env **env);

/*
** Builtin: unset - remove environment variables
*/
int			builtin_unset(char **args, t_env **env);

/*
** Builtin: env - print all environment variables
*/
int			builtin_env(t_env *env);

/*
** Builtin: exit - exit the shell
*/
int			builtin_exit(char **args, t_shell *shell);

/* ========== ENVIRONMENT ========== */
/*
** Initialize environment from envp array
*/
t_env		*init_env(char **envp);

/*
** Create a new environment node
*/
t_env		*create_env_node(char *key, char *value);

/*
** Add environment node to list
*/
void		add_env_node(t_env **env, t_env *new_node);

/*
** Remove environment node by key
*/
void		remove_env_node(t_env **env, char *key);

/*
** Convert environment list to array
*/
char		**env_to_array(t_env *env);

/*
** Free all environment nodes
*/
void		free_env(t_env *env);

/* ========== SIGNALS ========== */
/*
** Setup signal handlers for the shell
*/
void		setup_signals(void);

/*
** Handle SIGINT (Ctrl-C) signal
*/
void		handle_sigint(int sig);

/*
** Handle SIGQUIT (Ctrl-\) signal
*/
void		handle_sigquit(int sig);

/* ========== UTILS ========== */
/*
** Free a NULL-terminated array of strings
*/
void		free_array(char **arr);

/*
** Join two strings and free the first one
*/
char		*ft_strjoin_free(char *s1, char const *s2);

#endif
