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

#include "minishell.h"

t_shell	g_shell;

static int	is_all_space(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		if (!ft_isspace((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	process_line(char *line, t_shell *shell)
{
	t_token		*tokens;
	t_pipeline	*pipeline;

	if (!line || !*line)
		return ;
	if (has_unclosed_quotes(line))
	{
		ft_putendl_fd("minishell: syntax error: unclosed quotes", 2);
		shell->exit_status = 2;
		return ;
	}
	tokens = lexer(line);
	if (!tokens)
		return ;
	if (!validate_syntax(tokens, &g_shell))
	{
		free_tokens(tokens);
		return ;
	}
	pipeline = parser(tokens);
	free_tokens(tokens);
	if (!pipeline)
		return ;
	expander(pipeline, shell->env);
	executor(pipeline, shell);
	free_pipeline(pipeline);
}

void	shell_loop(t_shell *shell)
{
	char	*line;

	while (!shell->should_exit)
	{
		line = read_logical_line();
		if (!line)
		{
			/* Ctrl-C: exit_status set to 130 → just show new prompt */
			if (shell->exit_status == 130)
				continue;
			/* Real EOF (Ctrl-D): exit minishell like bash */
			ft_putendl_fd("exit", 1);
			break ;
		}
		if (*line && !is_all_space(line))
		{
			history_add_line(line);
			process_line(line, shell);
		}
		free(line);
	}
	history_save();
}

/* main.c */
int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	write(1, "\033[2J\033[H", 7);
	print_logo();
	init_shell(&g_shell, envp);
	setup_signals();
	init_terminal();
	shell_loop(&g_shell);
	rl_clear_history();
	free_env(g_shell.env);
	return (g_shell.exit_status);
}
