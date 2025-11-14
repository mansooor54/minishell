/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 16:02:34 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ====== helpers (static, same variable names) ====== */

static int	enter_quote(char *s, size_t *i, char *in_quote, int *escaped)
{
	if (*in_quote)
		return (0);
	if (s[*i] != '\'' && s[*i] != '"')
		return (0);
	*in_quote = s[*i];
	(*i)++;
	*escaped = 0;
	return (1);
}

static int	handle_inside(char *s, size_t *i, char *in_quote, int *escaped)
{
	if (!*in_quote)
		return (0);
	if (s[*i] == *in_quote && !*escaped)
	{
		*in_quote = 0;
		(*i)++;
		return (1);
	}
	if (*in_quote == '"' && s[*i] == '\\')
	{
		*escaped = !*escaped;
		(*i)++;
		return (1);
	}
	*escaped = 0;
	(*i)++;
	return (1);
}

static int	handle_outside(char *s, size_t *i)
{
	if (s[*i] != '\\')
		return (0);
	(*i)++;
	if (s[*i] != '\0')
		(*i)++;
	return (1);
}

int	has_unclosed_quotes(char *s)
{
	size_t	i;
	char	in_quote;
	int		escaped;

	i = 0;
	in_quote = 0;
	escaped = 0;
	while (s[i])
	{
		if (enter_quote(s, &i, &in_quote, &escaped))
			continue ;
		if (handle_inside(s, &i, &in_quote, &escaped))
			continue ;
		if (handle_outside(s, &i))
			continue ;
		i++;
	}
	return (in_quote != 0);
}
