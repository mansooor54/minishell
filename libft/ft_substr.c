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

#include "libft.h"

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
