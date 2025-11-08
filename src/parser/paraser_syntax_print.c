/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraser_syntax_print.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 16:18:14 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* Error messages as constants */
#define ERR_NEWLINE "minishell: syntax error near unexpected token `newline'"
#define ERR_PIPE "minishell: syntax error near unexpected token `|'"
#define ERR_AND "minishell: syntax error near unexpected token `&&'"
#define ERR_OR "minishell: syntax error near unexpected token `||'"
#define ERR_REDIR_IN "minishell: syntax error near unexpected token `<'"
#define ERR_REDIR_OUT "minishell: syntax error near unexpected token `>'"
#define ERR_REDIR_APPEND "minishell: syntax error near unexpected token `>>'"
#define ERR_REDIR_HEREDOC "minishell: syntax error near unexpected token `<<'"
#define ERR_MULTIPLE_REDIR "minishell: syntax error near unexpected token `<<<'"
#define ERR_CONSECUTIVE_REDIR \
	"minishell: syntax error near unexpected token `>'"

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

/* helpers */
static int	is_gt(t_token *t)
{
	return (t && (t->type == TOKEN_REDIR_OUT || t->type == TOKEN_REDIR_APPEND));
}

static int	is_lt(t_token *t)
{
	return (t && (t->type == TOKEN_REDIR_IN || t->type == TOKEN_REDIR_HEREDOC));
}

static int	tok_op_len(t_token *t)
{
	if (!t)
		return (0);
	if (t->type == TOKEN_REDIR_APPEND || t->type == TOKEN_REDIR_HEREDOC)
		return (2); /* ">>" or "<<" */
	if (t->type == TOKEN_REDIR_OUT || t->type == TOKEN_REDIR_IN)
		return (1); /* ">" or "<" */
	return (0);
}

/* choose correct unexpected token for runs like >>>> or <<<< */
static void	print_run_error(t_token *t)
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
			ft_putendl_fd(ERR_REDIR_APPEND, 2); /* >>>, >>>> → `>>' */
		else if (total == 2 && (!cur || is_control_operator(cur) || is_redirection(cur)))
			ft_putendl_fd(ERR_NEWLINE, 2);      /* >> + EOF/op → `newline' */
		else
			ft_putendl_fd(ERR_CONSECUTIVE_REDIR, 2); /* > followed by redir → `>' */
		return ;
	}
	if (is_lt(t))
	{
		while (is_lt(cur))
		{
			total += tok_op_len(cur);
			cur = cur->next;
		}
		if (total > 3)
			ft_putendl_fd(ERR_REDIR_IN, 2); /* <<<, <<<< → `<<' */
		else if (total < 4 && (!cur || is_control_operator(cur) || is_redirection(cur)))
			ft_putendl_fd(ERR_NEWLINE, 2);        /* << + EOF/op → `newline' */
		else
			ft_putendl_fd(ERR_REDIR_IN, 2);       /* < followed by redir → `<' */
	}
}

/* replace your redir branch with this */
void	print_syntax_error(t_token *token)
{
	if (!token)
		ft_putendl_fd(ERR_NEWLINE, 2);
	else if (is_redirection(token) && is_redirection(token->next))
		print_run_error(token);
	else if (token->type == TOKEN_PIPE)
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
