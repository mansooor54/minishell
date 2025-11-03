/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Global shell instance for signal handling
** This is the only global variable used, as required by the subject
** It allows signal handlers to access and modify shell state
*/
t_shell	g_shell;

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
static void	init_shell(t_shell *shell, char **envp)
{
	shell->env = init_env(envp);
	shell->exit_status = 0;
	shell->should_exit = 0;
}

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
** main - Entry point of the minishell program
**
** This function:
** 1. Initializes the global shell structure with environment variables
** 2. Sets up signal handlers for Ctrl-C, Ctrl-D, and Ctrl-\
** 3. Runs the main shell loop
** 4. Cleans up allocated memory before exiting
** 5. Returns the last exit status
**
** The argc and argv parameters are unused but required by the standard
** main function signature.
**
** @param argc: Argument count (unused)
** @param argv: Argument vector (unused)
** @param envp: Environment variables array
**
** Return: Exit status of the last executed command
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
