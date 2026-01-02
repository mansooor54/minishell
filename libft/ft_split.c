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

#include "libft.h"

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
