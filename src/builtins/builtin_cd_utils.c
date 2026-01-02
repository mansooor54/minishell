/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:45:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 13:45:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Dup_cwd: Duplicate current working directory
** Returns heap-allocated string with current directory or NULL on failure
*/
char	*dup_cwd(void)
{
	char	buf[4096];

	if (getcwd(buf, sizeof(buf)) == NULL)
		return (NULL);
	return (ft_strdup(buf));
}

/*
** Resolve cd target directory
** Handles: no args (HOME), "-" (OLDPWD), or explicit path
** Sets print_after flag for "-" case
** Returns heap-allocated target path or NULL on error
*/
static char	*dup_env_or_err(t_env *env, const char *key, const char *errmsg)
{
	char	*v;

	v = get_env_value(env, (char *)key);
	if (!v)
	{
		ft_putendl_fd((char *)errmsg, 2);
		return (NULL);
	}
	return (ft_strdup(v));
}

char	*resolve_target(char **args, t_env *env, int *print_after)
{
	*print_after = 0;
	if (!args[1])
		return (dup_env_or_err(env, "HOME", "minishell: cd: HOME not set"));
	if (ft_strcmp(args[1], "-") == 0)
	{
		*print_after = 1;
		return (dup_env_or_err(env, "OLDPWD", "minishell: cd: OLDPWD not set"));
	}
	return (ft_strdup(args[1]));
}
