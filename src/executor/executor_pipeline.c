/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 15:25:42 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** executor - Main executor function with logical operator support
**
** This is the top-level executor that handles pipelines connected by
** logical operators (&& and ||). It implements short-circuit evaluation:
**
** Logical AND (&&):
** - Execute first pipeline
** - If exit status is non-zero (failure), stop execution
** - If exit status is zero (success), continue to next pipeline
** - Example: "make && ./program" only runs program if make succeeds
**
** Logical OR (||):
** - Execute first pipeline
** - If exit status is zero (success), stop execution
** - If exit status is non-zero (failure), continue to next pipeline
** - Example: "test -f file || touch file" creates file only if it doesn't exist
**
** This function iterates through the pipeline list, executing each one
** and checking the logical operator to determine whether to continue.
** The exit status is preserved across the entire chain.
**
** Note: This is a BONUS feature. The mandatory part only requires single
** pipelines without logical operators.
**
** @param pipeline: Linked list of pipelines connected by logical operators
** 			Each pipeline has a logic_op
			field (TOKEN_AND, TOKEN_OR, or TOKEN_EOF)
** @param shell: Shell state structure
**
** Return: void (updates shell->exit_status)
*/
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
