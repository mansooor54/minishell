/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 16:39:31 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 16:59:44 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
This is the only global variable allowed by the subject.
It stores signal status so the shell can behave like bash.
*/
volatile sig_atomic_t g_signal_status = 0;

/*
SIGINT handler for interactive parent shell.
Behavior required by subject:
- Ctrl+C should:
  - print a newline
  - clear the current input line
  - redisplay a fresh prompt
  - not kill the shell
*/
static void	sigint_handler(int signo)
{
	(void)signo;
	g_signal_status = 130;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

/*
SIGQUIT handler for interactive parent shell.
Behavior required by subject:
- Ctrl+\ should do nothing visible in interactive mode.
*/
static void	sigquit_handler(int signo)
{
	(void)signo;
}

/*
Call this in the parent shell loop before calling readline().
*/
void	setup_signals_interactive(void)
{
	struct sigaction	act_int;
	struct sigaction	act_quit;

	sigemptyset(&act_int.sa_mask);
	act_int.sa_flags = 0;
	act_int.sa_handler = sigint_handler;
	sigaction(SIGINT, &act_int, NULL);

	sigemptyset(&act_quit.sa_mask);
	act_quit.sa_flags = 0;
	act_quit.sa_handler = sigquit_handler;
	sigaction(SIGQUIT, &act_quit, NULL);
}

/*
Child process should have default handlers.
Otherwise Ctrl+C won't terminate cat/sleep etc.
Call this in the child right before execve().
*/
void	setup_signals_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
