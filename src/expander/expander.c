/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:26:29 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** get_env_value - Get environment variable value by key
**
** Searches through the environment linked list to find a matching key
** and returns its associated value.
**
** @param env: Pointer to environment linked list
** @param key: Key string to search for
**
** Return: Value string if found, NULL otherwise
*/
char	*get_env_value(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
** process_quote_char - Process a single character for quote removal
**
** Handles quote state tracking and copies non-quote characters.
**
** @param str: Source string
** @param result: Destination string
** @param i: Pointer to source index
** @param j: Pointer to destination index
** @param quote: Pointer to current quote character
**
** Return: void
*/
static void	process_quote_char(t_quote_ctx *ctx)
{
	char	c;

	c = ctx->str[ctx->i];
	if (!ctx->quote && (c == '\'' || c == '"'))
		ctx->quote = c;
	else if (ctx->quote && c == ctx->quote)
		ctx->quote = 0;
	else
		ctx->res[ctx->j++] = c;
	ctx->i++;
}

/*
** remove_quotes - Remove quotes from a string
**
** Handles both single and double quotes, removing them while preserving
** the content between quotes.
**
** @param str: String to process
**
** Return: Newly allocated string without quotes
*/
char	*remove_quotes(char *str)
{
	t_quote_ctx	ctx;

	ctx.str = str;
	ctx.res = malloc(ft_strlen(str) + 1);
	if (!ctx.res)
		return (NULL);
	ctx.i = 0;
	ctx.j = 0;
	ctx.quote = 0;
	while (ctx.str[ctx.i])
		process_quote_char(&ctx);
	ctx.res[ctx.j] = '\0';
	return (ctx.res);
}
/*
** expand_exit_status - Expand $? to exit status value
**
** Converts the exit status integer to a string and copies it to the result.
**
** @param result: Destination buffer
** @param j: Pointer to destination index
** @param exit_status: Exit status value to expand
**
** Return: void
*/

void	expand_exit_status(char *result, int *j, int exit_status)
{
	char	*tmp;
	int		len;

	if (!result || !j)
		return ;
	tmp = ft_itoa(exit_status);
	if (!tmp)
		return ;
	len = ft_strlen(tmp);
	ft_strcpy(&result[*j], tmp);
	*j += len;
	free(tmp);
}

/*
** expand_var_name - Expand environment variable by name
**
** Extracts variable name from input string and replaces it with its value
** from the environment. Now includes bounds checking.
**
** @param ctx: Expansion context structure
**
** Return: void
*/
void	expand_var_name(t_exp_ctx *ctx)
{
	char	var_name[256];
	char	*var_value;
	int		k;

	if (!ctx || !ctx->str || !ctx->i || !ctx->j || !ctx->result)
		return ;
	k = 0;
	while (ctx->str[*(ctx->i)] && (ft_isalnum(ctx->str[*(ctx->i)])
			|| ctx->str[*(ctx->i)] == '_') && k < 255)
		var_name[k++] = ctx->str[(*(ctx->i))++];
	var_name[k] = '\0';
	while (ctx->str[*(ctx->i)] && (ft_isalnum(ctx->str[*(ctx->i)])
			|| ctx->str[*(ctx->i)] == '_'))
		(*(ctx->i))++;
	var_value = get_env_value(ctx->env, var_name);
	if (var_value)
	{
		ft_strcpy(&ctx->result[*(ctx->j)], var_value);
		*(ctx->j) += ft_strlen(var_value);
	}
}
