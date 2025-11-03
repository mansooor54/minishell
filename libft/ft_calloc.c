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

#include "libft.h"

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
