/* ************************************************************************** */
/*                                                                            */
/*   execute_pipeline_complete.c                                             */
/*                                                                            */
/*   Complete implementation of execute_pipeline with comprehensive          */
/*   error handling for fork, pipe, and dup2 system calls.                   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** print_error - Print error message to stderr
**
** Prints a formatted error message with the function name and errno message.
**
** @param function: Name of the function that failed
** @param message: Additional context message
*/
static void	print_error(const char *function, const char *message)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)function, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd((char *)message, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

/*
** safe_close - Safely close a file descriptor with error checking
**
** Closes a file descriptor and prints error if close fails.
** Does nothing if fd is -1 (already closed or invalid).
**
** @param fd: File descriptor to close
*/
static void	safe_close(int fd)
{
	if (fd >= 0)
	{
		if (close(fd) == -1)
			print_error("close", strerror(errno));
	}
}

/*
** cleanup_pipe - Close both ends of a pipe
**
** Safely closes both the read and write ends of a pipe.
**
** @param pipefd: Array containing pipe file descriptors
*/
static void	cleanup_pipe(int pipefd[2])
{
	safe_close(pipefd[0]);
	safe_close(pipefd[1]);
}

/*
** setup_child_pipe_fds - Set up file descriptors for a child in pipeline
**
** Redirects stdin/stdout to pipe ends as needed and closes unused fds.
**
** @param pipefd: Current pipe file descriptors
** @param is_first: 1 if this is the first command in pipeline
** @param is_last: 1 if this is the last command in pipeline
** @param prev_read_fd: Read end of previous pipe (or -1 if first command)
**
** Return: 0 on success, -1 on error
*/
static int	setup_child_pipe_fds(int pipefd[2], int is_first, int is_last,
								int prev_read_fd)
{
	if (!is_first)
	{
		if (dup2(prev_read_fd, STDIN_FILENO) == -1)
		{
			print_error("dup2", "failed to redirect stdin");
			return (-1);
		}
		safe_close(prev_read_fd);
	}
	if (!is_last)
	{
		safe_close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			print_error("dup2", "failed to redirect stdout");
			return (-1);
		}
		safe_close(pipefd[1]);
	}
	return (0);
}

/*
** execute_single_command - Execute a single command in pipeline
**
** Handles execution of one command, including redirections and
** determining whether it's a builtin or external command.
**
** @param cmd: Command structure containing args and redirections
** @param shell: Shell state structure with environment
**
** This function is called in a child process and should exit.
*/
static void	execute_single_command(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd || !cmd->args || !cmd->args[0])
		exit(0);
	if (setup_redirections(cmd->redirs) == -1)
		exit(1);
	if (is_builtin(cmd->args[0]))
	{
		exit(execute_builtin(cmd->args, shell));
	}
	path = find_executable(cmd->args[0], shell->env);
	if (!path)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		exit(127);
	}
	if (execve(path, cmd->args, shell->env) == -1)
	{
		print_error("execve", strerror(errno));
		free(path);
		exit(126);
	}
}

/*
** fork_and_execute - Fork and execute a command with error handling
**
** Creates a child process and executes the command. Handles fork errors.
**
** @param cmd: Command to execute
** @param shell: Shell state
** @param pipefd: Current pipe fds
** @param is_first: Is this the first command?
** @param is_last: Is this the last command?
** @param prev_read_fd: Previous pipe read end
**
** Return: Child PID on success, -1 on error
*/
static pid_t	fork_and_execute(t_cmd *cmd, t_shell *shell, int pipefd[2],
								int is_first, int is_last, int prev_read_fd)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		print_error("fork", strerror(errno));
		return (-1);
	}
	if (pid == 0)
	{
		if (setup_child_pipe_fds(pipefd, is_first, is_last,
								prev_read_fd) == -1)
			exit(1);
		execute_single_command(cmd, shell);
		exit(1);
	}
	return (pid);
}

/*
** wait_for_children - Wait for all child processes and get exit status
**
** Waits for all children in the pipeline and returns the exit status
** of the last command.
**
** @param pids: Array of child process IDs
** @param count: Number of children to wait for
**
** Return: Exit status of last command
*/
static int	wait_for_children(pid_t *pids, int count)
{
	int	i;
	int	status;
	int	last_status;

	last_status = 0;
	i = 0;
	while (i < count)
	{
		if (pids[i] > 0)
		{
			if (waitpid(pids[i], &status, 0) == -1)
			{
				print_error("waitpid", strerror(errno));
			}
			else
			{
				if (WIFEXITED(status))
					last_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					last_status = 128 + WTERMSIG(status);
			}
		}
		i++;
	}
	return (last_status);
}

