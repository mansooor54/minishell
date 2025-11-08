/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraser_syntax_check.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 16:18:14 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* Add check for consecutive redirections */
static int	check_consecutive_redirections(t_token *token)
{
	if (is_redirection(token) && is_redirection(token->next))
	{
		print_syntax_error(token);
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}

/* Add validate_token_sequence function */
static int	validate_token_sequence(t_token *t, t_token *next)
{
	if (!check_consecutive_redirections(t))
		return (0);
	if (is_redirection(t) || is_control_operator(t))
	{
		if (!is_valid_word(next))
		{
			print_syntax_error(next);
			g_shell.exit_status = 258;
			return (0);
		}
	}
	return (1);
}

/* Update validate_syntax function */
int	validate_syntax(t_token *t, t_shell *shell)
{
	t_token	*next;

	(void)shell;
	if (!t)
		return (1);
	if (is_control_operator(t))
	{
		print_syntax_error(t);
		g_shell.exit_status = 258;
		return (0);
	}
	while (t)
	{
		next = t->next;
		if (!validate_token_sequence(t, next))
			return (0);
		t = t->next;
	}
	if (is_control_operator(t) || is_redirection(t))
	{
		print_syntax_error(NULL);
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}
