#include "minishell.h"

/* print_prompt:
   - if sh->show_dir == 0:
        minishell$
   - else:
        minishell (<dir>)$
*/
void	print_prompt(t_shell *sh)
{
	char	cwd[PATH_MAX];
	char	*name;

	(void)sh;
	if (getcwd(cwd, PATH_MAX) == NULL)
	{
		ft_putstr("minishell$ ");
		return ;
	}
	name = get_last_dir_name(cwd);

	/* if we are in the main project dir "minishell" -> plain minishell$ */
	if (ft_strcmp(name, "minishell") == 0)
	{
		ft_putstr("minishell$ ");
		return ;
	}

	/* else show minishell(<dirname>)$ */
	ft_putstr("minishell(");
	ft_putstr(name);
	ft_putstr(")$ ");
}

/*
read loop:
- show prompt
- read line
- split args
- handle exit
- run command (which may update sh for cd)
*/
int	main(int ac, char **av, char **envp)
{
	char		buffer[BUFFER_SIZE];
	char		*argv[MAX_ARGS];
	int			res;
	int			argc;
	t_shell		sh;

	(void)ac;
	(void)av;
	sh.envp = envp;
	while (1)
	{
		print_prompt(&sh);
		res = read_line(buffer, BUFFER_SIZE);
		if (res < 0)
		{
			ft_putstr_fd("read error\n", 2);
			break ;
		}
		if (res == 0)
		{
			ft_putstr("\n");
			break ;
		}
		argc = split_args(buffer, argv, MAX_ARGS);
		if (argc == 0)
			continue ;
		if (argc == 1
			&& argv[0][0] == 'e'
			&& argv[0][1] == 'x'
			&& argv[0][2] == 'i'
			&& argv[0][3] == 't'
			&& argv[0][4] == '\0')
			break ;
		run_command(argv, &sh);
	}
	return (0);
}
