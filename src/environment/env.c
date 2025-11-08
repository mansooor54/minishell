/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:01:22 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** increment_shlvl - Increment the SHLVL environment variable
**
** SHLVL tracks the nesting level of shells. Each time a new shell
** is started, SHLVL should be incremented by 1.
**
** @param env: Pointer to environment linked list
**
** Return: void
*/
void	increment_shlvl(t_env **env)
{
	t_env	*current;
	int		shlvl_value;

	current = *env;
	while (current)
	{
		if (ft_strcmp(current->key, "SHLVL") == 0)
		{
			shlvl_value = ft_atoi(current->value);
			if (shlvl_value < 0)
				shlvl_value = 0;
			shlvl_value++;
			free(current->value);
			current->value = ft_itoa(shlvl_value);
			return ;
		}
		current = current->next;
	}
	add_env_node(env, create_env_node("SHLVL", "1"));
}

/*
** Initialize environment from envp array
** Creates linked list of environment variables
*/
t_env	*init_env(char **envp)
{
	t_env	*env;
	int		i;
	char	*key;
	char	*value;
	char	*eq_pos;

	env = NULL;
	i = 0;
	while (envp[i])
	{
		eq_pos = ft_strchr(envp[i], '=');
		if (eq_pos)
		{
			key = ft_substr(envp[i], 0, eq_pos - envp[i]);
			value = ft_strdup(eq_pos + 1);
			add_env_node(&env, create_env_node(key, value));
			free(key);
			free(value);
		}
		i++;
	}
	return (env);
}

void	init_shell(t_shell *shell, char **envp)
{
	shell->env = init_env(envp);
	shell->exit_status = 0;
	shell->should_exit = 0;
	increment_shlvl(&shell->env);
	history_init(shell->env); 
}
