/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 14:00:49 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_redir	*create_redir(t_token_type type, char *file)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = ft_strdup(file);
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	redir->next = NULL;
	return (redir);
}

void	append_redir(t_redir **head, t_redir *new_redir)
{
	t_redir	*current;

	if (!new_redir)
		return ;
	if (!*head)
	{
		*head = new_redir;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_redir;
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
