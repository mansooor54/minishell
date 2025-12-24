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

static int	count_env(t_env *env)
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

static void	sort_env_keys(char **keys, int count)
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

static void	print_sorted_export(t_env *env, char **keys, int count)
{
	int		i;
	t_env	*cur;
	char	*val;

	i = 0;
	while (i < count)
	{
		cur = env;
		val = NULL;
		while (cur)
		{
			if (ft_strcmp(cur->key, keys[i]) == 0)
			{
				val = cur->value;
				break ;
			}
			cur = cur->next;
		}
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(keys[i], 1);
		if (val)
		{
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(val, 1);
			ft_putstr_fd("\"", 1);
		}
		ft_putstr_fd("\n", 1);
		i++;
	}
}

static void	print_export_list(t_env *env)
{
	int		count;
	int		i;
	char	**keys;
	t_env	*cur;

	count = count_env(env);
	if (count == 0)
		return ;
	keys = malloc(sizeof(char *) * count);
	if (!keys)
		return ;
	cur = env;
	i = 0;
	while (cur)
	{
		keys[i++] = cur->key;
		cur = cur->next;
	}
	sort_env_keys(keys, count);
	print_sorted_export(env, keys, count);
	free(keys);
}

/*
** Export builtin command
** Sets environment variables in KEY=VALUE format
** Returns 0 on success
*/
int	builtin_export(char **args, t_env **env)
{
	int		i;
	int		ret;
	char	*key;
	char	*value;

	if (!args[1])
	{
		print_export_list(*env);
		return (0);
	}
	i = 1;
	ret = 0;
	while (args[i])
	{
		parse_export_arg(args[i], &key, &value);
		if (!is_valid_identifier(key))
		{
			ft_putstr_fd("minishell: export: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			ret = 1;
		}
		else
			env_set_value(env, key, value);
		free(key);
		free(value);
		i++;
	}
	return (ret);
}
