/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join_continuation.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/12/25 00:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static size_t	get_safe_len(const char *s)
{
	if (s)
		return (ft_strlen(s));
	return (0);
}

static void	copy_str(char *dst, const char *src, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		dst[i] = src[i];
		i++;
	}
}

static char	*str_join(const char *a, const char *b)
{
	size_t	la;
	size_t	lb;
	char	*r;

	la = get_safe_len(a);
	lb = get_safe_len(b);
	r = malloc(la + lb + 1);
	if (!r)
		return (NULL);
	if (a)
		copy_str(r, a, la);
	if (b)
		copy_str(r + la, b, lb);
	r[la + lb] = '\0';
	return (r);
}

char	*join_continuation(char *line, char *next)
{
	char	*r;

	if (!line && !next)
		return (NULL);
	if (!line)
		return (ft_strdup(next));
	if (!next)
		return (line);
	while (*next == ' ' || *next == '\t' || *next == '\n')
		next++;
	r = str_join(line, next);
	free(line);
	return (r);
}
