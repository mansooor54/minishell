/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 16:18:38 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 16:19:02 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* split PATH into dirs using ':' and try execve on each */
static void	try_exec_path(char **argv, char **env_arr)
{
	char	*path_val;
	char	*tmp;
	int		i, j, start;

	path_val = getenv("PATH"); /* temporary: use real environment */
	if (!path_val || !argv[0])
	{
		execve(argv[0], argv, env_arr);
		perror(argv[0]);
		_exit(127);
	}
	i = 0;
	start = 0;
	while (1)
	{
		if (path_val[i] == ':' || path_val[i] == '\0')
		{
			int len = i - start;
			char dirbuf[PATH_MAX];

			if (len >= PATH_MAX)
				len = PATH_MAX - 1;
			for (j = 0; j < len; j++)
				dirbuf[j] = path_val[start + j];
			dirbuf[len] = '\0';
			tmp = ft_strjoin_path(dirbuf, argv[0]);
			if (tmp)
			{
				execve(tmp, argv, env_arr);
				free(tmp);
			}
			if (path_val[i] == '\0')
				break ;
			start = i + 1;
		}
		i++;
	}
	perror(argv[0]);
	_exit(127);
}

int	exec_command(t_shell *sh, t_cmd *cmd)
{
	pid_t	pid;
	int		status;
	char	**env_arr;
	int		bcode;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);

	if (is_builtin(cmd->argv[0]))
	{
		bcode = run_builtin(sh, cmd);
		sh->last_status = bcode;
		return (bcode);
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		sh->last_status = 1;
		return (1);
	}
	if (pid == 0)
	{
		setup_signals_child();
		env_arr = env_to_envarray(sh->env);
		try_exec_path(cmd->argv, env_arr);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		sh->last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		sh->last_status = 128 + WTERMSIG(status);
	return (sh->last_status);
}
