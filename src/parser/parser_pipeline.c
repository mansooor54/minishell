/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:00:00 by student           #+#    #+#             */
/*   Updated: 2025/11/02 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Create a new pipeline node
** Initializes pipeline with commands and logical operator
*/
static t_pipeline	*create_pipeline(void)
{
	t_pipeline	*pipeline;

	pipeline = malloc(sizeof(t_pipeline));
	if (!pipeline)
		return (NULL);
	pipeline->cmds = NULL;
	pipeline->logic_op = TOKEN_EOF;
	pipeline->next = NULL;
	return (pipeline);
}

/*
** Parse commands separated by pipes
** Builds a linked list of commands in a single pipeline
*/
static t_cmd	*parse_pipe_sequence(t_token **tokens)
{
	t_cmd	*cmds;
	t_cmd	*new_cmd;
	t_cmd	*current;

	cmds = NULL;
	while (*tokens && (*tokens)->type != TOKEN_AND
		&& (*tokens)->type != TOKEN_OR)
	{
		new_cmd = parse_command(tokens);
		if (!cmds)
			cmds = new_cmd;
		else
		{
			current = cmds;
			while (current->next)
				current = current->next;
			current->next = new_cmd;
		}
		if (*tokens && (*tokens)->type == TOKEN_PIPE)
			*tokens = (*tokens)->next;
		else
			break ;
	}
	return (cmds);
}

/*
** Main parser function
** Parses tokens into pipeline structures with logical operators
** Handles && and || for bonus part
*/
t_pipeline	*parser(t_token *tokens)
{
	t_pipeline	*pipelines;
	t_pipeline	*new_pipeline;
	t_pipeline	*current;

	pipelines = NULL;
	while (tokens)
	{
		new_pipeline = create_pipeline();
		new_pipeline->cmds = parse_pipe_sequence(&tokens);
		if (tokens && (tokens->type == TOKEN_AND
				|| tokens->type == TOKEN_OR))
		{
			new_pipeline->logic_op = tokens->type;
			tokens = tokens->next;
		}
		if (!pipelines)
			pipelines = new_pipeline;
		else
		{
			current = pipelines;
			while (current->next)
				current = current->next;
			current->next = new_pipeline;
		}
		if (!tokens || tokens->type == TOKEN_EOF)
			break ;
	}
	return (pipelines);
}
