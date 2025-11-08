/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 14:16:03 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_redir	*parse_single_redirection(t_token **tokens)
{
	t_redir	*redir;

	if (!*tokens)
		return (NULL);
	if ((*tokens)->type != TOKEN_REDIR_IN
		&& (*tokens)->type != TOKEN_REDIR_OUT
		&& (*tokens)->type != TOKEN_REDIR_APPEND
		&& (*tokens)->type != TOKEN_REDIR_HEREDOC)
		return (NULL);
	if (!(*tokens)->next || !(*tokens)->next->value)
	{
		ft_putendl_fd(
			"minishell: syntax error near unexpected token "
			"`newline'",
			2);
		return (NULL);
	}
	redir = create_redir((*tokens)->type, (*tokens)->next->value);
	if (!redir)
		return (NULL);
	*tokens = (*tokens)->next->next;
	return (redir);
}

static int	count_args(t_token *tokens)
{
	int	count;

	count = 0;
	while (tokens && tokens->type == TOKEN_WORD)
	{
		count++;
		tokens = tokens->next;
		while (tokens && (tokens->type == TOKEN_REDIR_IN
				|| tokens->type == TOKEN_REDIR_OUT
				|| tokens->type == TOKEN_REDIR_APPEND
				|| tokens->type == TOKEN_REDIR_HEREDOC))
		{
			if (!tokens->next)
				return (count);
			tokens = tokens->next->next;
		}
	}
	return (count);
}

static t_cmd	*new_cmd(int arg_count)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->expanded = 0;
	cmd->args = malloc(sizeof(char *) * (arg_count + 1));
	if (!cmd->args)
	{
		free(cmd);
		return (NULL);
	}
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	consume_redirs(t_token **tokens, t_cmd *cmd)
{
	t_redir	*new_redir;

	while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN
			|| (*tokens)->type == TOKEN_REDIR_OUT
			|| (*tokens)->type == TOKEN_REDIR_APPEND
			|| (*tokens)->type == TOKEN_REDIR_HEREDOC))
	{
		new_redir = parse_single_redirection(tokens);
		if (!new_redir)
			break ;
		append_redir(&cmd->redirs, new_redir);
	}
}

t_cmd	*parse_command(t_token **tokens)
{
	t_cmd	*cmd;
	int		i;
	int		arg_count;

	if (!tokens || !*tokens)
		return (NULL);
	arg_count = count_args(*tokens);
	cmd = new_cmd(arg_count);
	if (!cmd)
		return (NULL);
	i = 0;
	while (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		cmd->args[i++] = ft_strdup((*tokens)->value);
		*tokens = (*tokens)->next;
		consume_redirs(tokens, cmd);
	}
	cmd->args[i] = NULL;
	return (cmd);
}
