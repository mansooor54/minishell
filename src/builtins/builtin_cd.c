/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* return heap string; never NULL, returns "" if src is NULL */
static char	*safe_strdup_or_empty(const char *src)
{
	if (src == NULL)
		return (ft_strdup(""));
	return (ft_strdup((char *)src));
}

/* update or add env key=value, never store NULL values */
static int	set_env_kv(t_env **env, const char *key, const char *value)
{
	t_env	*cur;

	if (!env || !key)
		return (1);
	cur = *env;
	while (cur)
	{
		if (ft_strcmp(cur->key, (char *)key) == 0)
		{
			free(cur->value);
			cur->value = safe_strdup_or_empty(value);
			return (0);
		}
		cur = cur->next;
	}
	add_env_node(env, create_env_node((char *)key,
			safe_strdup_or_empty(value)));
	return (0);
}

/* handle chdir + print errors */
static int	change_directory(char *target)
{
	if (chdir(target) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(target, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		return (1);
	}
	return (0);
}

/* helper: update OLDPWD and PWD after successful chdir */
static void	update_pwd_vars(t_env **env, char *oldpwd, int print_after)
{
	char	*newpwd;

	if (oldpwd)
		set_env_kv(env, "OLDPWD", oldpwd);
	newpwd = dup_cwd();
	if (newpwd)
	{
		set_env_kv(env, "PWD", newpwd);
		if (print_after)
			ft_putendl_fd(newpwd, 1);
	}
	free(newpwd);
}

/* main builtin_cd
** Change directory builtin command
** Changes to HOME if no argument, otherwise changes to specified directory
** Returns 0 on success, 1 on failure
*/
int	builtin_cd(char **args, t_env **env)
{
	char	*oldpwd;
	char	*target;
	int		print_after;

	oldpwd = dup_cwd();
	target = resolve_target(args, *env, &print_after);
	if (!target)
		return (1);
	if (change_directory(target))
	{
		free(target);
		free(oldpwd);
		return (1);
	}
	update_pwd_vars(env, oldpwd, print_after);
	free(oldpwd);
	free(target);
	return (0);
}
