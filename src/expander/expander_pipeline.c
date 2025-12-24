/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/04 15:10:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	expand_redirections(t_redir *redir, t_env *env, int exit_status)
{
	while (redir)
	{
		expand_arg(&redir->file, env, exit_status);
		redir = redir->next;
	}
}

static void	expand_single_cmd(t_cmd *cmd, t_env *env, int exit_status)
{
	if (!cmd || cmd->expanded)
		return ;
	expand_cmd_args(cmd, env, exit_status);
	expand_redirections(cmd->redirs, env, exit_status);
	cmd->expanded = 1;
}

void	expand_pipeline_cmds(t_cmd *cmds, t_env *env, int exit_status)
{
	while (cmds)
	{
		expand_single_cmd(cmds, env, exit_status);
		cmds = cmds->next;
	}
}

void	expander(t_pipeline *pipeline, t_env *env, int exit_status)
{
	while (pipeline)
	{
		expand_pipeline_cmds(pipeline->cmds, env, exit_status);
		pipeline = pipeline->next;
	}
}
