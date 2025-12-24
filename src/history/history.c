/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/12/25 00:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*history_path_from_env(t_env *env)
{
	char	*home;
	char	*p;
	char	*tmp;

	home = get_env_value(env, "HOME");
	if (!home)
		return (ft_strdup(".minishell_history"));
	tmp = ft_strjoin(home, "/");
	if (!tmp)
		return (NULL);
	p = ft_strjoin(tmp, ".minishell_history");
	free(tmp);
	return (p);
}

void	free_history(t_hist *hist)
{
	t_hist	*tmp;

	while (hist)
	{
		tmp = hist->next;
		free(hist->line);
		free(hist);
		hist = tmp;
	}
}

void	add_hist_node(t_shell *shell, char *line)
{
	t_hist	*new;
	t_hist	*curr;

	new = malloc(sizeof(t_hist));
	if (!new)
		return ;
	new->line = ft_strdup(line);
	new->next = NULL;
	if (!new->line)
	{
		free(new);
		return ;
	}
	if (!shell->history)
		shell->history = new;
	else
	{
		curr = shell->history;
		while (curr->next)
			curr = curr->next;
		curr->next = new;
	}
	shell->hist_count++;
}
