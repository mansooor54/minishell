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

void	free_args(char **args, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		args[i] = NULL;
		i++;
	}
	(void)args;
}

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
		if (path[i] == '/')
		{
			if (path[i + 1] != '\0')
				last = i + 1;
		}
		i++;
	}
	return (&path[last]);
}
