/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 14:32:20 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

#include <sys/ioctl.h>

/*
** Handle SIGINT (Ctrl-C)
** Displays new prompt without terminating shell
*/
void	handle_sigint(int sig)
{
	extern int	rl_done;

	(void)sig;
	g_shell.exit_status = 130;
	if (g_shell.in_heredoc)
	{
		g_shell.heredoc_sigint = 1;
		write(1, "^C\n", 3);
		rl_done = 1;
		ioctl(STDIN_FILENO, TIOCSTI, "\n");
		return ;
	}
	if (g_shell.in_continuation)
	{
		g_shell.sigint_during_read = 1;
		write(1, "^C\n", 3);
		rl_done = 1;
		ioctl(STDIN_FILENO, TIOCSTI, "\n");
		return ;
	}
	write(1, "^C\n", 3);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/*
** Handle SIGQUIT (Ctrl-\)
** Does nothing in interactive mode
*/
void	handle_sigquit(int sig)
{
	(void)sig;
}

/*
** Setup signal handlers for the shell
** SIGINT displays new prompt, SIGQUIT is ignored
*/
void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = handle_sigquit;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGQUIT, &sa, NULL);
}
