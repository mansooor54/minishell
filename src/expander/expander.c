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

char	*remove_quotes(char *str)
{
	t_quote_ctx	ctx;

	if (!str)
		return (NULL);
	if (ft_strcmp(str, "\"\"") == 0 || ft_strcmp(str, "''") == 0)
		return (ft_strdup(""));
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
