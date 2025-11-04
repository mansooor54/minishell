/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_helpers.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2024/11/04 00:00:00 by your_login       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                    HELPER FUNCTIONS FOR execute_pipeline_v2
** ============================================================================
**
** This file contains the required helper functions that execute_pipeline_v2
** depends on. You may already have some of these implemented in your project.
**
** If you already have these functions, you don't need this file.
** Otherwise, use these as reference implementations.
**
** ============================================================================
*/

#include "minishell.h"

/*
** find_executable - Find executable file in PATH or validate path
**
** Searches for an executable in PATH directories, or validates
** absolute/relative paths.
**
** @param cmd: Command name or path
** @param env: Environment variables array
**
** Return: Allocated path to executable, or NULL if not found
**
** Note: Caller must free returned string
*/
char	*find_executable(char *cmd, char **env)
{
	char	*path_env;
	char	**path_dirs;
	char	*full_path;
	int		i;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_env = get_env_value("PATH", env);
	if (!path_env)
		return (NULL);
	path_dirs = ft_split(path_env, ':');
	if (!path_dirs)
		return (NULL);
	i = 0;
	while (path_dirs[i])
	{
		full_path = build_path(path_dirs[i], cmd);
		if (full_path && access(full_path, X_OK) == 0)
		{
			free_array(path_dirs);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_array(path_dirs);
	return (NULL);
}

/*
** get_env_value - Get value of environment variable
**
** @param key: Environment variable name
** @param env: Environment array
**
** Return: Value string, or NULL if not found
*/
char	*get_env_value(char *key, char **env)
{
	int		i;
	int		key_len;

	if (!key || !env)
		return (NULL);
	key_len = ft_strlen(key);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], key, key_len) == 0
			&& env[i][key_len] == '=')
			return (env[i] + key_len + 1);
		i++;
	}
	return (NULL);
}

/*
** build_path - Join directory and filename with '/'
**
** @param dir: Directory path
** @param file: Filename
**
** Return: Allocated full path, or NULL on error
*/
char	*build_path(char *dir, char *file)
{
	char	*path;
	char	*temp;

	if (!dir || !file)
		return (NULL);
	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	path = ft_strjoin(temp, file);
	free(temp);
	return (path);
}

/*
** free_array - Free NULL-terminated string array
**
** @param array: Array to free
*/
void	free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

/*
** setup_redirections - Set up all redirections for a command
**
** Opens files and redirects file descriptors according to
** redirection list.
**
** @param redirs: Linked list of redirections
**
** Return: 0 on success, -1 on error
*/
int	setup_redirections(t_redir *redirs)
{
	t_redir	*current;

	current = redirs;
	while (current)
	{
		if (current->type == REDIR_IN)
		{
			if (handle_input_redir(current->file) == -1)
				return (-1);
		}
		else if (current->type == REDIR_OUT)
		{
			if (handle_output_redir(current->file, 0) == -1)
				return (-1);
		}
		else if (current->type == REDIR_APPEND)
		{
			if (handle_output_redir(current->file, 1) == -1)
				return (-1);
		}
		else if (current->type == REDIR_HEREDOC)
		{
			if (handle_heredoc_redir(current->file) == -1)
				return (-1);
		}
		current = current->next;
	}
	return (0);
}

/*
** handle_input_redir - Handle input redirection (<)
**
** @param file: Input filename
**
** Return: 0 on success, -1 on error
*/
int	handle_input_redir(char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(file, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

/*
** handle_output_redir - Handle output redirection (> or >>)
**
** @param file: Output filename
** @param append: 1 for append mode (>>), 0 for truncate (>)
**
** Return: 0 on success, -1 on error
*/
int	handle_output_redir(char *file, int append)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(file, flags, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(file, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

/*
** handle_heredoc_redir - Handle heredoc redirection (<<)
**
** Note: This is a simplified version. Full implementation should
** read lines until delimiter and store in temp file or pipe.
**
** @param delimiter: Heredoc delimiter
**
** Return: 0 on success, -1 on error
*/
int	handle_heredoc_redir(char *delimiter)
{
	int		pipefd[2];
	char	*line;

	if (pipe(pipefd) == -1)
		return (-1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		close(pipefd[0]);
		return (-1);
	}
	close(pipefd[0]);
	return (0);
}

/*
** is_builtin - Check if command is a builtin
**
** @param cmd: Command name
**
** Return: 1 if builtin, 0 otherwise
*/
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

/*
** execute_builtin - Execute builtin command
**
** @param args: NULL-terminated argument array
** @param shell: Shell state
**
** Return: Exit status of builtin
*/
int	execute_builtin(char **args, t_shell *shell)
{
	if (!args || !args[0])
		return (0);
	if (ft_strcmp(args[0], "echo") == 0)
		return (builtin_echo(args));
	if (ft_strcmp(args[0], "cd") == 0)
		return (builtin_cd(args, shell));
	if (ft_strcmp(args[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(args[0], "export") == 0)
		return (builtin_export(args, shell));
	if (ft_strcmp(args[0], "unset") == 0)
		return (builtin_unset(args, shell));
	if (ft_strcmp(args[0], "env") == 0)
		return (builtin_env(shell->env));
	if (ft_strcmp(args[0], "exit") == 0)
		return (builtin_exit(args, shell));
	return (0);
}

/*
** ============================================================================
**                         ADDITIONAL NOTES
** ============================================================================
**
** The above helper functions assume you have the following libft functions:
**   - ft_strchr
**   - ft_strdup
**   - ft_split
**   - ft_strlen
**   - ft_strncmp
**   - ft_strcmp
**   - ft_strjoin
**   - ft_putstr_fd
**
** And the following builtin implementations:
**   - builtin_echo
**   - builtin_cd
**   - builtin_pwd
**   - builtin_export
**   - builtin_unset
**   - builtin_env
**   - builtin_exit
**
** If you don't have these, you'll need to implement them.
**
** ============================================================================
*/
