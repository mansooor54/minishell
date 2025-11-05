/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 13:31:53 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** find_executable - Locate executable file in PATH or validate path
**
** This function searches for an executable command in the system PATH or
** validates an absolute/relative path. It implements the following logic:
**
** 1. If the command contains a '/' character, it's treated as an absolute
**    or relative path and returned as-is (after duplication)
** 2. Otherwise, it searches through all directories in the PATH environment
**    variable to find an executable file with the given name
** 3. Uses access() with X_OK to verify execute permissions
**
** The function allocates memory for the full path which must be freed by
** the caller.
**
** @param cmd: The command name to search for (e.g., "ls" or "/bin/ls")
** @param env: Linked list of environment variables containing PATH
**
** Return: Allocated string with full path to executable, or NULL if not found
**         - NULL if cmd is NULL or empty
**         - Duplicated cmd if it contains '/'
**         - Full path from PATH if found and executable
**         - NULL if PATH doesn't exist or command not found
*/


char	*find_executable(char *cmd, t_env *env)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin_free(full_path, cmd);
		if (access(full_path, X_OK) == 0)
		{
			free_array(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_array(paths);
	return (NULL);
}

/*
** execute_external - Execute an external (non-builtin) command
**
** This function handles the execution of external commands by:
** 1. Finding the executable path using find_executable()
** 2. Forking a child process
** 3. Setting up redirections in the child process
** 4. Converting environment list to array format for execve()
** 5. Executing the command with execve()
** 6. Waiting for the child process and capturing exit status
**
** Error handling:
** - If command is not found, prints error message and sets exit status to 127
** - If redirections fail, child exits with status 1
** - If execve fails, child exits with status 1
**
** The parent process waits for the child and updates the shell's exit_status
** based on the child's exit code.
**
** @param cmd: Command structure containing args and redirections
** @param shell: Shell state structure containing environment and exit status
**
** Return: void (updates shell->exit_status)
*/
static void	child_exec_external(t_cmd *cmd, t_shell *shell, char *path)
{
	if (setup_redirections(cmd->redirs) == -1)
		exit(1);
	execve(path, cmd->args, env_to_array(shell->env));
	exit(1);
}

static void	wait_and_set_status(pid_t pid, t_shell *shell)
{
	int	st;

	waitpid(pid, &st, 0);
	if (WIFEXITED(st))
		shell->exit_status = WEXITSTATUS(st);
}

static void	execute_external(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	pid_t	pid;

	path = find_executable(cmd->args[0], shell->env);

	if (!path)
		return (ft_putstr_fd("minishell: ", 2),
			ft_putstr_fd(cmd->args[0], 2),
			ft_putendl_fd(": command not found", 2),
			(void)(shell->exit_status = 127));
	pid = fork();
	if (pid == 0)
		child_exec_external(cmd, shell, path);
	wait_and_set_status(pid, shell);
	free(path);
}

/*
** execute_builtin_with_redir - Execute builtin command with redirections
**
** This function handles the special case of executing a builtin command
** that has redirections. Since redirections modify file descriptors and
** we don't want to affect the parent shell's file descriptors, we:
**
** 1. Fork a child process
** 2. Set up redirections in the child (which modifies stdin/stdout)
** 3. Execute the builtin in the child
** 4. Exit the child with the builtin's return value
** 5. Wait for the child and capture the exit status in the parent
**
** This approach ensures that redirections only affect the builtin execution
** and not the parent shell's state.
**
** Example: "echo hello > file.txt" should redirect only for this command,
** not affect subsequent commands in the shell.
**
** @param cmd: Command structure with builtin name, args, and redirections
** @param shell: Shell state structure
**
** Return: void (updates shell->exit_status)
*/
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

/*
** execute_command - Execute a single command (builtin or external)
**
** This is the main command execution dispatcher that determines whether
** a command is a builtin or external command and routes it accordingly.
**
** Execution flow:
** 1. Validate that command exists and has arguments
** 2. Check if command is a builtin using is_builtin()
** 3. For builtins:
**    - If redirections exist, fork and execute in child (to preserve parent FDs)
**    - If no redirections, execute directly in parent (e.g., cd, export)
** 4. For external commands:
**    - Always fork and execute using execve()
**
** The distinction between builtin execution with/without redirections is
** important because some builtins (like cd, export, unset) need to modify
** the parent shell's state, which wouldn't work if executed in a child process.
**
** @param cmd: Command structure containing:
**      - args: NULL-terminated array of arguments (args[0] is command name)
**      - redirs: Linked list of redirections (can be NULL)
** @param shell: Shell state structure containing environment and exit status
**
** Return: void (updates shell->exit_status)
*/
void	execute_command(t_cmd *cmd, t_shell *shell)
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
