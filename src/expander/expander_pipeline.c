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

/*
** expand_redirections - Expand file names in redirections
**
** Iterates through all redirections and expands environment variables
** and removes quotes from file names.
**
** @param redir: Redirection linked list
** @param env: Environment linked list
** @param exit_status: Current exit status
**
** Return: void
*/
void	expand_redirections(t_redir *redir, t_env *env, int exit_status)
{
	while (redir)
	{
		expand_arg(&redir->file, env, exit_status);
		redir = redir->next;
	}
}

/*
** expand_single_cmd - Expand a single command
**
** Expands arguments and redirections for one command.
**
** @param cmd: Command structure
** @param env: Environment linked list
** @param exit_status: Current exit status
**
** Return: void
*/
static void	expand_single_cmd(t_cmd *cmd, t_env *env, int exit_status)
{
	expand_cmd_args(cmd, env, exit_status);
	expand_redirections(cmd->redirs, env, exit_status);
}

/*
** expand_pipeline_cmds - Expand all commands in a pipeline
**
** Iterates through all commands in a pipeline and expands their
** arguments and redirections.
**
** @param cmds: Command linked list
** @param env: Environment linked list
** @param exit_status: Current exit status
**
** Return: void
*/
void	expand_pipeline_cmds(t_cmd *cmds, t_env *env, int exit_status)
{
	while (cmds)
	{
		expand_single_cmd(cmds, env, exit_status);
		cmds = cmds->next;
	}
}

/*
** expander - Expand variables in all commands of the pipeline
**
** Main entry point for the expander. Processes all pipelines, commands,
** arguments, and redirection files to expand environment variables and
** remove quotes.
**
** @param pipeline: Pipeline linked list
** @param env: Environment linked list
**
** Return: void
*/
void	expander(t_pipeline *pipeline, t_env *env)
{
	while (pipeline)
	{
		expand_pipeline_cmds(pipeline->cmds, env, g_shell.exit_status);
		pipeline = pipeline->next;
	}
}
