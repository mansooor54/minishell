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

#include "minishell.h"

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
	expand_var_name(c);
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