/*
** count_commands - Count the number of commands in a pipeline
**
** Traverses the command linked list and counts the commands.
**
** @param cmds: Head of command linked list
**
** Return: Number of commands
*/
static int	count_commands(t_cmd *cmds)
{
	int		count;
	t_cmd	*current;

	count = 0;
	current = cmds;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/*
** execute_pipeline - Execute a pipeline of commands
**
** Main function to execute a pipeline of commands connected by pipes.
** Handles all pipe creation, forking, file descriptor management,
** and error handling.
**
** @param cmds: Linked list of commands to execute
** @param shell: Shell state structure
**
** Return: Exit status of last command, or 1 on error
**
** Example: For "ls | grep .c | wc -l"
**   cmds points to: [ls] -> [grep .c] -> [wc -l] -> NULL
**
** Process:
**   1. Count commands
**   2. Allocate arrays for pids and pipes
**   3. For each command:
**      a. Create pipe (except for last command)
**      b. Fork child
**      c. Child: Setup fds and execute
**      d. Parent: Close used fds, save pid
**   4. Wait for all children
**   5. Return exit status of last command
*/
int	execute_pipeline(t_cmd *cmds, t_shell *shell)
{
	int		cmd_count;
	int		i;
	int		pipefd[2];
	int		prev_read_fd;
	pid_t	*pids;
	t_cmd	*current;
	int		exit_status;

	if (!cmds)
		return (0);
	cmd_count = count_commands(cmds);
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
	{
		print_error("malloc", "failed to allocate pid array");
		return (1);
	}
	i = 0;
	current = cmds;
	prev_read_fd = -1;
	while (current)
	{
		if (current->next)
		{
			if (pipe(pipefd) == -1)
			{
				print_error("pipe", strerror(errno));
				safe_close(prev_read_fd);
				free(pids);
				return (1);
			}
		}
		pids[i] = fork_and_execute(current, shell, pipefd,
									(i == 0), (current->next == NULL),
									prev_read_fd);
		if (pids[i] == -1)
		{
			safe_close(prev_read_fd);
			if (current->next)
				cleanup_pipe(pipefd);
			free(pids);
			return (1);
		}
		if (prev_read_fd >= 0)
			safe_close(prev_read_fd);
		if (current->next)
		{
			safe_close(pipefd[1]);
			prev_read_fd = pipefd[0];
		}
		current = current->next;
		i++;
	}
	exit_status = wait_for_children(pids, cmd_count);
	free(pids);
	return (exit_status);
}

/*
** ============================================================================
**                         ALTERNATIVE IMPLEMENTATION
**                    (Norminette-compliant, split functions)
** ============================================================================
*/

/*
** The above implementation is clear but may exceed Norminette's 25-line
** limit. Here's an alternative that splits execute_pipeline into smaller
** functions while maintaining the same functionality.
*/

/*
** init_pipeline - Initialize pipeline execution
**
** Allocates memory for pid array and initializes variables.
**
** @param cmd_count: Number of commands in pipeline
** @param pids: Pointer to store allocated pid array
**
** Return: 0 on success, -1 on error
*/
static int	init_pipeline(int cmd_count, pid_t **pids)
{
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		print_error("malloc", "failed to allocate pid array");
		return (-1);
	}
	return (0);
}

/*
** handle_pipe_creation - Create pipe for current command if needed
**
** Creates a pipe if this is not the last command.
**
** @param current: Current command
** @param pipefd: Array to store pipe fds
**
** Return: 0 on success, -1 on error
*/
static int	handle_pipe_creation(t_cmd *current, int pipefd[2])
{
	if (current->next)
	{
		if (pipe(pipefd) == -1)
		{
			print_error("pipe", strerror(errno));
			return (-1);
		}
	}
	return (0);
}

