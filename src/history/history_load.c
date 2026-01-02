/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_load.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 00:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/12/25 00:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*read_file_content(int fd)
{
	char	buf[4096];
	char	*content;
	char	*tmp;
	int		n;

	content = ft_strdup("");
	if (!content)
		return (NULL);
	n = 1;
	while (n > 0)
	{
		n = read(fd, buf, 4095);
		if (n > 0)
		{
			buf[n] = '\0';
			tmp = ft_strjoin(content, buf);
			free(content);
			content = tmp;
			if (!content)
				return (NULL);
		}
	}
	return (content);
}

static void	load_lines_to_hist(t_shell *shell, char *content)
{
	int		i;
	int		start;
	char	*line;

	i = 0;
	start = 0;
	while (content[i])
	{
		if (content[i] == '\n')
		{
			line = ft_substr(content, start, i - start);
			if (line && line[0])
			{
				add_history(line);
				add_hist_node(shell, line);
			}
			free(line);
			start = i + 1;
		}
		i++;
	}
}

static void	load_history_file(t_shell *shell)
{
	int		fd;
	char	*content;

	fd = open(shell->history_path, O_RDONLY);
	if (fd == -1)
		return ;
	content = read_file_content(fd);
	close(fd);
	if (!content)
		return ;
	load_lines_to_hist(shell, content);
	free(content);
}

int	history_init(t_shell *shell)
{
	shell->history = NULL;
	shell->hist_count = 0;
	shell->history_path = history_path_from_env(shell->env);
	if (!shell->history_path)
		return (0);
	load_history_file(shell);
	return (1);
}
