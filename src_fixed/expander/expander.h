/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:30:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/04 15:45:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H

# include "../../minishell.h"

/*
** Structure to hold expansion context
** Used to avoid passing too many arguments (>4) to functions
*/
typedef struct s_exp_ctx
{
	char	*str;
	char	*result;
	int		*i;
	int		*j;
	t_env	*env;
	int		exit_status;
}	t_exp_ctx;

/* Main expander functions */
char	*get_env_value(t_env *env, char *key);
char	*expand_variables(char *str, t_env *env, int exit_status);
void	expander(t_pipeline *pipeline, t_env *env);

/* Helper functions for variable expansion */
void	expand_exit_status(char *result, int *j, int exit_status);
void	expand_var_name(t_exp_ctx *ctx);
void	process_dollar(t_exp_ctx *ctx);

/* Helper functions for argument and command processing */
void	expand_arg(char **arg, t_env *env, int exit_status);
void	expand_cmd_args(t_cmd *cmd, t_env *env, int exit_status);
void	expand_redirections(t_redir *redir, t_env *env, int exit_status);
void	expand_pipeline_cmds(t_cmd *cmds, t_env *env, int exit_status);

/* Quote removal */
char	*remove_quotes(char *str);

#endif
