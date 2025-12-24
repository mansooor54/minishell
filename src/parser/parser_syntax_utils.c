/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 12:20:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/23 12:20:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_separator_token(t_token *t)
{
	if (!t)
		return (0);
	if (t->type == TOKEN_PIPE)
		return (1);
	if (t->type == TOKEN_AND)
		return (1);
	if (t->type == TOKEN_OR)
		return (1);
	if (t->type == TOKEN_SEMICOLON)
		return (1);
	return (0);
}

int	check_redirection_pair(t_token *t, t_token *next)
{
	if (is_redirection(t) && is_redirection(next))
	{
		print_syntax_error(next);
		return (0);
	}
	if (is_redirection(t) && !is_valid_word(next))
	{
		print_syntax_error(next);
		return (0);
	}
	return (1);
}

int	check_control_operator(t_token *t, t_token *next)
{
	if (is_control_operator(t) && !is_valid_word(next) && !is_redirection(next))
	{
		print_syntax_error(next);
		return (0);
	}
	return (1);
}

int	check_semicolon(t_token *t, t_token *next)
{
	if (t->type == TOKEN_SEMICOLON
		&& (is_separator_token(next) || is_redirection(next)))
	{
		print_syntax_error(next);
		return (0);
	}
	return (1);
}
