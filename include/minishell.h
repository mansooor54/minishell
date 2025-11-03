/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:00:00 by student           #+#    #+#             */
/*   Updated: 2025/11/02 00:00:00 by student          ###   ########.fr       */
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
# include "../libft/libft.h"

/* Token types for lexer */
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

/* Token structure */
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

/* Redirection structure */
typedef struct s_redir
{
	t_token_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

/* Command structure */
typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;

/* Pipeline structure with logical operators */
typedef struct s_pipeline
{
	t_cmd				*cmds;
	t_token_type		logic_op;
	struct s_pipeline	*next;
}	t_pipeline;

/* Environment variable structure */
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

/* Main shell data structure */
typedef struct s_shell
{
	t_env		*env;
	int			exit_status;
	int			should_exit;
}	t_shell;

/* Global shell instance for signal handling */
extern t_shell	g_shell;

/* ========== LEXER ========== */
t_token		*lexer(char *input);
t_token		*create_token(t_token_type type, char *value);
void		add_token(t_token **tokens, t_token *new_token);
void		free_tokens(t_token *tokens);

/* ========== PARSER ========== */
t_pipeline	*parser(t_token *tokens);
t_cmd		*parse_command(t_token **tokens);
t_redir		*parse_redirections(t_token **tokens);
void		free_pipeline(t_pipeline *pipeline);

/* ========== EXPANDER ========== */
void		expander(t_pipeline *pipeline, t_env *env);
char		*expand_variables(char *str, t_env *env, int exit_status);
char		*get_env_value(t_env *env, char *key);

/* ========== EXECUTOR ========== */
void		executor(t_pipeline *pipeline, t_shell *shell);
void		execute_pipeline(t_pipeline *pipeline, t_shell *shell);
void		execute_command(t_cmd *cmd, t_shell *shell);
int			setup_redirections(t_redir *redirs);
char		*find_executable(char *cmd, t_env *env);

/* ========== BUILTINS ========== */
int			is_builtin(char *cmd);
int			execute_builtin(t_cmd *cmd, t_shell *shell);
int			builtin_echo(char **args);
int			builtin_cd(char **args, t_env *env);
int			builtin_pwd(void);
int			builtin_export(char **args, t_env **env);
int			builtin_unset(char **args, t_env **env);
int			builtin_env(t_env *env);
int			builtin_exit(char **args, t_shell *shell);

/* ========== ENVIRONMENT ========== */
t_env		*init_env(char **envp);
t_env		*create_env_node(char *key, char *value);
void		add_env_node(t_env **env, t_env *new_node);
void		remove_env_node(t_env **env, char *key);
char		**env_to_array(t_env *env);
void		free_env(t_env *env);

/* ========== SIGNALS ========== */
void		setup_signals(void);
void		handle_sigint(int sig);
void		handle_sigquit(int sig);

/* ========== UTILS ========== */
void		free_array(char **arr);
char		*ft_strjoin_free(char *s1, char const *s2);

#endif
