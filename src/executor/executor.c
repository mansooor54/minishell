/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 11:56:09 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	execute_external(t_cmd *cmd, t_shell *shell)
{
	struct stat	st;
	char		*path;
	pid_t		pid;
	int			status;

	if (!cmd || !cmd->args || !cmd->args[0])
		return ;

	/* Case 1: direct path given (contains /) */
	if (ft_strchr(cmd->args[0], '/'))
	{
		/* directory check first */
		if (stat(cmd->args[0], &st) == 0 && S_ISDIR(st.st_mode))
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->args[0], 2);
			ft_putendl_fd(": is a directory", 2);
			shell->exit_status = 126;
			return ;
		}
	}
	/* Case 2: normal PATH lookup */
	path = find_executable(cmd->args[0], shell->env);
	if (!path)
	{
		cmd_not_found(cmd->args[0]);
		shell->exit_status = 127;
		return ;
	}

	pid = fork();
	if (pid == -1)
	{
		print_error("fork", strerror(errno));
		free(path);
		shell->exit_status = 127;
		return ;
	}
	if (pid == 0)
	{
		if (setup_redirections(cmd->redirs) == -1)
			exit(1);
		execve(path, cmd->args, env_to_array(shell->env));
		if (errno == EACCES)
			exit(126);
		else
			exit(127);
	}
	free(path);
/* PARENT: read child status and save it */
	status = 0;
	if (waitpid(pid, &status, 0) == -1)
	{
		print_error("waitpid", strerror(errno));
		shell->exit_status = 1;
	}
	else
	{
		if (WIFEXITED(status))
			shell->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			shell->exit_status = 128 + WTERMSIG(status);
	}
}

static void	execute_builtin_with_redir(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
	{
		if (setup_redirections(cmd->redirs) == -1)
			exit(1);
		exit(execute_builtin(cmd, shell));
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		shell->exit_status = WEXITSTATUS(status);
}

/* stderr helpers you likely already have; keep your own if present */
static void put2(const char *s) { if (s) write(2, s, (int)ft_strlen(s)); }

static void perror_with_cmd(const char *cmd)
{
	put2("minishell: ");
	put2(cmd);
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


void	execute_commands(t_cmd *cmd, t_shell *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (is_builtin(cmd->args[0]))
	{
		if (cmd->redirs)
			execute_builtin_with_redir(cmd, shell);
		else
			shell->exit_status = execute_builtin(cmd, shell);
	}
	else
		execute_external(cmd, shell);
}

void execute_command(char **argv, t_shell *shell, char **envp)
{
	char *path;

	if (!argv || !argv[0] || !argv[0][0])
	{
		shell->exit_status = 0;
		return;
	}

	/* resolve path: direct if contains '/', else search PATH */
	path = NULL;
	if (has_slash(argv[0]) == 1)
		path = ft_strdup(argv[0]);
	else
		path = find_executable(argv[0], shell->env);

	/* not found -> 127 */
	if (!path)
	{
		cmd_not_found(argv[0]);
		shell->exit_status = 127;
		return;
	}

	/* directory -> 126 (e.g., //) */
	if (is_directory(path) == 1)
	{
		put2("minishell: ");
		put2(argv[0]);
		put2(": is a directory\n");
		free(path);
		shell->exit_status = 126;
		return;
	}

	/* permission / existence checks before execve */
	if (access(path, X_OK) == -1)
	{
		if (errno == EACCES)
		{
			perror_with_cmd(argv[0]);   /* “Permission denied” */
			free(path);
			shell->exit_status = 126;   /* found but not executable */
			return;
		}
		/* any other reason (e.g., removed between resolve and access) */
		perror_with_cmd(argv[0]);
		free(path);
		shell->exit_status = 127;
		return;
	}

	/* try to execute */
	execve(path, argv, envp);

	/* execve only returns on error */
	if (errno == EACCES)
		shell->exit_status = 126;
	else
		shell->exit_status = 127;
	perror_with_cmd(argv[0]);
	free(path);
}
