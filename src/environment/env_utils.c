/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:04:12 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Parse environment string (KEY=VALUE)
** Splits at '=' and returns key and value
*/
void	parse_env_string(char *env_str, char **key, char **value)
{
	char	*eq;

	eq = ft_strchr(env_str, '=');
	if (eq)
	{
		*key = ft_substr(env_str, 0, eq - env_str);
		*value = ft_strdup(eq + 1);
	}
	else
	{
		*key = ft_strdup(env_str);
		*value = NULL;
	}
}

/*
** Count environment variables
** Returns the number of nodes in the list
*/
static int	count_env(t_env *env)
{
	int	count;

	count = 0;
	while (env)
	{
		if (env->value)
			count++;
		env = env->next;
	}
	return (count);
}

/*
** Convert environment list to array
** Creates char** array in KEY=VALUE format for execve
*/
char	**env_to_array(t_env *env)
{
	char	**envp;
	char	*tmp;
	int		i;

	envp = malloc(sizeof(char *) * (count_env(env) + 1));
	if (!envp)
		return (NULL);
	i = 0;
	while (env)
	{
		if (env->value)
		{
			tmp = ft_strjoin(env->key, "=");
			envp[i] = ft_strjoin(tmp, env->value);
			free(tmp);
			i++;
		}
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}
