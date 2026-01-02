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

#include "libft.h"

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
