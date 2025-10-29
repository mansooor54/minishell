/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 16:15:35 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 16:15:52 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*last_dir(const char *path)
{
	size_t i = 0;
	size_t last = 0;

	if (!path || path[0] == '\0')
		return ((char *)path);
	while (path[i])
	{
		if (path[i] == '/' && path[i + 1] != '\0')
			last = i + 1;
		i++;
	}
	return ((char *)(path + last));
}

char	*build_prompt(t_shell *sh)
{
	char	cwd[PATH_MAX];
	char	*name;
	char	*out;
	size_t	len;
	const char *prefix_a = "minishell$ ";
	const char *prefix_b1 = "minishell(";
	const char *prefix_b2 = ")$ ";

	(void)sh;
	if (!getcwd(cwd, sizeof(cwd)))
		return (ft_strdup("minishell$ "));
	name = last_dir(cwd);
	if (ft_strcmp(name, "minishell") == 0)
		return (ft_strdup("minishell$ "));
	len = ft_strlen(prefix_b1) + ft_strlen(name)
		+ ft_strlen(prefix_b2);
	out = malloc(len + 1);
	if (!out)
		return (ft_strdup(prefix_a));
	/* build "minishell(<dir>)$ " */
	{
		size_t p = 0;
		for (size_t i = 0; prefix_b1[i]; i++) out[p++] = prefix_b1[i];
		for (size_t i = 0; name[i]; i++)      out[p++] = name[i];
		for (size_t i = 0; prefix_b2[i]; i++) out[p++] = prefix_b2[i];
		out[p] = '\0';
	}
	return (out);
}
