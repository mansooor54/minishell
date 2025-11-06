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

static t_redir	*create_redir(t_token_type type, char *file)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = ft_strdup(file);
	redir->next = NULL;
	return (redir);
}

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
		ft_putendl_fd("minishell: syntax error near unexpected token `newline'", 2);
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

t_cmd	*parse_command(t_token **tokens)
{
	t_cmd	*cmd;
	t_redir	*new_redir;
	int		i;
	int		arg_count;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->expanded = 0;
	arg_count = count_args(*tokens);
	cmd->args = malloc(sizeof(char *) * (arg_count + 1));
	cmd->redirs = NULL;
	i = 0;
	while (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		cmd->args[i++] = ft_strdup((*tokens)->value);
		*tokens = (*tokens)->next;
		while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN
				|| (*tokens)->type == TOKEN_REDIR_OUT
				|| (*tokens)->type == TOKEN_REDIR_APPEND
				|| (*tokens)->type == TOKEN_REDIR_HEREDOC))
		{
			new_redir = parse_single_redirection(tokens);
			if (new_redir)
				append_redir(&cmd->redirs, new_redir);
			else
				break ;
		}
	}
	cmd->args[i] = NULL;
	cmd->next = NULL;
	return (cmd);
}

void	free_pipeline(t_pipeline *pipeline)
{
	t_pipeline	*tmp_pipe;
	t_cmd		*tmp_cmd;
	t_redir		*tmp_redir;

	while (pipeline)
	{
		tmp_pipe = pipeline;
		while (pipeline->cmds)
		{
			tmp_cmd = pipeline->cmds;
			free_array(tmp_cmd->args);
			while (tmp_cmd->redirs)
			{
				tmp_redir = tmp_cmd->redirs;
				tmp_cmd->redirs = tmp_redir->next;
				free(tmp_redir->file);
				free(tmp_redir);
			}
			pipeline->cmds = tmp_cmd->next;
			free(tmp_cmd);
		}
		pipeline = pipeline->next;
		free(tmp_pipe);
	}
}