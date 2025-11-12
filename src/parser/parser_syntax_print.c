/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraser_syntax_print.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:09:48 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	tok_op_len(t_token *t)
{
	if (!t)
		return (0);
	if (t->type == TOKEN_REDIR_APPEND || t->type == TOKEN_REDIR_HEREDOC)
		return (2);
	if (t->type == TOKEN_REDIR_OUT || t->type == TOKEN_REDIR_IN)
		return (1);
	return (0);
}

/* print_unexpected is now in parser_error.c */

/* replace only the '<' branch inside print_run_error(...) */
/* line 43: counts 1 for '<', 2 for '<<' */
/* line 56: return total >= 6 */
static void	handle_lt_run(t_token *t)
{
	int			total;
	t_token		*cur;

	total = 0;
	cur = t;
	while (is_lt(cur))
	{
		total += tok_op_len(cur);
		cur = cur->next;
	}
	if (total <= 3)
		return (print_unexpected("newline"));
	if (total == 4)
		return (print_unexpected("<"));
	if (total == 5)
		return (print_unexpected("<<"));
	return (print_unexpected("<<<"));
}

/* choose correct unexpected token for runs like >>>> or <<<< */
/* line 75: >>>, >>>> → `>>' */
void	print_run_error(t_token *t)
{
	int			total;
	t_token		*cur;

	total = 0;
	cur = t;
	if (is_gt(t))
	{
		while (is_gt(cur))
		{
			total += tok_op_len(cur);
			cur = cur->next;
		}
		if (total > 3)
			ft_putendl_fd(ERR_REDIR_APPEND, 2);
		else if (total == 2 && (!cur || is_control_operator(cur)
				|| is_redirection(cur)))
			ft_putendl_fd(ERR_NEWLINE, 2);
		else
			ft_putendl_fd(ERR_CONSECUTIVE_REDIR, 2);
		return ;
	}
	if (is_lt(t))
		return (handle_lt_run(t));
}

/* replace your redir branch with this */
void	print_syntax_error(t_token *token)
{
	if (!token)
	{
		print_unexpected("newline");
		return ;
	}
	if (token->type == TOKEN_SEMI)
	{
		//print_unexpected(";");
		ft_putendl_fd(ERR_SEMI, 2);
		return ;
	}
	if (is_redirection(token) && is_redirection(token->next))
	{
		print_run_error(token);
		return ;
	}
	if (token->type == TOKEN_PIPE)
		ft_putendl_fd(ERR_PIPE, 2);
	else if (token->type == TOKEN_AND)
		ft_putendl_fd(ERR_AND, 2);
	else if (token->type == TOKEN_OR)
		ft_putendl_fd(ERR_OR, 2);
	else if (token->type == TOKEN_REDIR_IN)
		ft_putendl_fd(ERR_REDIR_IN, 2);
	else if (token->type == TOKEN_REDIR_OUT)
		ft_putendl_fd(ERR_REDIR_OUT, 2);
	else if (token->type == TOKEN_REDIR_APPEND)
		ft_putendl_fd(ERR_REDIR_APPEND, 2);
	else if (token->type == TOKEN_REDIR_HEREDOC)
		ft_putendl_fd(ERR_REDIR_HEREDOC, 2);
}
