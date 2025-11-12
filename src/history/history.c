/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 14:41:56 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static char	*g_hist = NULL;

static int	is_blank(const char *s)
{
	int	i;

	if (!s)
		return (1);
	i = 0;
	while (s[i] == ' ' || s[i] == '\t')
		i++;
	return (s[i] == '\0');
}

static char	*history_path_from_env(t_env *env)
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

int	history_init(t_env *env)
{
	g_hist = history_path_from_env(env);
	if (!g_hist)
		return (0);
	read_history(g_hist);
	stifle_history(1000);
	return (1);
}

void	history_add_line(const char *line)
{
	HIST_ENTRY	*last;

	if (is_blank(line))
		return ;
	last = NULL;
	if (history_length > 0)
		last = history_get(history_length);
	if (!last || ft_strcmp(last->line, line) != 0)
		add_history((char *)line);
}

void	history_save(void)
{
	if (!g_hist)
		return ;
	write_history(g_hist);
}
