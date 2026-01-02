/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:03:10 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
/*
** Identify and create operator token
** Handles |, ||, &&, <, <<, >, >>
*/

t_token	*get_operator_token(char **input)
{
	t_token		*tok;

	tok = try_or_pipe(input);
	if (tok)
		return (tok);
	tok = try_and(input);
	if (tok)
		return (tok);
	tok = try_inredir(input);
	if (tok)
		return (tok);
	return (try_outredir(input));
}
