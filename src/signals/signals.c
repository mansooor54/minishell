/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:00:00 by student           #+#    #+#             */
/*   Updated: 2025/11/02 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
** Handle SIGINT (Ctrl-C)
** Displays new prompt without terminating shell
*/
void	handle_sigint(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_shell.exit_status = 130;
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
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}
