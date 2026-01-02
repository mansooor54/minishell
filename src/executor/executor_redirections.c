/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 12:50:35 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	process_single_redirection(t_redir *redir, t_shell *shell)
{
	if (redir->type == TOKEN_REDIR_IN)
		return (handle_input(redir->file));
	else if (redir->type == TOKEN_REDIR_OUT)
		return (handle_output(redir->file, 0));
	else if (redir->type == TOKEN_REDIR_APPEND)
		return (handle_output(redir->file, 1));
	else if (redir->type == TOKEN_REDIR_HEREDOC)
		return (handle_heredoc(redir->file, shell));
	return (0);
}

int	setup_redirections(t_redir *redirs, t_shell *shell)
{
	while (redirs)
	{
		if (process_single_redirection(redirs, shell) == -1)
			return (-1);
		redirs = redirs->next;
	}
	return (0);
}
