/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_vars.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:23:36 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

/* reads name at c->str[c->i], appends value (or nothing) into c->result,
   and advances c->i past the name, updates c->j accordingly */
void	expand_var_name(t_exp_ctx *ctx)
{
	int		start;
	char	*key;
	char	*val;

	start = ctx->i;
	while (ft_isalnum((unsigned char)ctx->str[ctx->i])
		|| ctx->str[ctx->i] == '_')
		ctx->i++;
	if (ctx->i == start)
	{
		ctx->result[ctx->j++] = '$';
		return ;
	}
	key = ft_substr(ctx->str, start, ctx->i - start);
	val = get_env_value(ctx->env, key);
	if (val)
		while (*val)
			ctx->result[ctx->j++] = *val++;
	free(key);
}
