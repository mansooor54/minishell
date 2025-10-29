/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:24:01 by malmarzo          #+#    #+#             */
/*   Updated: 2025/10/29 15:24:01 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>              /* printf, perror */
# include <stdlib.h>             /* malloc, free, exit */
# include <unistd.h>     /* write, access, execve, fork, getcwd, chdir */
# include <sys/types.h>          /* pid_t */
# include <sys/wait.h>           /* waitpid */
# include <readline/readline.h>  /* readline, rl_* */
# include <readline/history.h>   /* add_history */
# include <string.h>             /* strcmp, strlen (placeholder until libft) */
# include <errno.h>              /* errno */
# include <limits.h>             /* PATH_MAX */
# include <signal.h>             /* sigaction, sig_atomic_t */

/*
Temporary forward declarations for readline types.
This avoids implicit declaration errors on systems without readline headers.
On school machine you will compile with real readline headers.
*/
char	*readline(const char *prompt);
void	add_history(const char *line);
void	rl_replace_line(const char *text, int clear_undo);
void	rl_on_new_line(void);
void	rl_redisplay(void);

# define PROMPT_DEFAULT "minishell$ "

extern volatile sig_atomic_t g_signal_status;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_cmd
{
	char	**argv;
}	t_cmd;

typedef struct s_shell
{
	t_env	*env;
	int		last_status;
	char	*prompt_cache;
}	t_shell;

/* utils.c */
size_t	ft_strlen(const char *s);
char	*ft_strdup(const char *s);
int		ft_strcmp(const char *a, const char *b);
char	*ft_strjoin_path(const char *dir, const char *cmd);

/* env.c */
t_env	*env_build(char **envp);
char	*env_get_value(t_env *env, const char *key);
char	**env_to_envarray(t_env *env);
void	env_free_array(char **arr);

/* prompt.c */
char	*build_prompt(t_shell *sh);

/* parser.c */
t_cmd	*parse_line_basic(char *line);
void	free_cmd(t_cmd *cmd);

/* builtins.c */
int		is_builtin(char *name);
int		run_builtin(t_shell *sh, t_cmd *cmd);

/* exec.c */
int		exec_command(t_shell *sh, t_cmd *cmd);

/* signals.c */
void	setup_signals_interactive(void);
void	setup_signals_child(void);

/* main.c */
int		shell_loop(t_shell *sh);

#endif
