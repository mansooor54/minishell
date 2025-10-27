#include "minishell.h"

/*
read_line:
Return codes:
-1 error
 0 EOF before any char (Ctrl+D at empty prompt)
>0 success (line stored in buf, no '\n')
We return (i + 1) so even empty line gives >=1
*/
int	read_line(char *buf, int buf_size)
{
	int		i;
	int		ret;
	char	c;
	int		got_any;

	i = 0;
	got_any = 0;
	while (i < buf_size - 1)
	{
		ret = read(0, &c, 1);
		if (ret < 0)
			return (-1);
		if (ret == 0)
		{
			if (got_any == 0)
				return (0);
			break ;
		}
		got_any = 1;
		if (c == '\n')
			break ;
		buf[i] = c;
		i++;
	}
	buf[i] = '\0';
	return (i + 1);
}

/*
split_args:
turn "ls -l /tmp" into argv[0]="ls", argv[1]="-l", ...
*/
int	split_args(char *line, char **argv, int max_args)
{
	int	i;
	int	arg_i;
	int	in_word;

	i = 0;
	arg_i = 0;
	in_word = 0;
	while (line[i] != '\0' && arg_i < max_args - 1)
	{
		if (line[i] == ' ' || line[i] == '\t')
		{
			line[i] = '\0';
			in_word = 0;
		}
		else if (in_word == 0)
		{
			argv[arg_i] = &line[i];
			arg_i++;
			in_word = 1;
		}
		i++;
	}
	argv[arg_i] = NULL;
	return (arg_i);
}

/* check if env_entry starts with "NAME=" */
static int	starts_with_name(char *env_entry, char *name)
{
	size_t	i;

	i = 0;
	while (name[i] != '\0')
	{
		if (env_entry[i] != name[i])
			return (0);
		i++;
	}
	if (env_entry[i] == '=')
		return (1);
	return (0);
}

/*
get_env_value("PATH", envp)
-> returns pointer to the chars after "PATH="
*/
char	*get_env_value(char *name, char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
	{
		if (starts_with_name(envp[i], name))
			return (envp[i] + ft_strlen(name) + 1);
		i++;
	}
	return (NULL);
}

/*
split_path_dirs:
PATH like "/usr/local/bin:/usr/bin:/bin"
We return malloc'ed array of char* ending with NULL.
Each entry is its own malloc'd copy of one dir.
*/
char	**split_path_dirs(char *path_value)
{
	int		i;
	int		count;
	int		start;
	char	**dirs;
	int		d;

	if (!path_value)
		return (NULL);
	/* first pass count segments */
	count = 1;
	i = 0;
	while (path_value[i])
	{
		if (path_value[i] == ':')
			count++;
		i++;
	}
	dirs = (char **)ft_calloc(count + 1, sizeof(char *));
	if (!dirs)
		return (NULL);
	i = 0;
	d = 0;
	start = 0;
	while (1)
	{
		if (path_value[i] == ':' || path_value[i] == '\0')
		{
			int len = i - start;
			char *tmp = (char *)malloc(len + 1);
			int k = 0;
			if (!tmp)
				return (dirs); /* leak on purpose for now. 42 later we free */
			while (k < len)
			{
				tmp[k] = path_value[start + k];
				k++;
			}
			tmp[k] = '\0';
			dirs[d] = tmp;
			d++;
			if (path_value[i] == '\0')
				break ;
			start = i + 1;
		}
		i++;
	}
	dirs[d] = NULL;
	return (dirs);
}
