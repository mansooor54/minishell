/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:24:14 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 15:24:14 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}

char	*ft_strdup(const char *s)
{
	size_t	len;
	size_t	i;
	char	*out;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	i = 0;
	while (i < len)
	{
		out[i] = s[i];
		i++;
	}
	out[i] = '\0';
	return (out);
}

int	ft_strcmp(const char *a, const char *b)
{
	size_t	i = 0;

	if (!a && !b)
		return (0);
	if (!a)
		return (-1);
	if (!b)
		return (1);
	while (a[i] && b[i] && a[i] == b[i])
		i++;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

/* join dir + "/" + cmd into new malloc string */
char	*ft_strjoin_path(const char *dir, const char *cmd)
{
	size_t	len_d = ft_strlen(dir);
	size_t	len_c = ft_strlen(cmd);
	char	*out;
	size_t	i;
	size_t	j;

	out = malloc(len_d + 1 + len_c + 1);
	if (!out)
		return (NULL);
	i = 0;
	while (i < len_d)
	{
		out[i] = dir[i];
		i++;
	}
	out[i++] = '/';
	j = 0;
	while (j < len_c)
	{
		out[i + j] = cmd[j];
		j++;
	}
	out[i + j] = '\0';
	return (out);
}
