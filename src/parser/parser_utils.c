/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 12:58:19 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

size_t	calculate_expansion_size(char *str, t_env *env, int exit_status)
{
	(void)env;
	(void)exit_status;
	return (ft_strlen(str));
}

/*
** append_redir - Append a redirection to the end of the list
**
** Adds a new redirection node to the end of an existing list.
**
** @param head: Pointer to the head of the redirection list
** @param new_redir: New redirection node to append
**
** Return: void
*/
void	append_redir(t_redir **head, t_redir *new_redir)
{
	t_redir	*current;

	if (!new_redir)
		return ;
	if (!*head)
	{
		*head = new_redir;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}