/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraser_check_token.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 12:32:12 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 13:52:15 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/* Token validation functions */
int	is_valid_word(t_token *token)
{
	return (token && token->type == TOKEN_WORD);
}

int	is_control_operator(t_token *token)
{
	return (token && (token->type == TOKEN_PIPE
			|| token->type == TOKEN_AND
			|| token->type == TOKEN_OR));
}

int	is_redirection(t_token *token)
{
	return (token && (token->type == TOKEN_REDIR_IN
			|| token->type == TOKEN_REDIR_OUT
			|| token->type == TOKEN_REDIR_APPEND
			|| token->type == TOKEN_REDIR_HEREDOC));
}

int	is_gt(t_token *t)
{
	return (t && (t->type == TOKEN_REDIR_OUT || t->type == TOKEN_REDIR_APPEND));
}

int	is_lt(t_token *t)
{
	return (t && (t->type == TOKEN_REDIR_IN || t->type == TOKEN_REDIR_HEREDOC));
}
