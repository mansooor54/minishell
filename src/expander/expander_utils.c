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

void	process_dollar(t_exp_ctx *ctx)
{
	(*(ctx->i))++;
	if (ctx->str[*(ctx->i)] == '?')
	{
		expand_exit_status(ctx->result, ctx->j, ctx->exit_status);
		(*(ctx->i))++;
	}
	else
		expand_var_name(ctx);
}

void	update_quote_state(char *str, int i, char *in_quote)
{
	if (!*in_quote && (str[i] == '\'' || str[i] == '"'))
		*in_quote = str[i];
	else if (*in_quote && str[i] == *in_quote)
		*in_quote = 0;
}

char	*expand_variables(char *str, t_env *env, int exit_status)
{
	t_exp_ctx	ctx;
	size_t		max_size;
	int			i;
	int			j;
	char		in_quote;
	char		*result;

	if (!str)
		return (NULL);
	max_size = ft_strlen(str) * 10 + 4096;
	result = malloc(max_size);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	in_quote = 0;
	ctx.str = str;
	ctx.result = result;
	ctx.i = &i;
	ctx.j = &j;
	ctx.in_quote = in_quote;
	ctx.env = env;
	ctx.exit_status = exit_status;
	while (str[i] && j < (int)max_size - 1)
	{
		update_quote_state(str, i, &in_quote);
		ctx.in_quote = in_quote;
		if (str[i] == '$' && in_quote != '\'')
			process_dollar(&ctx);
		else
			result[j++] = str[i++];
	}
	result[j] = '\0';
	return (result);
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