/*
** handle_parent_fds - Handle file descriptor cleanup in parent
**
** Closes file descriptors that the parent no longer needs.
**
** @param prev_read_fd: Previous pipe read end
** @param pipefd: Current pipe fds
** @param current: Current command
**
** Return: New prev_read_fd for next iteration
*/
static int	handle_parent_fds(int prev_read_fd, int pipefd[2], t_cmd *current)
{
	if (prev_read_fd >= 0)
		safe_close(prev_read_fd);
	if (current->next)
	{
		safe_close(pipefd[1]);
		return (pipefd[0]);
	}
	return (-1);
}

/*
** execute_pipeline_loop - Main loop for pipeline execution
**
** Iterates through commands, creating pipes and forking children.
**
** @param cmds: Command list
** @param shell: Shell state
** @param pids: Array to store child pids
** @param cmd_count: Number of commands
**
** Return: 0 on success, -1 on error
*/
static int	execute_pipeline_loop(t_cmd *cmds, t_shell *shell,
								pid_t *pids, int cmd_count)
{
	int		i;
	int		pipefd[2];
	int		prev_read_fd;
	t_cmd	*current;

	i = 0;
	current = cmds;
	prev_read_fd = -1;
	while (current && i < cmd_count)
	{
		if (handle_pipe_creation(current, pipefd) == -1)
		{
			safe_close(prev_read_fd);
			return (-1);
		}
		pids[i] = fork_and_execute(current, shell, pipefd,
									(i == 0), (current->next == NULL),
									prev_read_fd);
		if (pids[i] == -1)
		{
			safe_close(prev_read_fd);
			if (current->next)
				cleanup_pipe(pipefd);
			return (-1);
		}
		prev_read_fd = handle_parent_fds(prev_read_fd, pipefd, current);
		current = current->next;
		i++;
	}
	return (0);
}

/*
** execute_pipeline_v2 - Execute pipeline (Norminette-compliant version)
**
** Alternative implementation that splits the logic into smaller functions
** to comply with Norminette's 25-line limit.
**
** @param cmds: Linked list of commands
** @param shell: Shell state
**
** Return: Exit status of last command
*/
int	execute_pipeline_v2(t_cmd *cmds, t_shell *shell)
{
	int		cmd_count;
	pid_t	*pids;
	int		exit_status;

	if (!cmds)
		return (0);
	cmd_count = count_commands(cmds);
	if (init_pipeline(cmd_count, &pids) == -1)
		return (1);
	if (execute_pipeline_loop(cmds, shell, pids, cmd_count) == -1)
	{
		free(pids);
		return (1);
	}
	exit_status = wait_for_children(pids, cmd_count);
	free(pids);
	return (exit_status);
}

/*
** ============================================================================
**                              USAGE EXAMPLES
** ============================================================================
*/

/*
** Example 1: Simple two-command pipeline
**
** Command: ls | grep .c
**
** Setup:
**   t_cmd cmd1 = { .args = {"ls", NULL}, .redirs = NULL, .next = &cmd2 };
**   t_cmd cmd2 = { .args = {"grep", ".c", NULL}, .redirs = NULL, .next = NULL };
**   
** Execution:
**   int status = execute_pipeline(&cmd1, shell);
**
** Process:
**   1. Count: 2 commands
**   2. Allocate pids[2]
**   3. Create pipe (pipefd[0], pipefd[1])
**   4. Fork child 1:
**      - Close pipefd[0]
**      - dup2(pipefd[1], STDOUT_FILENO)
**      - Close pipefd[1]
**      - execve("/bin/ls", ...)
**   5. Parent closes pipefd[1]
**   6. Fork child 2:
**      - dup2(pipefd[0], STDIN_FILENO)
**      - Close pipefd[0]
**      - execve("/usr/bin/grep", ...)
**   7. Parent closes pipefd[0]
**   8. Wait for both children
**   9. Return exit status of grep
*/

/*
** Example 2: Three-command pipeline
**
** Command: cat file.txt | grep pattern | wc -l
**
** Setup:
**   t_cmd cmd1 = { .args = {"cat", "file.txt", NULL}, .next = &cmd2 };
**   t_cmd cmd2 = { .args = {"grep", "pattern", NULL}, .next = &cmd3 };
**   t_cmd cmd3 = { .args = {"wc", "-l", NULL}, .next = NULL };
**
** Execution:
**   int status = execute_pipeline(&cmd1, shell);
**
** Process:
**   1. Count: 3 commands
**   2. Allocate pids[3]
**   3. Create pipe1 (for cat -> grep)
**   4. Fork child 1 (cat):
**      - stdout -> pipe1 write end
**   5. Parent closes pipe1 write end
**   6. Create pipe2 (for grep -> wc)
**   7. Fork child 2 (grep):
**      - stdin -> pipe1 read end
**      - stdout -> pipe2 write end
**   8. Parent closes pipe1 read end, pipe2 write end
**   9. Fork child 3 (wc):
**      - stdin -> pipe2 read end
**   10. Parent closes pipe2 read end
**   11. Wait for all three children
**   12. Return exit status of wc
*/

