/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:23:36 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* No expansion in single quotes. Respect \$ escape before unquoting. */
void	process_dollar(t_exp_ctx *c)
{
	if (c->i > 0 && c->str[c->i - 1] == '\\' && c->in_quote != '\'')
	{
		c->result[c->j++] = c->str[c->i++];
		return ;
	}
	if (c->in_quote == '\'')
	{
		c->result[c->j++] = c->str[c->i++];
		return ;
	}
	c->i++;
	if (c->str[c->i] == '?')
	{
		expand_exit_status(c->result, &c->j, c->exit_status);
		c->i++;
		return ;
	}
	/* expand variable name starting at c->i; writes into c->result and
	   updates c->i and c->j. If you have an old expand_var_name(ctx)
	   that used pointers, convert it to use c->i / c->j like here. */
	expand_var_name(c);
}

/* file: expander_utils.c (or same file as expand_variables) */
static int	init_ctx(t_exp_ctx *c, char *s, t_env *env, int st)
{
	size_t cap;

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

static int	is_quote(t_exp_ctx *c)
{
	char ch = c->str[c->i];
	return (ch == '\'' || ch == '"');
}

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

static void	copy_char(t_exp_ctx *c)
{
	c->result[c->j++] = c->str[c->i++];
}

/* ≤ 25 lines, ≤ 4 params */
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
			process_dollar(&c); /* must use c.i/c.j */
		else
			copy_char(&c);
	}
	c.result[c.j] = '\0';
	return (c.result);
}

void	expand_arg(char **arg, t_env *env, int exit_status)
{
	char	*expanded;
	char	*unquoted;

	expanded = expand_variables(*arg, env, exit_status);
	unquoted = remove_quotes(expanded);
	free(*arg);
	free(expanded);
	*arg = unquoted;
}

void	expand_cmd_args(t_cmd *cmd, t_env *env, int exit_status)
{
	int	i;

	i = 0;
	while (cmd->args[i])
	{
		expand_arg(&cmd->args[i], env, exit_status);
		i++;
	}
}
