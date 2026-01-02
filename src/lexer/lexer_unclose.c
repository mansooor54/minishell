/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_unclose.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/12/25 00:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	has_unclosed_quotes(char *s)
{
	size_t	i;
	char	in_quote;

	i = 0;
	in_quote = 0;
	while (s[i])
	{
		if (!in_quote && (s[i] == '\'' || s[i] == '"'))
			in_quote = s[i];
		else if (in_quote && s[i] == in_quote)
			in_quote = 0;
		i++;
	}
	return (in_quote != 0);
}
