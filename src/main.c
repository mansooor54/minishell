/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:00:00 by student           #+#    #+#             */
/*   Updated: 2025/11/02 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Global shell instance for signal handling */
t_shell	g_shell;

/*
** Initialize the shell structure with environment variables
** Sets exit status to 0 and should_exit flag to 0
*/
static void	init_shell(t_shell *shell, char **envp)
{
	shell->env = init_env(envp);
	shell->exit_status = 0;
	shell->should_exit = 0;
}

/*
** Process a single line of input through the shell pipeline:
** 1. Lexer: tokenize the input
** 2. Parser: build command structure
** 3. Expander: expand variables
** 4. Executor: execute commands
*/
static void	process_line(char *line, t_shell *shell)
{
	t_token		*tokens;
	t_pipeline	*pipeline;

	if (!line || !*line)
		return ;
	tokens = lexer(line);
	if (!tokens)
		return ;
	pipeline = parser(tokens);
	free_tokens(tokens);
	if (!pipeline)
		return ;
	expander(pipeline, shell->env);
	executor(pipeline, shell);
	free_pipeline(pipeline);
}

/*
** Main shell loop:
** Display prompt, read input, add to history, and process
** Continue until user exits with Ctrl-D or 'exit' command
*/
static void	shell_loop(t_shell *shell)
{
	char	*line;

	while (!shell->should_exit)
	{
		line = readline("minishell> ");
		if (!line)
		{
			ft_putendl_fd("exit", 1);
			break ;
		}
		if (*line)
		{
			add_history(line);
			process_line(line, shell);
		}
		free(line);
	}
}

/*
** Main entry point
** Initialize shell, setup signals, run main loop, cleanup
*/
int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	init_shell(&g_shell, envp);
	setup_signals();
	shell_loop(&g_shell);
	free_env(g_shell.env);
	return (g_shell.exit_status);
}
