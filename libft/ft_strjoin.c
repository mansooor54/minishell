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

#include "libft.h"

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
