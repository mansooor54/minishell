/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 14:28:41 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* System includes */
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <signal.h>
# include <termios.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"

/* Error messages */
# define ERR_NEWLINE "minishell: syntax error near unexpected token `newline'"
# define ERR_PIPE "minishell: syntax error near unexpected token `|'"
# define ERR_AND "minishell: syntax error near unexpected token `&&'"
# define ERR_OR "minishell: syntax error near unexpected token `||'"
# define ERR_REDIR_IN "minishell: syntax error near unexpected token `<'"
# define ERR_REDIR_OUT "minishell: syntax error near unexpected token `>'"
# define ERR_REDIR_APPEND "minishell: syntax error near unexpected token `>>'"
# define ERR_REDIR_HEREDOC "minishell: syntax error near unexpected token `<<'"
# define ERR_TRIPLE_LT "minishell: syntax error near unexpected token `<<<'"
# define ERR_MULTIPLE_REDIR_IN \
	"minishell: syntax error near unexpected token `<<<'"
# define ERR_MULTIPLE_REDIR_OUT \
	"minishell: syntax error near unexpected token `>>>'"
# define ERR_CONSECUTIVE_REDIR \
	"minishell: syntax error near unexpected token `>'"

/* ===================== DATA STRUCTURES ===================== */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_HEREDOC,
	TOKEN_EOF
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

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
	int				expanded;
}	t_cmd;

typedef struct s_pipeline
{
	t_cmd				*cmds;
	t_token_type		logic_op;
	struct s_pipeline	*next;
}	t_pipeline;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_shell
{
	t_env	*env;
	int		exit_status;
	int		should_exit;
	int		sigint_during_read;
	char	*history_path;
}	t_shell;

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

