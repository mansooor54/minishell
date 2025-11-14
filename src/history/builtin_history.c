/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_history.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 14:33:12 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_history(char **args)
{
	HIST_ENTRY	**list;
	int			i;

	(void)args;
	list = history_list();
	if (!list)
		return (0);
	i = 0;
	while (list[i])
	{
		ft_putnbr_fd(i + 1, 1);
		ft_putstr_fd("  ", 1);
		ft_putendl_fd(list[i]->line, 1);
		i++;
	}
	return (0);
}
