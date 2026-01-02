/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 12:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/24 12:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	count_env_all(t_env *env)
{
	int	count;

	count = 0;
	while (env)
	{
		count++;
		env = env->next;
	}
	return (count);
}

void	sort_env_keys(char **keys, int count)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strcmp(keys[j], keys[j + 1]) > 0)
			{
				tmp = keys[j];
				keys[j] = keys[j + 1];
				keys[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

static char	*find_env_value(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

static void	print_one_export(char *key, char *val)
{
	ft_putstr_fd("declare -x ", 1);
	ft_putstr_fd(key, 1);
	if (val)
	{
		ft_putstr_fd("=\"", 1);
		ft_putstr_fd(val, 1);
		ft_putstr_fd("\"", 1);
	}
	ft_putstr_fd("\n", 1);
}

void	print_sorted_export(t_env *env, char **keys, int count)
{
	int		i;
	char	*val;

	i = 0;
	while (i < count)
	{
		val = find_env_value(env, keys[i]);
		print_one_export(keys[i], val);
		i++;
	}
}
