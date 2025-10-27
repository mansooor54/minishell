#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>     /* read, write, fork, execve, chdir, getcwd, access */
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
	char	**envp;
}	t_shell;

/* utils.c */
size_t	ft_strlen(char *s);
void	ft_putstr(char *s);
void	ft_putstr_fd(char *s, int fd);
void	*ft_calloc(size_t count, size_t size);
char	*get_last_dir_name(char *path);
char	*ft_strdup(char *s);
char	*ft_strjoin_path(char *dir, char *cmd);
int		ft_strcmp(char *a, char *b);

/* parser.c */
int		read_line(char *buf, int buf_size);
int		split_args(char *line, char **argv, int max_args);
char	*get_env_value(char *name, char **envp);
char	**split_path_dirs(char *path_value);

/* executor.c */
int		run_builtin(char **argv);
void	run_command(char **argv, t_shell *sh);

/* main.c helper */
void	print_prompt(t_shell *sh);

#endif
// - if PATH not found or command not found in any dir -> print error and continue
// 		if (ft_strcmp(argv[0], "exit") == 0)
// 			break ;
// 		run_command(argv, &sh);
// 	}
// 	return (0);
