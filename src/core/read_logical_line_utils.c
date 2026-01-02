/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_logical_line_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/12/25 00:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	has_unclosed_quotes_in_string(const char *s)
{
	size_t	i;
	char	in_q;

	i = 0;
	in_q = 0;
	while (s[i])
	{
		if (!in_q && (s[i] == '\'' || s[i] == '"'))
			in_q = s[i];
		else if (in_q && s[i] == in_q)
			in_q = 0;
		i++;
	}
	return (in_q != 0);
}

int	needs_continuation(const char *line)
{
	if (line && has_unclosed_quotes_in_string(line))
		return (1);
	return (0);
}
