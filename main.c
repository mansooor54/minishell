/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 14:57:20 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_shell	g_shell;

/*
** process_line - Process a single line of user input
**
** This function implements the main shell pipeline:
** 1. Lexer: Tokenizes the input string into a list of tokens
** 2. Parser: Builds a command structure from the tokens
** 3. Expander: Expands environment variables and removes quotes
** 4. Executor: Executes the commands
**
** Each stage is responsible for freeing its own allocated memory.
** If any stage fails, the function returns early.
**
** @param line: The input string to process
** @param shell: Pointer to the shell structure containing state
**
** Return: void
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
** shell_loop - Main interactive loop of the shell
**
** This function implements the REPL (Read-Eval-Print-Loop):
** 1. Display prompt "minishell> " using readline
** 2. Read user input
** 3. Add non-empty input to history
** 4. Process the input line
** 5. Free the input line
** 6. Repeat until user exits
**
** The loop terminates when:
** - User presses Ctrl-D (readline returns NULL)
** - User executes the exit builtin (should_exit flag set)
**
** @param shell: Pointer to the shell structure
**
** Return: void
*/
void	shell_loop(t_shell *shell)
{
	char	*line;

	while (!shell->should_exit)
	{
		line = readline("\033[1;33mminishell> \033[0m");
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

/* single global allowed by subject 
** Global shell instance for signal handling
** This is the only global variable used, as required by the subject
** It allows signal handlers to access and modify shell state
*/

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	write(1, "\033[2J\033[H", 7);
	print_logo();
	init_shell(&g_shell, envp);
	setup_signals();
	shell_loop(&g_shell);
	free_env(g_shell.env);
	return (g_shell.exit_status);
}
