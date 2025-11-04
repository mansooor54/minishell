/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Create a new redirection node
** Allocates and initializes a redirection structure
*/
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

/*
** Parse redirections from token stream
** Extracts all redirection operators and their target files
*/
t_redir	*parse_redirections(t_token **tokens)
{
	t_redir	*redirs;
	t_redir	*new_redir;
	t_redir	*current;

	redirs = NULL;
	while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN
			|| (*tokens)->type == TOKEN_REDIR_OUT
			|| (*tokens)->type == TOKEN_REDIR_APPEND
			|| (*tokens)->type == TOKEN_REDIR_HEREDOC))
	{
		new_redir = create_redir((*tokens)->type, (*tokens)->next->value);
		if (!redirs)
			redirs = new_redir;
		else
		{
			current = redirs;
			while (current->next)
				current = current->next;
			current->next = new_redir;
		}
		*tokens = (*tokens)->next->next;
	}
	return (redirs);
}

/*
** Count arguments in token stream until pipe or logical operator
** Returns the number of word tokens found
*/
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
			tokens = tokens->next->next;
	}
	return (count);
}

/*
** Parse a single command with its arguments and redirections
** Builds a command structure from tokens until pipe or end
*/
t_cmd	*parse_command(t_token **tokens)
{
	t_cmd	*cmd;
	int		i;
	int		arg_count;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	arg_count = count_args(*tokens);
	cmd->args = malloc(sizeof(char *) * (arg_count + 1));
	i = 0;
	while (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		cmd->args[i++] = ft_strdup((*tokens)->value);
		*tokens = (*tokens)->next;
		cmd->redirs = parse_redirections(tokens);
	}
	cmd->args[i] = NULL;
	cmd->next = NULL;
	return (cmd);
}

/*
** Free all commands in the pipeline
** Recursively frees command arguments and redirections
*/
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
