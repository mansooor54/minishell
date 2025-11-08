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
	"minishell: syntax error near unexpected token `<'"

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

/* Add function to count consecutive redirections */
static int	count_consecutive_redirections(t_token *token)
{
	int	count;

	count = 0;
	while (token && is_redirection(token))
	{
		count++;
		token = token->next;
	}
	return (count);
}

/* Update print_syntax_error function */
void	print_syntax_error(t_token *token)
{
	if (!token)
		ft_putendl_fd(ERR_NEWLINE, 2);
	else if (is_redirection(token) && is_redirection(token->next))
	{
		if (count_consecutive_redirections(token) > 2)
			ft_putendl_fd(ERR_MULTIPLE_REDIR, 2);
		else
			ft_putendl_fd(ERR_CONSECUTIVE_REDIR, 2);
	}
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
