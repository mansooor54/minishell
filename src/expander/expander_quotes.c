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

static int	is_quote_char(char ch)
{
	return (ch == '\'' || ch == '"');
}

static int	handle_quote(t_quote_ctx *c, char *s)
{
	if (!c->quote && is_quote_char(s[c->i]))
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

static void	handle_bs_outside(t_quote_ctx *c, char *s)
{
	if (s[c->i] == '\\' && !c->quote && s[c->i + 1])
	{
		c->res[c->j++] = s[c->i + 1];
		c->i += 2;
	}
	else
	{
		c->res[c->j++] = s[c->i++];
	}
}

static void	handle_bs_in_dq(t_quote_ctx *c, char *s)
{
	if (s[c->i] == '\\' && c->quote == '"' && s[c->i + 1])
	{
		if (s[c->i + 1] == '"' || s[c->i + 1] == '$'
			|| s[c->i + 1] == '\\')
		{
			c->res[c->j++] = s[c->i + 1];
			c->i += 2;
		}
		else
		{
			c->res[c->j++] = s[c->i++];
		}
	}
	else
		handle_bs_outside(c, s);
}

char	*remove_quotes(char *s)
{
	t_quote_ctx	c;
	char		*res;

	if (!s)
		return (NULL);
	c.i = 0;
	c.j = 0;
	c.quote = 0;
	c.str = s;
	res = malloc(ft_strlen(s) + 1);
	if (!res)
		return (NULL);
	c.res = res;
	while (s[c.i])
	{
		if (handle_quote(&c, s))
			continue ;
		if (s[c.i] == '\\')
			handle_bs_in_dq(&c, s);
		else
			c.res[c.j++] = s[c.i++];
	}
	c.res[c.j] = '\0';
	return (res);
}
