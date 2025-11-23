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

#include "../../minishell.h"

/* Combined validation for token sequences */
/* line 35 two redirections in a row: always a syntax error */
/* line 42 redirection must be followed by a word (filename) */
/*
** line 54 control operator (|, &&, ||, ;) rules:
** - cannot be followed by another separator
** - cannot be followed by end of input (handled in validate_last_token)
** - CAN be followed by a redirection (command starting with redirs)
*/
/* line 63 semicolon specific: '; ;', '; |', '; >' etc. are invalid */
/* Combined validation for token sequences */
static int	validate_token_pair(t_token *t, t_token *next)
{
	if (!check_redirection_pair(t, next))
		return (0);
	if (!check_control_operator(t, next))
		return (0);
	if (!check_semicolon(t, next))
		return (0);
	return (1);
}

/* Validate first token */
static int	validate_first_token(t_token *first)
{
	if (is_separator_token(first))
	{
		print_syntax_error(first);
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}

/* Validate last token */
/*
** line 99 Line cannot end with:
** - a separator (|, &&, ||, ;)
** - a redirection without a target
*/
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
