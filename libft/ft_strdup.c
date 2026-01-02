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

#include "libft.h"

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
