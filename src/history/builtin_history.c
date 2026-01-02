/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_history.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/12/25 00:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	builtin_history(char **args, t_shell *shell)
{
	t_hist	*curr;
	int		i;

	(void)args;
	if (!shell->history)
		return (0);
	curr = shell->history;
	i = 1;
	while (curr)
	{
		ft_putnbr_fd(i, 1);
		ft_putstr_fd("  ", 1);
		ft_putendl_fd(curr->line, 1);
		curr = curr->next;
		i++;
	}
	return (0);
}
