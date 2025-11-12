/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:23:36 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* Helper: check if a character is a quote (for quote removal) */
static int	is_quote(char ch)
{
	return (ch == '\'' || ch == '"');
}

/* Helper: handle quote removal logic */
static int	handle_quote(t_quote_ctx *c, char *s)
{
	if (!c->quote && is_quote(s[c->i]))
	{
		c->quote = s[c->i++];
		return (1);
	}
	if (c->quote && s[c->i] == c->quote)
	{
		c->quote = 0;
		c->i++;
		return (1);
	}
	return (0);
}

static int	handle_bs_outside(t_quote_ctx *c, char *s)
{
	if (c->quote || s[c->i] != '\\')
		return (0);
	if (s[c->i + 1] == '\n')
	{
		c->i += 2;                 /* remove backslash-newline */
		return (1);
	}
	if (s[c->i + 1] == '\0')
	{
		c->res[c->j++] = '\\';     /* PRESERVE trailing '\' */
		c->i += 1;
		return (1);
	}
	c->res[c->j++] = s[c->i + 1];  /* unescape next char */
	c->i += 2;
	return (1);
}

static int	handle_bs_in_dq(t_quote_ctx *c, char *s)
{
	char	nx;

	if (c->quote != '"' || s[c->i] != '\\')
		return (0);
	nx = s[c->i + 1];
	if (nx == '\\' || nx == '"' || nx == '`' || nx == '$')
	{
		c->res[c->j++] = nx;
		c->i += 2;
		return (1);
	}
	if (nx == '\n')
	{
		c->i += 2;
		return (1);
	}
	return (0);
}

char	*remove_quotes(char *str)
{
	t_quote_ctx	c;
	size_t		len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	c.res = malloc(len + 1);
	if (!c.res)
		return (NULL);
	c.i = 0;
	c.j = 0;
	c.quote = 0;
	while (str[c.i])
	{
		if (handle_quote(&c, str))
			continue ;
		if (handle_bs_outside(&c, str))
			continue ;
		if (handle_bs_in_dq(&c, str))
			continue ;
		c.res[c.j++] = str[c.i++];
	}
	c.res[c.j] = '\0';
	return (c.res);
}
