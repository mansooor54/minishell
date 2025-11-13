/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   merged_minishell_ifelse.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ChatGPT                                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 20:00:00 by ChatGPT           #+#    #+#             */
/*   Updated: 2025/11/13 20:00:00 by ChatGPT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/* ===================== small helpers (if/else only) ===================== */
static void put2(const char *s)
{
	if (s)
		write(2, s, (int)ft_strlen(s));
}

static void perror_with_cmd(const char *cmd)
{
	put2("minishell: ");
	if (cmd)
		put2(cmd);
	else
		put2("");
	put2(": ");
	put2(strerror(errno));
	put2("\n");
}

static int is_directory(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return 0;
	if (S_ISDIR(st.st_mode))
		return 1;
	else
		return 0;
}

static int has_slash(const char *s)
{
	int i;
	if (!s)
		return 0;
	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == '/')
			return 1;
		i++;
	}
	return 0;
}

static int is_exec_file(const char *full)
{
	struct stat st;
	if (!full)
		return 0;
	if (stat(full, &st) == -1)
		return 0;
	if (S_ISREG(st.st_mode))
	{
		if (access(full, X_OK) == 0)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

static char *join_cmd_path(const char *dir, const char *cmd)
{
	char *tmp;
	char *full;
	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return NULL;
	full = ft_strjoin(tmp, cmd);
	free(tmp);
	if (!full)
		return NULL;
	return full;
}

/* ===================== PATH resolution (if/else only) ===================== */
static size_t seg_end(const char *path, size_t start)
{
	size_t i;
	i = start;
	while (path[i] && path[i] != ':')
		i++;
	return i;
}

static char *dup_segment_or_dot(const char *path, size_t start, size_t end)
{
	if (end == start)
		return ft_strdup(".");
	else
		return ft_substr(path, start, end - start);
}

static char *probe_dir_for_cmd(const char *dir, const char *cmd)
{
	char *full;
	full = join_cmd_path(dir, cmd);
	if (!full)
		return NULL;
	if (is_exec_file(full))
		return full;
	else
	{
		free(full);
		return NULL;
	}
}

static char *search_in_path(const char *path, const char *cmd)
{
	size_t i;
	size_t j;
	char *dir;
	char *hit;

	if (!path)
		return NULL;
	i = 0;
	while (1)
	{
		j = seg_end(path, i);
		dir = dup_segment_or_dot(path, i, j);
		if (!dir)
			return NULL;
		hit = probe_dir_for_cmd(dir, cmd);
		free(dir);
		if (hit)
			return hit;
		else
		{
			if (!path[j])
				break ;
			i = j + 1;
		}
	}
	return NULL;
}

char *find_executable(char *cmd, t_env *env)
{
	char *path;
	if (!cmd)
		return NULL;
	if (!*cmd)
		return NULL;
	if (ft_strchr(cmd, '/'))
	{
		if (is_exec_file(cmd))
			return ft_strdup(cmd);
		else
			return NULL;
	}
	else
	{
		path = get_env_value(env, "PATH");
		return search_in_path(path, cmd);
	}
}

/* ===================== Child exec (if/else only) ===================== */
static void execute_external_child(t_cmd *cmd, t_shell *shell, char *path)
{
	char **envp;
	(void)shell;
	envp = env_to_array(shell->env);
	if (!envp)
	{
		print_error("env_to_array", "allocation failed");
		free(path);
		exit(1);
	}
	execve(path, cmd->args, envp);
	free_array(envp);
	free(path);
	print_error("execve", strerror(errno));
	exit(126);
}

static void execute_builtin_child(t_cmd *cmd, t_shell *shell)
{
	int exit_code;
	exit_code = execute_builtin(cmd, shell);
	exit(exit_code);
}

static void execute_cmd_child(t_cmd *cmd, t_shell *shell)
{
	char *path;
	if (!cmd)
		exit(0);
	else
	{
		if (!cmd->args)
			exit(0);
		else
		{
			if (!cmd->args[0])
				exit(0);
			else
			{
				if (setup_redirections(cmd->redirs) == -1)
					exit(1);
				else
				{
					if (is_builtin(cmd->args[0]))
						execute_builtin_child(cmd, shell);
					else
					{
						path = find_executable(cmd->args[0], shell->env);
						if (!path)
						{
							cmd_not_found(cmd->args[0]);
							exit(127);
						}
						else
							execute_external_child(cmd, shell, path);
					}
				}
			}
		}
	}
}

/* ===================== Pipes / FDs (if/else only) ===================== */
static int setup_input_fd(int prev_read_fd)
{
	if (prev_read_fd >= 0)
	{
		if (dup2(prev_read_fd, STDIN_FILENO) == -1)
		{
			print_error("dup2", "failed to redirect stdin");
			return -1;
		}
		safe_close(prev_read_fd);
		return 0;
	}
	else
		return 0;
}

static int setup_output_fd(int pipefd[2], int has_next)
{
	if (has_next)
	{
		safe_close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			print_error("dup2", "failed to redirect stdout");
			return -1;
		}
		safe_close(pipefd[1]);
		return 0;
	}
	else
		return 0;
}

int setup_child_fds(int pipefd[2], int prev_read_fd, int has_next)
{
	if (setup_input_fd(prev_read_fd) == -1)
		return -1;
	else
	{
		if (setup_output_fd(pipefd, has_next) == -1)
			return -1;
		else
			return 0;
	}
}

pid_t create_child_process(t_cmd *cmd, t_shell *shell, t_child_io *io)
{
	pid_t pid;
	int   pipefd[2];

	pid = fork();
	if (pid == -1)
		return (print_error("fork", strerror(errno)), -1);
	else
	{
		if (pid == 0)
		{
			pipefd[0] = io->pipe_rd;
			pipefd[1] = io->pipe_wr;
			if (setup_child_fds(pipefd, io->prev_rd, io->has_next) == -1)
				exit(1);
			else
				execute_cmd_child(cmd, shell);
			exit(1);
		}
		else
			return pid;
	}
}

int update_prev_fd(int prev_read_fd, int pipefd[2], int has_next)
{
	safe_close(prev_read_fd);
	if (has_next)
	{
		safe_close(pipefd[1]);
		return pipefd[0];
	}
	else
		return -1;
}

/* ===================== Top-level execute_command (parent-side helper) === */
void execute_command(char **argv, t_shell *shell, char **envp)
{
	char *path;
	if (!argv)
	{
		shell->exit_status = 0;
		return;
	}
	else if (!argv[0])
	{
		shell->exit_status = 0;
		return;
	}
	else if (!argv[0][0])
	{
		shell->exit_status = 0;
		return;
	}
	else
	{
		path = NULL;
		if (has_slash(argv[0]) == 1)
			path = ft_strdup(argv[0]);
		else
			path = find_executable(argv[0], shell->env);

		if (!path)
		{
			cmd_not_found(argv[0]);
			shell->exit_status = 127;
			return;
		}
		else
		{
			if (is_directory(path) == 1)
			{
				put2("minishell: ");
				put2(argv[0]);
				put2(": is a directory\n");
				free(path);
				shell->exit_status = 126;
				return;
			}
			else
			{
				if (access(path, X_OK) == -1)
				{
					if (errno == EACCES)
					{
						perror_with_cmd(argv[0]);
						free(path);
						shell->exit_status = 126;
						return;
					}
					else
					{
						perror_with_cmd(argv[0]);
						free(path);
						shell->exit_status = 127;
						return;
					}
				}
				else
				{
					execve(path, argv, envp);
					if (errno == EACCES)
						shell->exit_status = 126;
					else
						shell->exit_status = 127;
					perror_with_cmd(argv[0]);
					free(path);
				}
			}
		}
	}
}

/* ===================== Pipeline / Executor (if/else only) ============== */
int init_pipeline(int cmd_count, pid_t **pids)
{
	*pids = (pid_t *)malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		print_error("malloc", "failed to allocate pid array");
		return -1;
	}
	else
		return 0;
}

