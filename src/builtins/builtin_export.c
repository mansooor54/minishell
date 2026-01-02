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

static void	print_export_list(t_env *env)
{
	int		count;
	int		i;
	char	**keys;
	t_env	*cur;

	count = count_env_all(env);
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

static int	process_export_arg(char *arg, t_env **env)
{
	char	*key;
	char	*value;
	int		ret;

	ret = 0;
	parse_export_arg(arg, &key, &value);
	if (!is_valid_identifier(key))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		ret = 1;
	}
	else
		env_set_value(env, key, value);
	free(key);
	free(value);
	return (ret);
}

int	builtin_export(char **args, t_env **env)
{
	int	i;
	int	ret;

	if (!args[1])
	{
		print_export_list(*env);
		return (0);
	}
	i = 1;
	ret = 0;
	while (args[i])
	{
		if (process_export_arg(args[i], env))
			ret = 1;
		i++;
	}
	return (ret);
}
