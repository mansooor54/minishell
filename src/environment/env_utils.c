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

#include "../../include/minishell.h"

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
			if (!tmp)
				break ;
			envp[i] = ft_strjoin(tmp, env->value);
			free(tmp);
			if (!envp[i])
				break ;
			i++;
		}
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}

/*
** Update or add environment variable
** If key exists, update value; otherwise create new node
** Duplicates the value (if not NULL)
*/
void	env_set_value(t_env **env, char *key, char *value)
{
	t_env	*cur;

	cur = *env;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			free(cur->value);
			if (value)
				cur->value = ft_strdup(value);
			else
				cur->value = NULL;
			return ;
		}
		cur = cur->next;
	}
	add_env_node(env, create_env_node(key, value));
}
