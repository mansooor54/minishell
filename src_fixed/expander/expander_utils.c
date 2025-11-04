/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/04 15:45:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include "expander.h"

/*
** process_dollar - Process $ character for variable expansion
**
** Handles both $? (exit status) and $VAR (environment variable) expansion.
** Uses context structure to avoid >4 arguments.
**
** @param ctx: Expansion context structure
**
** Return: void
*/
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

/*
** update_quote_state - Update quote state while processing string
**
** Tracks whether we are inside single or double quotes.
**
** @param str: Source string
** @param i: Current index
** @param in_quote: Pointer to quote state variable
**
** Return: void
*/
static void	update_quote_state(char *str, int i, char *in_quote)
{
	if (!*in_quote && (str[i] == '\'' || str[i] == '"'))
		*in_quote = str[i];
	else if (*in_quote && str[i] == *in_quote)
		*in_quote = 0;
}

/*
** expand_variables - Expand environment variables in a string
**
** Handles $VAR and $? (exit status) expansion.
** Does not expand inside single quotes.
**
** @param str: String to expand
** @param env: Environment linked list
** @param exit_status: Current exit status
**
** Return: Newly allocated expanded string
*/
char	*expand_variables(char *str, t_env *env, int exit_status)
{
	t_exp_ctx	ctx;
	char		*result;
	int			i;
	int			j;
	char		in_quote;

	result = malloc(4096);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	in_quote = 0;
	ctx = (t_exp_ctx){str, result, &i, &j, env, exit_status};
	while (str[i])
	{
		update_quote_state(str, i, &in_quote);
		if (str[i] == '$' && in_quote != '\'')
			process_dollar(&ctx);
		else
			result[j++] = str[i++];
	}
	result[j] = '\0';
	return (result);
}

/*
** expand_arg - Expand and unquote a single argument
**
** Expands environment variables and removes quotes from an argument string.
**
** @param arg: Pointer to argument string (will be freed and replaced)
** @param env: Environment linked list
** @param exit_status: Current exit status
**
** Return: void
*/
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

/*
** expand_cmd_args - Expand all arguments in a command
**
** Iterates through all arguments and expands environment variables
** and removes quotes.
**
** @param cmd: Command structure
** @param env: Environment linked list
** @param exit_status: Current exit status
**
** Return: void
*/
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
