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

volatile sig_atomic_t	g_signal = 0;

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	write(1, "\033[2J\033[H", 7);
	print_logo();
	init_shell(&shell, envp);
	setup_signals();
	history_init(&shell);
	shell_loop(&shell);
	history_save(&shell);
	rl_clear_history();
	free_env(shell.env);
	free(shell.history_path);
	return (shell.exit_status);
}
