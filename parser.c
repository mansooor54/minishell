#include "minishell.h"

/*
read_line:
- Read from STDIN into buf
- Stop on '\n' or EOF
- Return number of bytes stored (0 means EOF)
*/

/*
read_line:
Reads one line from stdin into buf.
Stops at '\n' or buffer full.

Return values:
-1 : read() error
 0 : EOF and no data read (user pressed Ctrl+D on empty line)
>0 : length of line stored in buf (may be 0 chars if user just pressed Enter)
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
				return (0);      /* EOF with no input -> exit shell */
			break ;
		}
		got_any = 1;
		if (c == '\n')
			break ;
		buf[i] = c;
		i++;
	}
	buf[i] = '\0';
	/* normal line read, even if it's empty string */
	return (i + 1);
}


/*
split_args:
- Convert "ls -l /tmp" into:
  argv[0] = "ls"
  argv[1] = "-l"
  argv[2] = "/tmp"
  argv[3] = NULL
- It edits the same line in-place
- It returns argc
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
