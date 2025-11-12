/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_core.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:23:36 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* forward decl only; real body is in expander_utils.c */
void	process_dollar(t_exp_ctx *c);

static int	init_ctx(t_exp_ctx *c, char *s, t_env *env, int st)
{
	size_t	cap;

	if (!s)
		return (0);
	c->str = s;
	c->env = env;
	c->exit_status = st;
	c->in_quote = 0;
	c->i = 0;
	c->j = 0;
	cap = ft_strlen(s) * 10 + 4096;
	c->result = malloc(cap);
	return (c->result != NULL);
}

/* Helper: check if current character is a quote */
static int	is_quote(t_exp_ctx *c)
{
	char	ch;

	ch = c->str[c->i];
	return (ch == '\'' || ch == '"');
}

/* Helper: handle quote toggling during expansion */
static void	handle_quote(t_exp_ctx *c)
{
	if (!c->in_quote)
		c->in_quote = c->str[c->i];
	else if (c->in_quote == c->str[c->i])
		c->in_quote = 0;
	c->result[c->j++] = c->str[c->i++];
}

static int	should_expand(t_exp_ctx *c)
{
	return (c->str[c->i] == '$' && c->in_quote != '\'');
}

char	*expand_variables(char *str, t_env *env, int exit_status)
{
	t_exp_ctx	c;

	if (!init_ctx(&c, str, env, exit_status))
		return (NULL);
	while (str[c.i])
	{
		if (is_quote(&c))
			handle_quote(&c);
		else if (should_expand(&c))
			process_dollar(&c);
		else
			c.result[c.j++] = c.str[c.i++];
	}
	c.result[c.j] = '\0';
	return (c.result);
}
