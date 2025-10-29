/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:24:06 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 15:24:06 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* count tokens separated by spaces/tabs */
static int	count_tokens(char *line)
{
	int i = 0;
	int count = 0;
	int in_word = 0;

	while (line[i])
	{
		if (line[i] == ' ' || line[i] == '\t')
		{
			line[i] = '\0';
			in_word = 0;
		}
		else if (!in_word)
		{
			in_word = 1;
			count++;
		}
		i++;
	}
	return (count);
}

static void	fill_tokens(char *line, char **argv)
{
	int i = 0;
	int j = 0;
	int in_word = 0;

	while (line[i])
	{
		if (line[i] == '\0')
			in_word = 0;
		else if (!in_word)
		{
			in_word = 1;
			argv[j++] = &line[i];
		}
		i++;
	}
	argv[j] = NULL;
}

t_cmd	*parse_line_basic(char *line)
{
	t_cmd	*cmd;
	int		argc;

	if (!line)
		return (NULL);
	/* skip "" or spaces only */
	{
		int k = 0;
		int only_space = 1;
		while (line[k])
		{
			if (line[k] != ' ' && line[k] != '\t')
			{
				only_space = 0;
				break ;
			}
			k++;
		}
		if (only_space)
			return (NULL);
	}
	argc = count_tokens(line);
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = malloc(sizeof(char *) * (argc + 1));
	if (!cmd->argv)
	{
		free(cmd);
		return (NULL);
	}
	fill_tokens(line, cmd->argv);
	return (cmd);
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	if (cmd->argv)
		free(cmd->argv);
	free(cmd);
}
