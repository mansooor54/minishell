/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 00:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/12/25 00:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static char	*get_last_hist_line(t_shell *shell)
{
	t_hist	*curr;

	if (!shell->history)
		return (NULL);
	curr = shell->history;
	while (curr->next)
		curr = curr->next;
	return (curr->line);
}

void	history_add_line(const char *line, t_shell *shell)
{
	char	*last;
	int		i;

	if (!line)
		return ;
	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	if (line[i] == '\0')
		return ;
	last = get_last_hist_line(shell);
	if (last && ft_strcmp(last, line) == 0)
		return ;
	add_history((char *)line);
	add_hist_node(shell, (char *)line);
}

void	history_save(t_shell *shell)
{
	int		fd;
	t_hist	*curr;

	if (!shell->history_path)
		return ;
	fd = open(shell->history_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return ;
	curr = shell->history;
	while (curr)
	{
		write(fd, curr->line, ft_strlen(curr->line));
		write(fd, "\n", 1);
		curr = curr->next;
	}
	close(fd);
}
