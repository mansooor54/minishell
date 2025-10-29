/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 16:14:16 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 16:14:42 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* create node "KEY=VALUE" -> key, value split at first '=' */
static t_env	*env_new_pair(const char *entry)
{
	size_t	i = 0;
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	while (entry[i] && entry[i] != '=')
		i++;
	node->key = malloc(i + 1);
	if (!node->key)
		return (free(node), NULL);
	for (size_t k = 0; k < i; k++)
		node->key[k] = entry[k];
	node->key[i] = '\0';
	if (entry[i] == '=')
		node->value = ft_strdup(entry + i + 1);
	else
		node->value = ft_strdup("");
	node->next = NULL;
	return (node);
}

static void	env_add_back(t_env **head, t_env *new_node)
{
	t_env *t;

	if (!new_node)
		return ;
	if (!*head)
	{
		*head = new_node;
		return ;
	}
	t = *head;
	while (t->next)
		t = t->next;
	t->next = new_node;
}

t_env	*env_build(char **envp)
{
	t_env	*head = NULL;
	int		i = 0;

	while (envp && envp[i])
	{
		env_add_back(&head, env_new_pair(envp[i]));
		i++;
	}
	return (head);
}

char	*env_get_value(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/* build char ** like ["KEY=VALUE", ... , NULL] for execve */
char	**env_to_envarray(t_env *env)
{
	int		count = 0;
	t_env	*tmp = env;
	char	**arr;
	int		i;

	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (env)
	{
		size_t len_k = ft_strlen(env->key);
		size_t len_v = ft_strlen(env->value);
		char *kv = malloc(len_k + 1 + len_v + 1);
		if (!kv)
		{
			arr[i] = NULL;
			return (arr);
		}
		size_t p = 0;
		for (size_t a = 0; a < len_k; a++)
			kv[p++] = env->key[a];
		kv[p++] = '=';
		for (size_t b = 0; b < len_v; b++)
			kv[p++] = env->value[b];
		kv[p] = '\0';
		arr[i++] = kv;
		env = env->next;
	}
	arr[i] = NULL;
	return (arr);
}

void	env_free_array(char **arr)
{
	int i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
