/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator_type.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:02:14 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_token	*try_or_pipe(char **input)
{
	if (**input != '|')
		return (NULL);
	if (*(*input + 1) == '|')
	{
		*input += 2;
		return (create_token(TOKEN_OR, "||"));
	}
	(*input)++;
	return (create_token(TOKEN_PIPE, "|"));
}

t_token	*try_and(char **input)
{
	if (**input == '&' && *(*input + 1) == '&')
	{
		*input += 2;
		return (create_token(TOKEN_AND, "&&"));
	}
	return (NULL);
}

t_token	*try_inredir(char **input)
{
	if (**input == '<' && *(*input + 1) == '<')
	{
		*input += 2;
		return (create_token(TOKEN_REDIR_HEREDOC, "<<"));
	}
	if (**input == '<')
	{
		(*input)++;
		return (create_token(TOKEN_REDIR_IN, "<"));
	}
	return (NULL);
}

t_token	*try_outredir(char **input)
{
	if (**input == '>' && *(*input + 1) == '>')
	{
		*input += 2;
		return (create_token(TOKEN_REDIR_APPEND, ">>"));
	}
	if (**input == '>')
	{
		(*input)++;
		return (create_token(TOKEN_REDIR_OUT, ">"));
	}
	return (NULL);
}
