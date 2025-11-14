/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraser_syntax_check.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:08:29 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* helper: is a separator token (for syntax) */
static int	is_separator_token(t_token *t)
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

/* Combined validation for token sequences */
static int	validate_token_pair(t_token *t, t_token *next)
{
	if (is_redirection(t) && is_redirection(next))
	{
		print_syntax_error(t);
		g_shell.exit_status = 258;
		return (0);
	}
	if ((is_redirection(t) || is_control_operator(t)) && !is_valid_word(next))
	{
		print_syntax_error(next);
		g_shell.exit_status = 258;
		return (0);
	}
	if (t->type == TOKEN_SEMICOLON
		&& (is_separator_token(next) || is_redirection(next)))
	{
		print_syntax_error(next);
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}

/* Validate first token */
static int	validate_first_token(t_token *first)
{
	if (is_separator_token(first) || is_redirection(first))
	{
		print_syntax_error(first);
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}

/* Validate last token */
static int	validate_last_token(t_token *last)
{
	if (last && (is_separator_token(last) || is_redirection(last)))
	{
		if (last->type == TOKEN_SEMICOLON)
			print_syntax_error(last);
		else
			print_syntax_error(NULL);
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}

/* Main validation function */
int	validate_syntax(t_token *tokens, t_shell *shell)
{
	t_token	*current;
	t_token	*last;

	(void)shell;
	if (!tokens)
		return (1);
	if (!validate_first_token(tokens))
		return (0);
	current = tokens;
	last = tokens;
	while (current && current->next)
	{
		if (!validate_token_pair(current, current->next))
			return (0);
		current = current->next;
		last = current;
	}
	if (!validate_last_token(last))
		return (0);
	return (1);
}
