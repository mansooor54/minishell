/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 11:20:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** init_shell - Initialize the shell structure
**
** This function sets up the initial state of the shell by:
** 1. Initializing the environment variables from the system envp
** 2. Setting the exit status to 0 (success)
** 3. Setting the should_exit flag to 0 (continue running)
**
** @param shell: Pointer to the shell structure to initialize
** @param envp: Array of environment variable strings from main
**
** Return: void
*/
void	init_shell(t_shell *shell, char **envp)
{
	shell->env = init_env(envp);
	shell->exit_status = 0;
	shell->should_exit = 0;
}
