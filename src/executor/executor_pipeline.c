/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 15:38:52 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	executor(t_pipeline *pipeline, t_shell *shell)
{
	while (pipeline)
	{
		shell->exit_status = execute_pipeline(pipeline->cmds, shell);
		if (pipeline->logic_op == TOKEN_AND && shell->exit_status != 0)
			break ;
		if (pipeline->logic_op == TOKEN_OR && shell->exit_status == 0)
			break ;
		pipeline = pipeline->next;
	}
}
