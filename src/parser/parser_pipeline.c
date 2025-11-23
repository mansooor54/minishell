/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 16:18:14 by malmarzo         ###   ########.fr       */
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
t_cmd	*parse_pipe_sequence(t_token **tokens)
{
	t_cmd	*cmds;
	t_cmd	*new_cmd;
	t_cmd	*current;

	cmds = NULL;
	while (*tokens && (*tokens)->type != TOKEN_AND
		&& (*tokens)->type != TOKEN_OR
		&& (*tokens)->type != TOKEN_SEMICOLON)
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

/* helpers */
static void	append_pipeline(t_pipeline **head, t_pipeline *new_node)
{
	t_pipeline	*cur;

	if (!new_node)
		return ;
	if (!*head)
	{
		*head = new_node;
		return ;
	}
	cur = *head;
	while (cur->next)
		cur = cur->next;
	cur->next = new_node;
}

/* helpers */
void	set_logic_and_advance(t_pipeline *pl, t_token **tokens)
{
	if (!tokens || !*tokens)
		return ;
	if ((*tokens)->type == TOKEN_AND
		|| (*tokens)->type == TOKEN_OR
		|| (*tokens)->type == TOKEN_SEMICOLON)
	{
		pl->logic_op = (*tokens)->type;
		*tokens = (*tokens)->next;
	}
}

t_pipeline	*parser(t_token *tokens)
{
	t_pipeline	*head;
	t_pipeline	*node;

	head = NULL;
	while (tokens)
	{
		while (tokens && tokens->type == TOKEN_SEMICOLON)
			tokens = tokens->next;
		if (!tokens || tokens->type == TOKEN_EOF)
			break ;
		node = create_pipeline();
		node->cmds = parse_pipe_sequence(&tokens);
		set_logic_and_advance(node, &tokens);
		append_pipeline(&head, node);
		if (!tokens || tokens->type == TOKEN_EOF)
			break ;
		if (tokens->type == TOKEN_SEMICOLON)
			tokens = tokens->next;
	}
	return (head);
}
