#include "minishell.h"

/*
builtin: cd
returns 1 if handled, 0 if not builtin
*/
int	run_builtin(char **argv)
{
	if (!argv[0])
		return (0);
	if (argv[0][0] == 'c' && argv[0][1] == 'd' && argv[0][2] == '\0')
	{
		if (!argv[1])
		{
			ft_putstr_fd("cd: missing argument\n", 2);
			return (1);
		}
		if (chdir(argv[1]) != 0)
		{
			ft_putstr_fd("cd: cannot access: ", 2);
			ft_putstr_fd(argv[1], 2);
			ft_putstr_fd("\n", 2);
		}
		return (1);
	}
	return (0);
}

/* check if command already has '/' */
static int	has_slash(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == '/')
			return (1);
		i++;
	}
	return (0);
}

/*
child_exec:
- If cmd has '/', exec directly
- Else search PATH dirs from sh->envp
- On fail print "cmd: command not found" and _exit
This runs in the child only
*/
static void	child_exec(char **argv, t_shell *sh)
{
	char	*path_value;
	char	**dirs;
	int		i;
	char	*full;

	if (!argv[0])
		_exit(127);
	if (has_slash(argv[0]))
	{
		execve(argv[0], argv, sh->envp);
		ft_putstr_fd(argv[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		_exit(127);
	}
	path_value = get_env_value("PATH", sh->envp);
	dirs = split_path_dirs(path_value);
	if (!dirs)
	{
		ft_putstr_fd(argv[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		_exit(127);
	}
	i = 0;
	while (dirs[i])
	{
		full = ft_strjoin_path(dirs[i], argv[0]);
		if (full)
		{
			execve(full, argv, sh->envp);
			free(full);
		}
		i++;
	}
	ft_putstr_fd(argv[0], 2);
	ft_putstr_fd(": command not found\n", 2);
	_exit(127);
}

/*
run_command:
- Handle builtin (cd)
- Else fork
- Child calls child_exec
- Parent waits
*/
void	run_command(char **argv, t_shell *sh)
{
	pid_t	pid;
	int		status;

	if (!argv[0])
		return ;
	if (run_builtin(argv))
		return ;
	pid = fork();
	if (pid < 0)
	{
		ft_putstr_fd("fork error\n", 2);
		return ;
	}
	if (pid == 0)
		child_exec(argv, sh);
	waitpid(pid, &status, 0);
	(void)status;
}
