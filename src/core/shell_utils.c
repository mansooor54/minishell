/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 15:04:45 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_all_space(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		if (!ft_isspace((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

static int	check_unclosed_quotes(char *line, t_shell *shell)
{
	if (has_unclosed_quotes(line))
	{
		ft_putendl_fd("minishell: syntax error: unclosed quotes", 2);
		shell->exit_status = 258;
		return (0);
	}
	return (1);
}

static int	process_tokens(char *line, t_pipeline **pl, t_shell *shell)
{
	t_token	*tokens;

	if (needs_continuation(line))
		return (0);
	if (!check_unclosed_quotes(line, shell))
		return (0);
	tokens = lexer(line);
	if (!tokens)
		return (0);
	if (!validate_syntax(tokens, NULL))
	{
		shell->exit_status = 258;
		free_tokens(tokens);
		return (0);
	}
	*pl = parser(tokens);
	free_tokens(tokens);
	return (*pl != NULL);
}

void	process_line(char *line, t_shell *shell)
{
	t_pipeline	*pipeline;

	if (!line || !*line)
		return ;
	if (needs_continuation(line))
		return ;
	if (!process_tokens(line, &pipeline, shell))
		return ;
	executor(pipeline, shell);
	free_pipeline(pipeline);
}
