/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_array.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 11:20:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static char	*build_keyvalue_pair(const char *k, const char *v)
{
	size_t	lenk;
	size_t	lenv;
	char	*kv;

	if (!k || !*k || v == NULL)
		return (NULL);
	lenk = ft_strlen(k);
	lenv = ft_strlen(v);
	kv = malloc(lenk + lenv + 2);
	if (!kv)
		return (NULL);
	ft_memcpy(kv, k, lenk);
	kv[lenk] = '=';
	ft_memcpy(kv + lenk + 1, v, lenv);
	kv[lenk + lenv + 1] = '\0';
	return (kv);
}

/* main: append key=value to env array */
/* pseudo, adapt to your code */
/* build "KEY=VALUE" and push into NULL-terminated env array */
int	append_env(char ***arr, size_t *n, const char *k, const char *v)
{
	char	*kv;
	char	**newarr;
	size_t	i;

	if (!arr || !n)
		return (0);
	kv = build_keyvalue_pair(k, v);
	if (!kv)
		return (0);
	newarr = malloc(sizeof(char *) * (*n + 2));
	if (!newarr)
		return (free(kv), -1);
	i = 0;
	while (i < *n)
	{
		newarr[i] = (*arr)[i];
		i++;
	}
	newarr[*n] = kv;
	newarr[*n + 1] = NULL;
	free(*arr);
	*arr = newarr;
	*n += 1;
	return (0);
}
