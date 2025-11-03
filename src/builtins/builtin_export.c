/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Parse KEY=VALUE format
** Splits the string at '=' and returns key and value
*/
static void	parse_export_arg(char *arg, char **key, char **value)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (eq)
	{
		*key = ft_substr(arg, 0, eq - arg);
		*value = ft_strdup(eq + 1);
	}
	else
	{
		*key = ft_strdup(arg);
		*value = NULL;
	}
}

/*
** Update or add environment variable
** If key exists, update value; otherwise create new node
*/
static void	update_or_add_env(t_env **env, char *key, char *value)
{
	t_env	*current;
	t_env	*new_node;

	current = *env;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (value)
			{
				free(current->value);
				current->value = ft_strdup(value);
			}
			return ;
		}
		current = current->next;
	}
	new_node = create_env_node(key, value);
	add_env_node(env, new_node);
}

/*
** Export builtin command
** Sets environment variables in KEY=VALUE format
** Returns 0 on success
*/
int	builtin_export(char **args, t_env **env)
{
	int		i;
	char	*key;
	char	*value;

	if (!args[1])
		return (builtin_env(*env));
	i = 1;
	while (args[i])
	{
		parse_export_arg(args[i], &key, &value);
		update_or_add_env(env, key, value);
		free(key);
		free(value);
		i++;
	}
	return (0);
}