void executor(t_pipeline *pipeline, t_shell *shell)
{
	while (pipeline)
	{
		shell->exit_status = execute_pipeline(pipeline->cmds, shell);
		if (pipeline->logic_op == TOKEN_AND)
		{
			if (shell->exit_status != 0)
				break ;
		}
		else if (pipeline->logic_op == TOKEN_OR)
		{
			if (shell->exit_status == 0)
				break ;
		}
		pipeline = pipeline->next;
	}
}

/* ===================== Syntax error printers (if/else only) ============ */
void print_run_error(t_token *t)
{
	int total;
	t_token *cur;

	total = 0;
	cur = t;
	if (is_gt(t))
	{
		while (is_gt(cur))
		{
			total += tok_op_len(cur);
			cur = cur->next;
		}
		if (total > 3)
			ft_putendl_fd(ERR_REDIR_APPEND, 2);
		else if (total == 2)
		{
			if (!cur)
				ft_putendl_fd(ERR_NEWLINE, 2);
			else if (is_control_operator(cur) || is_redirection(cur))
				ft_putendl_fd(ERR_NEWLINE, 2);
			else
				ft_putendl_fd(ERR_CONSECUTIVE_REDIR, 2);
		}
		else
			ft_putendl_fd(ERR_CONSECUTIVE_REDIR, 2);
	}
	else if (is_lt(t))
	{
		int total_lt = 0;
		cur = t;
		while (is_lt(cur))
		{
			total_lt += tok_op_len(cur);
			cur = cur->next;
		}
		if (total_lt <= 3)
			print_unexpected("newline");
		else if (total_lt == 4)
			print_unexpected("<");
		else if (total_lt == 5)
			print_unexpected("<<");
		else
			print_unexpected("<<<");
	}
}

void print_syntax_error(t_token *token)
{
	if (!token)
		print_unexpected("newline");
	else if (token->type == TOKEN_SEMI)
		ft_putendl_fd(ERR_SEMI, 2);
	else if (is_redirection(token) && is_redirection(token->next))
		print_run_error(token);
	else if (token->type == TOKEN_PIPE)
		ft_putendl_fd(ERR_PIPE, 2);
	else if (token->type == TOKEN_AND)
		ft_putendl_fd(ERR_AND, 2);
	else if (token->type == TOKEN_OR)
		ft_putendl_fd(ERR_OR, 2);
	else if (token->type == TOKEN_REDIR_IN)
		ft_putendl_fd(ERR_REDIR_IN, 2);
	else if (token->type == TOKEN_REDIR_OUT)
		ft_putendl_fd(ERR_REDIR_OUT, 2);
	else if (token->type == TOKEN_REDIR_APPEND)
		ft_putendl_fd(ERR_REDIR_APPEND, 2);
}

/* ===================== End ============================================ */
