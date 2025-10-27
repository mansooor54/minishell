#include "minishell.h"

/* run_builtin:
   - handles builtins that must run in parent (like cd)
   - returns 1 if builtin executed, 0 if not builtin
*/
int	run_builtin(char **argv, t_shell *sh)
{
	char	cwd[PATH_MAX];
	char	*name;

	if (!argv[0])
		return (0);
	/* cd */
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
			return (1);
		}
		/* cd succeeded: update prompt mode */
		if (getcwd(cwd, PATH_MAX) == NULL)
		{
			sh->show_dir = 0;
			sh->cwd_name[0] = '\0';
			return (1);
		}
		name = get_last_dir_name(cwd);

		/* copy name into sh->cwd_name */
		{
			int i = 0;
			while (name[i] && i < PATH_MAX - 1)
			{
				sh->cwd_name[i] = name[i];
				i++;
			}
			sh->cwd_name[i] = '\0';
		}

		/* if dir name is "minishell", hide it */
		if (sh->cwd_name[0] == 'm'
			&& sh->cwd_name[1] == 'i'
			&& sh->cwd_name[2] == 'n'
			&& sh->cwd_name[3] == 'i'
			&& sh->cwd_name[4] == 's'
			&& sh->cwd_name[5] == 'h'
			&& sh->cwd_name[6] == 'e'
			&& sh->cwd_name[7] == 'l'
			&& sh->cwd_name[8] == 'l'
			&& sh->cwd_name[9] == '\0')
		{
			sh->show_dir = 0;
		}
		else
			sh->show_dir = 1;
		return (1);
	}
	return (0);
}

/* run_command:
   - if builtin handled in parent, skip fork
   - else fork/exec external command (/bin/<cmd>)
*/
void	run_command(char **argv, t_shell *sh)
{
	pid_t	pid;
	int		status;
	char	path[256];
	int		i;
	int		ok;

	if (!argv[0])
		return ;
	/* try builtin first */
	if (run_builtin(argv, sh))
		return ;
	pid = fork();
	if (pid < 0)
	{
		ft_putstr_fd("fork error\n", 2);
		return ;
	}
	if (pid == 0)
	{
		/* child builds "/bin/<cmd>" */
		ok = 1;
		if (ft_strlen(argv[0]) + 5 >= 256)
			ok = 0;
		if (ok)
		{
			path[0] = '/';
			path[1] = 'b';
			path[2] = 'i';
			path[3] = 'n';
			path[4] = '/';
			path[5] = '\0';
			i = 0;
			while (argv[0][i] != '\0' && (5 + i) < 255)
			{
				path[5 + i] = argv[0][i];
				i++;
			}
			path[5 + i] = '\0';
			execve(path, argv, NULL);
		}
		ft_putstr_fd("execve: ", 2);
		ft_putstr_fd(argv[0], 2);
		ft_putstr_fd(": failed\n", 2);
		_exit(127);
	}
	waitpid(pid, &status, 0);
	(void)status;
	(void)sh;
}
