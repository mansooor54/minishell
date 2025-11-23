/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_logical_line_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/20 21:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* ************************************************************************** */
/*                     1.  QUOTE & BACKSLASH DETECTION                        */
/* ************************************************************************** */

static int	has_unclosed_quotes_in_string(const char *s)
{
	size_t	i;
	char	in_q;
	int		esc;

	i = 0;
	in_q = 0;
	esc = 0;
	while (s[i])
	{
		if (!in_q && (s[i] == '\'' || s[i] == '"'))
			in_q = s[i];
		else if (in_q && s[i] == in_q && !esc)
			in_q = 0;
		if (in_q == '"' && s[i] == '\\' && !esc)
			esc = 1;
		else if (esc)
			esc = 0;
		i++;
	}
	return (in_q != 0);
}

static int	has_odd_trailing_backslashes(const char *s)
{
	int	i;
	int	c;

	if (!s)
		return (0);
	i = (int)ft_strlen(s) - 1;
	while (i >= 0 && (s[i] == ' ' || s[i] == '\t'))
		i--;
	c = 0;
	while (i >= 0 && s[i] == '\\')
	{
		c++;
		i--;
	}
	return (c % 2 == 1);
}

int	needs_continuation(const char *line)
{
	if (line && has_unclosed_quotes_in_string(line))
		return (1);
	if (has_odd_trailing_backslashes(line))
		return (1);
	return (0);
}
