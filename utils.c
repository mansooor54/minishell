#include "minishell.h"

size_t	ft_strlen(char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}

void	ft_putstr(char *s)
{
	if (!s)
		return ;
	write(1, s, ft_strlen(s));
}

void	ft_putstr_fd(char *s, int fd)
{
	if (!s)
		return ;
	write(fd, s, ft_strlen(s));
}

void	*ft_calloc(size_t count, size_t size)
{
	size_t	i;
	size_t	total;
	char	*mem;

	total = count * size;
	mem = (char *)malloc(total);
	if (!mem)
		return (NULL);
	i = 0;
	while (i < total)
	{
		mem[i] = 0;
		i++;
	}
	return ((void *)mem);
}

/* return pointer to last component after '/' */
char	*get_last_dir_name(char *path)
{
	int	i;
	int	last;

	if (!path || path[0] == '\0')
		return ("/");
	i = 0;
	last = 0;
	while (path[i] != '\0')
	{
		if (path[i] == '/' && path[i + 1] != '\0')
			last = i + 1;
		i++;
	}
	return (&path[last]);
}

/* basic strcmp */
int	ft_strcmp(char *a, char *b)
{
	int	i;

	i = 0;
	while (a[i] && b[i] && a[i] == b[i])
		i++;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

/* duplicate string with malloc */
char	*ft_strdup(char *s)
{
	size_t	len;
	size_t	i;
	char	*copy;

	len = ft_strlen(s);
	copy = (char *)malloc(len + 1);
	if (!copy)
		return (NULL);
	i = 0;
	while (i < len)
	{
		copy[i] = s[i];
		i++;
	}
	copy[i] = '\0';
	return (copy);
}

/*
ft_strjoin_path:
dir="/bin"
cmd="ls"
-> "/bin/ls"
Returns malloc'd string or NULL.
*/
char	*ft_strjoin_path(char *dir, char *cmd)
{
	size_t	len_dir;
	size_t	len_cmd;
	size_t	i;
	size_t	j;
	char	*out;

	len_dir = ft_strlen(dir);
	len_cmd = ft_strlen(cmd);
	out = (char *)malloc(len_dir + 1 + len_cmd + 1);
	if (!out)
		return (NULL);
	i = 0;
	while (i < len_dir)
	{
		out[i] = dir[i];
		i++;
	}
	out[i] = '/';
	i++;
	j = 0;
	while (j < len_cmd)
	{
		out[i + j] = cmd[j];
		j++;
	}
	out[i + j] = '\0';
	return (out);
}
