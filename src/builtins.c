/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 16:17:14 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 16:17:41 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* echo [-n] ... */
static int	builtin_echo(t_cmd *cmd)
{
	int	i = 1;
	int	no_nl = 0;

	if (cmd->argv[1] && ft_strcmp(cmd->argv[1], "-n") == 0)
	{
		no_nl = 1;
		i = 2;
	}
	while (cmd->argv[i])
	{
		write(1, cmd->argv[i], ft_strlen(cmd->argv[i]));
		if (cmd->argv[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (!no_nl)
		write(1, "\n", 1);
	return (0);
}

/* cd <path> */
static int	builtin_cd(t_cmd *cmd)
{
	if (!cmd->argv[1])
	{
		perror("cd");
		return (1);
	}
	if (chdir(cmd->argv[1]) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

/* pwd */
static int	builtin_pwd(void)
{
	char	buf[PATH_MAX];

	if (!getcwd(buf, sizeof(buf)))
	{
		perror("pwd");
		return (1);
	}
	printf("%s\n", buf);
	return (0);
}

/* env */
static int	builtin_env(t_shell *sh)
{
	t_env	*cur = sh->env;

	while (cur)
	{
		if (cur->value)
			printf("%s=%s\n", cur->key, cur->value);
		cur = cur->next;
	}
	return (0);
}

/* exit */
static int	builtin_exit(t_shell *sh, t_cmd *cmd)
{
	int	code = sh->last_status;

	if (cmd->argv[1])
		code = atoi(cmd->argv[1]);
	exit(code);
}

int	is_builtin(char *name)
{
	if (!name)
		return (0);
	if (ft_strcmp(name, "echo") == 0)
		return (1);
	if (ft_strcmp(name, "cd") == 0)
		return (1);
	if (ft_strcmp(name, "pwd") == 0)
		return (1);
	if (ft_strcmp(name, "env") == 0)
		return (1);
	if (ft_strcmp(name, "exit") == 0)
		return (1);
	/* export / unset to be added */
	return (0);
}

int	run_builtin(t_shell *sh, t_cmd *cmd)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	if (ft_strcmp(cmd->argv[0], "echo") == 0)
		return (builtin_echo(cmd));
	if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (builtin_cd(cmd));
	if (ft_strcmp(cmd->argv[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(cmd->argv[0], "env") == 0)
		return (builtin_env(sh));
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		return (builtin_exit(sh, cmd));
	return (0);
}
