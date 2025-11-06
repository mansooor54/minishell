/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+      */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/07 20:55:00 by malmarzo         ###   ########.fr       */
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

/* ===================== TOKENS ===================== */
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

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

/* ===================== PARSED NODES ===================== */
typedef struct s_redir
{
	t_token_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd	*next;
	int				expanded; /* 0/1 guard to avoid double expansion */
}	t_cmd;

typedef struct s_pipeline
{
	t_cmd				*cmds;
	t_token_type		logic_op;
	struct s_pipeline	*next;
}	t_pipeline;

/* ===================== ENV ===================== */
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

/* ===================== SHELL STATE ===================== */
typedef struct s_shell
{
	t_env	*env;
	int		exit_status;
	int		should_exit;
}	t_shell;

extern t_shell	g_shell;

/* ===================== EXECUTION AUX TYPES ===================== */
typedef struct s_child_io
{
	int	prev_rd;
	int	pipe_rd;
	int	pipe_wr;
	int	has_next;
}	t_child_io;

typedef struct s_pipe_ctx
{
	t_shell	*shell;
	pid_t	*pids;
	int		*prev_rd;
}	t_pipe_ctx;

/* ===================== EXPANSION CTX ===================== */
typedef struct s_exp_ctx
{
	char	*str;
	char	*result;
	int		*i;
	int		*j;
	char	in_quote;
	t_env	*env;
	int		exit_status;
}	t_exp_ctx;

typedef struct s_quote_ctx
{
	int		i;
	int		j;
	char	quote;
	char	*str;
	char	*res;
}	t_quote_ctx;

/* ===================== LEXER ===================== */
/* Tokenize input string into a linked list of tokens */
t_token		*lexer(char *input);
t_token		*create_token(t_token_type type, char *value);
void		add_token(t_token **tokens, t_token *new_token);
void		free_tokens(t_token *tokens);
int			has_unclosed_quotes(char *str);
t_token		*get_operator_token(char **input);

/* ===================== PARSER ===================== */
t_pipeline	*parser(t_token *tokens);
t_cmd		*parse_command(t_token **tokens);
t_redir		*parse_redirections(t_token **tokens);
void		append_redir(t_redir **head, t_redir *new_redir);
void		free_pipeline(t_pipeline *pipeline);

/* ===================== EXPANDER ===================== */
char		*get_env_value(t_env *env, char *key);
char		*expand_variables(char *str, t_env *env, int exit_status);
void		expander(t_pipeline *pipeline, t_env *env);
void		expand_exit_status(char *result, int *j, int exit_status);
void		expand_var_name(t_exp_ctx *ctx);
void		process_dollar(t_exp_ctx *ctx);
void		expand_arg(char **arg, t_env *env, int exit_status);
void		expand_cmd_args(t_cmd *cmd, t_env *env, int exit_status);
void		expand_redirections(t_redir *redir, t_env *env, int exit_status);
void		expand_pipeline_cmds(t_cmd *cmds, t_env *env, int exit_status);
char		*remove_quotes(char *str);

/* ===================== EXECUTOR ===================== */
void		executor(t_pipeline *pipeline, t_shell *shell);
int			init_pipeline(int cmd_count, pid_t **pids);
int			create_pipe_if_needed(t_cmd *current, int pipefd[2]);
int			count_commands(t_cmd *cmds);
int			wait_for_children(pid_t *pids, int count);
int			execute_pipeline_loop(t_cmd *cmds, t_shell *shell,
				pid_t *pids, int cmd_count);
int			execute_single_builtin_parent(t_cmd *cmd, t_shell *shell);
int			execute_pipeline(t_cmd *cmds, t_shell *shell);
void		execute_command(t_cmd *cmd, t_shell *shell);
int			setup_redirections(t_redir *redirs);
int			setup_child_fds(int pipefd[2], int prev_read_fd, int has_next);
char		*find_executable(char *cmd, t_env *env);
void		print_error(const char *function, const char *message);
void		safe_close(int fd);
void		cleanup_pipe(int pipefd[2]);
pid_t		create_child_process(t_cmd *cmd, t_shell *shell, t_child_io *io);
int			update_prev_fd(int prev_read_fd, int pipefd[2], int has_next);
int			execute_one_command(t_cmd *cmd, int index, t_pipe_ctx *ctx);

/* ===================== BUILTINS ===================== */
int			is_builtin(char *cmd);
int			execute_builtin(t_cmd *cmd, t_shell *shell);
int			builtin_echo(char **args);
int			builtin_cd(char **args, t_env **env);
char		*dup_cwd(void);
char		*resolve_target(char **args, t_env *env, int *print_after);
int			builtin_pwd(void);
int			builtin_export(char **args, t_env **env);
int			builtin_unset(char **args, t_env **env);
int			builtin_env(t_env *env);
int			builtin_exit(char **args, t_shell *shell);

/* ===================== ENV MANAGEMENT ===================== */
t_env		*init_env(char **envp);
void		increment_shlvl(t_env **env);
t_env		*create_env_node(char *key, char *value);
void		add_env_node(t_env **env, t_env *new_node);
void		remove_env_node(t_env **env, char *key);
char		**env_to_array(t_env *env);
void		free_env(t_env *env);
int			append_env(char ***arr, size_t *n, const char *k, const char *v);
void		parse_env_string(char *env_str, char **key, char **value);

/* ===================== SIGNALS ===================== */
void		setup_signals(void);
void		handle_sigint(int sig);
void		handle_sigquit(int sig);

/* ===================== UTILS ===================== */
void		free_array(char **arr);
char		*ft_strjoin_free(char *s1, char const *s2);
void		print_logo(void);

/* ===================== CORE ===================== */
void		init_shell(t_shell *shell, char **envp);
void		shell_loop(t_shell *shell);

#endif
