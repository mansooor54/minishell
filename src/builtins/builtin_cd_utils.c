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
** Duplicate current working directory
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
char	*resolve_target(char **args, t_env *env, int *print_after)
{
	char	*target;

	*print_after = 0;
	if (!args[1])
	{
		target = get_env_value(env, "HOME");
		if (!target)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			return (NULL);
		}
		return (ft_strdup(target));
	}
	if (ft_strcmp(args[1], "-") == 0)
	{
		target = get_env_value(env, "OLDPWD");
		if (!target)
		{
			ft_putendl_fd("minishell: cd: OLDPWD not set", 2);
			return (NULL);
		}
		*print_after = 1;
		return (ft_strdup(target));
	}
	return (ft_strdup(args[1]));
}
