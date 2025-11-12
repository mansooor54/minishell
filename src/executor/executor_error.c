/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_error.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:12:28 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:41:19 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// executor_error.c
void	cmd_not_found(char *name)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(name, 2);
	ft_putendl_fd(": command not found", 2);
}

// // in your exec path resolution
// if (!path_found)
// {
// 	cmd_not_found(argv[0]);  // argv[0] == "\\", shown as "\"
// 	shell->exit_status = 127;
// 	return;
// }