typedef struct s_exp_ctx
{
	char	*str;
	char	*result;
	int		i;
	int		j;
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

/* Global variable - required for signal handling and shell state */
t_shell	g_shell;

/* ===================== MAIN & CORE ===================== */
int		main(int argc, char **argv, char **envp);
void	shell_loop(t_shell *shell);
int		is_all_space(const char *s);
void	process_line(char *line, t_shell *shell);
char	*read_logical_line(void);
int		needs_continuation(const char *s);

/* ===================== LEXER ===================== */
t_token	*lexer(char *input);
t_token	*create_token(t_token_type type, char *value);
void	add_token(t_token **tokens, t_token *new_token);
void	free_tokens(t_token *tokens);
int		has_unclosed_quotes(char *str);
int		is_whitespace(char c);
int		is_operator(char c);
int		extract_word(char *input, char **word);
t_token	*get_operator_token(char **input);
t_token	*try_or_pipe(char **input);
t_token	*try_and(char **input);
t_token	*try_inredir(char **input);
t_token	*try_outredir(char **input);

/* ===================== PARSER ===================== */
t_pipeline	*parser(t_token *tokens);
t_cmd		*parse_command(t_token **tokens);
t_redir		*create_redir(t_token_type type, char *file);
void		append_redir(t_redir **head, t_redir *new_redir);
void		free_pipeline(t_pipeline *pipeline);
int			validate_syntax(t_token *tokens, t_shell *shell);
void		print_syntax_error(t_token *token);
void		print_run_error(t_token *t);
void		print_unexpected(char *s);
int			is_valid_word(t_token *token);
int			is_control_operator(t_token *token);
int			is_redirection(t_token *token);
int			is_gt(t_token *t);
int			is_lt(t_token *t);
int			tok_op_len(t_token *t);

/* ===================== EXPANDER ===================== */
void	expander(t_pipeline *pipeline, t_env *env);
char	*expand_variables(char *str, t_env *env, int exit_status);
void	expand_arg(char **arg, t_env *env, int exit_status);
void	expand_cmd_args(t_cmd *cmd, t_env *env, int exit_status);
void	expand_redirections(t_redir *redir, t_env *env, int exit_status);
void	expand_pipeline_cmds(t_cmd *cmds, t_env *env, int exit_status);
char	*get_env_value(t_env *env, char *key);
void	expand_exit_status(char *result, int *j, int exit_status);
void	expand_var_name(t_exp_ctx *ctx);
void	process_dollar(t_exp_ctx *c);
char	*remove_quotes(char *str);

/* ===================== EXECUTOR ===================== */
void	executor(t_pipeline *pipeline, t_shell *shell);
int		execute_pipeline(t_cmd *cmds, t_shell *shell);
int		execute_pipeline_loop(t_cmd *cmds, t_shell *shell,
			pid_t *pids, int cmd_count);
int		execute_single_builtin_parent(t_cmd *cmd, t_shell *shell);
int		wait_for_children(pid_t *pids, int count);
int		execute_one_command(t_cmd *cmd, int index, t_pipe_ctx *ctx);
int		init_pipeline(int cmd_count, pid_t **pids);
int		count_commands(t_cmd *cmds);
void	print_error(const char *function, const char *message);
void	safe_close(int fd);
void	cleanup_pipe(int pipefd[2]);
void	execute_commands(t_cmd *cmd, t_shell *shell);
void	perror_with_cmd(const char *cmd);
int		is_directory(const char *path);
int		has_slash(const char *s);
void	execute_external(t_cmd *cmd, t_shell *shell);
void	execute_command(char **argv, t_shell *shell, char **envp);
char	*resolve_command_path(char *cmd, t_shell *shell);
int		handle_path_errors(char *cmd, char *path, t_shell *shell);
int		check_execution_permission(char *cmd, char *path, t_shell *shell);
pid_t	create_child_process(t_cmd *cmd, t_shell *shell, t_child_io *io);
int		setup_child_fds(int pipefd[2], int prev_read_fd, int has_next);
int		setup_redirections(t_redir *redirs);
int		handle_heredoc(char *delimiter);
int		handle_input(char *file);
int		handle_output(char *file, int append);
char	*find_executable(char *cmd, t_env *env);
char	*search_in_path(const char *path, const char *cmd);
int		is_exec_file(const char *path);
char	*join_cmd_path(const char *dir, const char *cmd);
size_t	seg_end(const char *path, size_t start);
char	*dup_segment_or_dot(const char *path, size_t start, size_t end);
void	cmd_not_found(char *name);

/* ===================== BUILTINS ===================== */
int		is_builtin(char *cmd);
int		execute_builtin(t_cmd *cmd, t_shell *shell);
int		builtin_echo(char **args);
int		builtin_cd(char **args, t_env **env);
char	*dup_cwd(void);
char	*resolve_target(char **args, t_env *env, int *print_after);
int		builtin_pwd(void);
int		builtin_export(char **args, t_env **env);
int		builtin_unset(char **args, t_env **env);
int		builtin_env(t_env *env);
int		builtin_exit(char **args, t_shell *shell);
int		builtin_history(char **args);

/* ===================== ENVIRONMENT ===================== */
t_env	*init_env(char **envp);
void	increment_shlvl(t_env **env);
void	init_shell(t_shell *shell, char **envp);
t_env	*create_env_node(char *key, char *value);
void	add_env_node(t_env **env, t_env *new_node);
void	remove_env_node(t_env **env, char *key);
char	**env_to_array(t_env *env);
void	parse_env_string(char *env_str, char **key, char **value);
int		append_env(char ***arr, size_t *n, const char *k, const char *v);

/* ===================== SIGNALS ===================== */
void	setup_signals(void);
void	handle_sigint(int sig);
void	handle_sigquit(int sig);
void	init_terminal(void);

/* ===================== HISTORY ===================== */
void	history_add_line(const char *line);
int		history_init(t_shell *shell);
void	history_add_line(const char *line);
void	history_save(t_shell *shell);

/* ===================== UTILS ===================== */
void	free_array(char **arr);
char	*ft_strjoin_free(char *s1, char const *s2);
void	free_env(t_env *env);
void	print_logo(void);

#endif
