#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>     /* read, write, fork, execve, chdir, getcwd */
# include <sys/types.h>  /* pid_t */
# include <sys/wait.h>   /* waitpid */
# include <stdlib.h>     /* malloc, free, exit */
# include <stddef.h>     /* size_t */
# include <errno.h>      /* errno */
# include <limits.h>     /* PATH_MAX */

# define MAX_ARGS 100
# define BUFFER_SIZE 1024

typedef struct s_shell
{
	int		show_dir;           /* 0 = minishell$, 1 = minishell (dir)$ */
	char	cwd_name[PATH_MAX]; /* cached last dir name */
}	t_shell;

/* utils.c */
size_t	ft_strlen(char *s);
void	ft_putstr(char *s);
void	ft_putstr_fd(char *s, int fd);
void	*ft_calloc(size_t count, size_t size);
void	free_args(char **args, int count);
char	*get_last_dir_name(char *path);

/* parser.c */
int		read_line(char *buf, int buf_size);
int		split_args(char *line, char **argv, int max_args);

/* executor.c */
int		run_builtin(char **argv, t_shell *sh);
void	run_command(char **argv, t_shell *sh);

#endif
