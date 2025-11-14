/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:05:53 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:06:32 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_unexpected(char *s)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(s, 2);
	ft_putendl_fd("'", 2);
}

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
