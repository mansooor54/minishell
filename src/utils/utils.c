/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:06:08 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
/*
** Free array of strings
** Frees each string and the array itself
*/
void	free_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

/*
** Join two strings and free the first one
** Useful for chaining multiple joins
*/
char	*ft_strjoin_free(char *s1, char const *s2)
{
	char	*res;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
	{
		if (!s2)
			return (NULL);
		return (ft_strdup(s2));
	}
	if (!s2)
	{
		res = ft_strdup(s1);
		free(s1);
		return (res);
	}
	res = ft_strjoin(s1, s2);
	free(s1);
	return (res);
}

/*
** Free all environment variables
** Frees keys, values, and nodes
*/
void	free_env(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env;
		env = env->next;
		free(tmp->key);
		free(tmp->value);
		free(tmp);
	}
}

int	is_valid_identifier(char *s)
{
	int	i;

	if (!s || !(ft_isalpha((unsigned char)s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i])
	{
		if (!(ft_isalnum((unsigned char)s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

/*
** return heap string; never NULL, returns "" if src is NULL
*/
char	*safe_strdup_or_empty(const char *src)
{
	if (src == NULL)
		return (ft_strdup(""));
	return (ft_strdup((char *)src));
}