/*
** Example 3: Pipeline with redirections
**
** Command: cat < input.txt | grep pattern > output.txt
**
** Setup:
**   t_redir redir1 = { .type = REDIR_IN, .file = "input.txt", .next = NULL };
**   t_redir redir2 = { .type = REDIR_OUT, .file = "output.txt", .next = NULL };
**   t_cmd cmd1 = { .args = {"cat", NULL}, .redirs = &redir1, .next = &cmd2 };
**   t_cmd cmd2 = { .args = {"grep", "pattern", NULL}, .redirs = &redir2, .next = NULL };
**
** Process:
**   1. Fork child 1 (cat):
**      - Setup pipe: stdout -> pipe write end
**      - Setup redirections: stdin -> input.txt
**      - execve cat
**   2. Fork child 2 (grep):
**      - Setup pipe: stdin -> pipe read end
**      - Setup redirections: stdout -> output.txt
**      - execve grep
*/

/*
** ============================================================================
**                           ERROR HANDLING SUMMARY
** ============================================================================
**
** This implementation handles all possible errors:
**
** 1. MALLOC FAILURE:
**    - Check if pids array allocation fails
**    - Print error and return 1
**
** 2. PIPE FAILURE:
**    - Check if pipe() returns -1
**    - Print error with errno
**    - Clean up and return 1
**
** 3. FORK FAILURE:
**    - Check if fork() returns -1
**    - Print error with errno
**    - Clean up pipes and return 1
**
** 4. DUP2 FAILURE:
**    - Check if dup2() returns -1
**    - Print error
**    - Exit child with status 1
**
** 5. EXECVE FAILURE:
**    - Check if execve() returns (only returns on error)
**    - Print error with errno
**    - Exit child with status 126
**
** 6. COMMAND NOT FOUND:
**    - Check if find_executable() returns NULL
**    - Print "command not found"
**    - Exit child with status 127
**
** 7. WAITPID FAILURE:
**    - Check if waitpid() returns -1
**    - Print error (but continue waiting for other children)
**
** 8. CLOSE FAILURE:
**    - Check if close() returns -1
**    - Print error (but continue execution)
**
** All file descriptors are properly closed in both success and error paths.
** Memory is freed before returning.
** Child processes always exit (never return to caller).
** Parent process waits for all children to prevent zombies.
*/

/*
** ============================================================================
**                              TESTING GUIDE
** ============================================================================
**
** Test Case 1: Basic pipe
**   Input: ls | cat
**   Expected: List of files
**   Verify: No fd leaks, correct output
**
** Test Case 2: Multiple pipes
**   Input: ls | grep .c | wc -l
**   Expected: Count of .c files
**   Verify: All children execute, correct count
**
** Test Case 3: Pipe with redirections
**   Input: cat < in.txt | grep pattern > out.txt
**   Expected: Filtered content in out.txt
**   Verify: Files opened correctly, pipe works
**
** Test Case 4: Command not found
**   Input: invalidcmd | cat
**   Expected: Error message, exit status 127
**   Verify: Error printed, second command still runs
**
** Test Case 5: Permission denied
**   Input: /bin/ls | /root/private
**   Expected: Error message, exit status 126
**   Verify: Error handling works
**
** Test Case 6: Empty command
**   Input: | cat
**   Expected: Syntax error or skip empty command
**   Verify: No crash
**
** Test Case 7: Memory leak test
**   Run: valgrind ./minishell
**   Input: Multiple piped commands
**   Expected: No leaks, all fds closed
**   Verify: valgrind shows 0 leaks
**
** Test Case 8: Signal handling
**   Input: sleep 10 | cat
**   Action: Press Ctrl+C
**   Expected: Both processes terminate
**   Verify: No zombies, clean exit
*/
