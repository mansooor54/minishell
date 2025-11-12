#include "../../minishell.h"
#include "minishell.h"
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:15:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_atoi(const char *str)
{
	int					i;
	int					sign;
	unsigned long long	result;
	int					digit;

	i = 0;
	sign = 1;
	result = 0;
	digit = 0;
	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
		i++;
	if ((str[i] == '-') || (str[i] == '+'))
		if (str[i++] == '-')
			sign = sign * -1;
	while (str[i] == '0')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i++] - '0');
		digit++;
	}
	if (result >= __LONG_LONG_MAX__ || digit > 19)
		return (-(sign == 1));
	return (result * sign);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:19:21 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:19:21 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	ft_bzero(void *s, size_t n)
{
	size_t	i;
	char	*str;

	str = s;
	i = 0;
	while (i < n)
	{
		str[i] = 0;
		i++;
	}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:20:11 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:20:11 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	*ft_calloc(size_t count, size_t size)
{
	size_t	total;
	void	*mem;

	if (size != 0 && count > SIZE_MAX / size)
		return (NULL);
	total = count * size;
	mem = malloc(total);
	if (mem == NULL)
		return (NULL);
	ft_memset(mem, 0, total);
	return (mem);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:20:29 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:20:29 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_isalnum(int c)
{
	if (ft_isdigit(c) || ft_isalpha(c))
		return (1);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:21:03 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:21:03 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_isalpha(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:21:22 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:21:22 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:21:46 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:21:46 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:22:13 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:22:13 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_isprint(int c)
{
	if (c >= 32 && c <= 126)
		return (1);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isspace.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:22:13 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:22:13 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_isspace(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:22:32 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:22:32 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


static int	ft_numlen(int n)
{
	int		len;
	long	nb;

	len = 1;
	nb = n;
	if (nb < 0)
	{
		nb = -nb;
		len++;
	}
	while (nb >= 10)
	{
		nb /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	long	num;
	int		len;
	char	*str;

	num = n;
	len = ft_numlen(n);
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	str[len] = '\0';
	if (num < 0)
	{
		str[0] = '-';
		num = -num;
	}
	while (num >= 10)
	{
		str[--len] = (num % 10) + '0';
		num /= 10;
	}
	str[--len] = num + '0';
	return (str);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:23:03 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:23:03 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*ptr;

	ptr = ((unsigned char *)s);
	i = 0;
	while (i < n)
	{
		if (ptr[i] == (unsigned char)c)
			return (ptr + i);
		i++;
	}
	return (NULL);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:23:18 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:23:18 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*ptr;
	const unsigned char	*ptr1;

	ptr = (const unsigned char *)s1;
	ptr1 = (const unsigned char *)s2;
	if (n == 0)
		return (0);
	while (n > 0)
	{
		if (*ptr != *ptr1)
			return ((int)(*ptr - *ptr1));
		ptr++;
		ptr1++;
		n--;
	}
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:23:27 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:23:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t				i;
	unsigned char		*d;
	const unsigned char	*s;

	i = 0;
	if (!dest && !src)
		return (NULL);
	if (dest == src)
		return (dest);
	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dest);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:24:23 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:24:23 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;
	size_t				i;

	if (!dest && !src)
		return (NULL);
	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	if (d > s && d < s + n)
		while (n--)
			d[n] = s[n];
	else
	{
		i = 0;
		while (i < n)
		{
			d[i] = s[i];
			i++;
		}
	}
	return (dest);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:25:44 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:25:44 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*ptr;

	ptr = (unsigned char *)s;
	while (n--)
		*(ptr++) = (unsigned char)c;
	return (s);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:26:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:26:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:26:11 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:26:11 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	ft_putendl_fd(char *s, int fd)
{
	if (!s)
		return ;
	ft_putstr_fd(s, fd);
	ft_putchar_fd('\n', fd);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:26:21 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:26:21 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	ft_putnbr_fd(int n, int fd)
{
	long	nb;

	nb = n;
	if (nb < 0)
	{
		write(fd, "-", 1);
		nb *= -1;
	}
	if (nb > 9)
		ft_putnbr_fd(nb / 10, fd);
	ft_putchar_fd((nb % 10) + '0', fd);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:26:32 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:26:32 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	ft_putstr_fd(char *s, int fd)
{
	if (!s)
		return ;
	while (*s)
	{
		ft_putchar_fd(*s, fd);
		s++;
	}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:26:43 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:26:43 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_safe_malloc(char **word_vector, int position, size_t buffer)
{
	int	i;

	i = 0;
	word_vector[position] = malloc(buffer);
	if (NULL == word_vector[position])
	{
		while (i < position)
			free(word_vector[i++]);
		free(word_vector);
		return (1);
	}
	return (0);
}

void	my_strlcpy(char *dst, const char *src, size_t dstsize)
{
	while (*src && --dstsize)
		*dst++ = *src++;
	*dst = '\0';
}

int	insert_words(char **word_vector, char const *s, char delimeter)
{
	size_t	len;
	int		i;

	i = 0;
	while (*s)
	{
		len = 0;
		while (*s == delimeter && *s)
			++s;
		while (*s != delimeter && *s)
		{
			++len;
			++s;
		}
		if (len)
		{
			if (ft_safe_malloc(word_vector, i, len + 1))
				return (1);
			my_strlcpy(word_vector[i], s - len, len + 1);
		}
		++i;
	}
	return (0);
}

size_t	count_words(char const *s, char delimeter)
{
	size_t	words_cont;
	int		inside_word;

	words_cont = 0;
	while (*s)
	{
		inside_word = 0;
		while (*s == delimeter && *s)
			++s;
		while (*s != delimeter && *s)
		{
			if (!inside_word)
			{
				++words_cont;
				inside_word = 1;
			}
			++s;
		}
	}
	return (words_cont);
}

char	**ft_split(const char *s, char c)
{
	size_t	words_cont;
	char	**word_vector;

	if (NULL == s)
		return (NULL);
	words_cont = 0;
	words_cont = count_words(s, c);
	word_vector = malloc((words_cont + 1) * sizeof(char *));
	if (NULL == word_vector)
		return (NULL);
	word_vector[words_cont] = NULL;
	if (insert_words(word_vector, s, c))
		return (NULL);
	return (word_vector);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:39:14 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:39:14 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_strchr(const char *s, int c)
{
	int		i;

	i = 0;
	while (s[i] != (unsigned char)c)
	{
		if (s[i] == '\0')
			return (NULL);
		i++;
	}
	return ((char *)s + i);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:39:14 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:39:14 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:26:43 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:24:40 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_strcpy(char *dst, const char *src)
{
	int	i;

	if (!dst || !src)
		return (NULL);
	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (dst);
}

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:40:46 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:40:46 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_strdup(const char *src)
{
	size_t	size;
	char	*dest;

	size = ft_strlen(src) + 1;
	dest = (char *)malloc(size * sizeof(char));
	if (!dest)
		return (NULL);
	ft_strlcpy(dest, src, size);
	return (dest);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:41:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:41:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	unsigned int	i;

	if (!s || !f)
		return ;
	i = -1;
	while (s[++i])
		f(i, &s[i]);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:41:27 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:41:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*dst;
	size_t	size;
	size_t	len_s1;
	size_t	len_s2;

	if (s1 == NULL || s2 == NULL)
		return (NULL);
	size = ft_strlen(s1) + ft_strlen(s2) + 1;
	dst = ft_calloc(sizeof(char), size);
	if (dst == NULL)
		return (NULL);
	len_s1 = 0;
	while (*(s1 + len_s1))
	{
		*(dst + len_s1) = *(s1 + len_s1);
		len_s1++;
	}
	len_s2 = 0;
	while (*(s2 + len_s2))
	{
		*(dst + len_s1 + len_s2) = *(s2 + len_s2);
		len_s2++;
	}
	*(dst + len_s1 + len_s2) = '\0';
	return (dst);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:44:18 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:44:18 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	dst_len;
	size_t	src_len;

	src_len = ft_strlen(src);
	if (!dest && size == 0)
		return (src_len);
	dst_len = ft_strlen(dest);
	if (size <= dst_len)
		return (size + src_len);
	i = 0;
	while (src[i] && (dst_len + i) < (size - 1))
	{
		dest[dst_len + i] = src[i];
		i++;
	}
	dest[dst_len + i] = '\0';
	return (dst_len + src_len);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:45:18 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:45:18 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	src_len;

	src_len = ft_strlen(src);
	if (size == 0)
		return (src_len);
	i = 0;
	while (src [i] != '\0' && i < (size - 1))
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (src_len);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:46:41 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:46:41 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:48:29 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:48:29 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_strmapi(const char *s, char (*f)(unsigned int, char))
{
	size_t	i;
	size_t	len;
	char	*result;

	if (!s || !f)
		return (NULL);
	len = ft_strlen(s);
	result = (char *)ft_calloc((len + 1), sizeof(char));
	if (!result)
		return (NULL);
	i = 0;
	while (i < len)
	{
		result[i] = f(i, s[i]);
		i++;
	}
	result[i] = '\0';
	return (result);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:48:41 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:48:41 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*str1;
	unsigned char	*str2;

	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	i = 0;
	if (n == 0)
		return (0);
	while (i < n - 1 && str1[i] && str1[i] == str2[i])
		i++;
	return (str1[i] - str2[i]);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:49:01 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:49:01 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;

	if (*needle == '\0')
		return ((char *)haystack);
	if (len == 0)
		return (NULL);
	i = 0;
	while (haystack[i] != '\0' && i < len)
	{
		j = 0;
		while (needle[j] != '\0' && haystack[i + j] == needle[j]
			&& (i + j) < len)
			j++;
		if (needle[j] == '\0')
			return ((char *)(haystack + i));
		i++;
	}
	return (NULL);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:49:12 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:49:12 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_strrchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	while (i >= 0)
	{
		if (s[i] == (char)c)
			return ((char *)(s + i));
		i--;
	}
	return (NULL);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:49:20 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:49:20 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_strtrim(const char *s1, const char *set)
{
	size_t	start;
	size_t	end;
	char	*trimmed_str;

	if (!s1 || !set)
		return (NULL);
	start = 0;
	end = ft_strlen(s1);
	while (s1[start] && ft_strchr(set, s1[start]))
		start++;
	if (start == end)
		return (ft_strdup(""));
	while (end > start && ft_strchr(set, s1[end - 1]))
		end--;
	trimmed_str = malloc(end - start + 1);
	if (!trimmed_str)
		return (NULL);
	ft_strlcpy(trimmed_str, &s1[start], end - start + 1);
	return (trimmed_str);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 14:22:02 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 14:22:02 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char	*new;
	size_t	slen;
	size_t	actual_len;

	if (!s)
		return (NULL);
	slen = ft_strlen(s);
	if (slen == 0 || start >= slen)
		return (ft_strdup(""));
	actual_len = slen - start;
	if (actual_len > len)
		actual_len = len;
	new = (char *)malloc(sizeof(char) * (actual_len + 1));
	if (!new)
		return (NULL);
	ft_strlcpy(new, s + start, actual_len + 1);
	return (new);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:51:17 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:51:17 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 32);
	return (c);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:51:50 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:51:50 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	ft_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}
