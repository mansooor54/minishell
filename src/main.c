/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:23:45 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 15:23:45 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	shell_loop(t_shell *sh)
{
	char	*line;
	t_cmd	*cmd;

	while (1)
	{
		if (sh->prompt_cache)
		{
			free(sh->prompt_cache);
			sh->prompt_cache = NULL;
		}
		sh->prompt_cache = build_prompt(sh);
		if (!sh->prompt_cache)
			sh->prompt_cache = ft_strdup(PROMPT_DEFAULT);

		setup_signals_interactive();
		line = readline(sh->prompt_cache);
		if (!line)
		{
			/* Ctrl-D */
			printf("exit\n");
			return (0);
		}
		if (line[0] != '\0')
			add_history(line);

		cmd = parse_line_basic(line);
		if (cmd)
		{
			exec_command(sh, cmd);
			free_cmd(cmd);
		}
		free(line);
	}
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	t_shell	sh;

	(void)ac;
	(void)av;
	sh.env = env_build(envp);
	sh.last_status = 0;
	sh.prompt_cache = NULL;
	shell_loop(&sh);
	return (0);
}
