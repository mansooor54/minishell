/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 15:04:45 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_shell	g_shell;

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	write(1, "\033[2J\033[H", 7);
	print_logo();
	init_shell(&g_shell, envp);
	setup_signals();
	history_init(&g_shell);
	shell_loop(&g_shell);
	history_save(&g_shell);
	rl_clear_history();
	free_env(g_shell.env);
	free(g_shell.history_path);
	return (g_shell.exit_status);
}
