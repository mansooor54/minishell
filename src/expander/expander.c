/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:00:00 by student           #+#    #+#             */
/*   Updated: 2025/11/02 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
** Get environment variable value by key
** Returns the value string or NULL if not found
*/
char	*get_env_value(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
** Remove quotes from a string
** Handles both single and double quotes
*/
static char	*remove_quotes(char *str)
{
	char	*result;
	int		i;
	int		j;
	char	quote;

	result = malloc(ft_strlen(str) + 1);
	i = 0;
	j = 0;
	quote = 0;
	while (str[i])
	{
		if (!quote && (str[i] == '\'' || str[i] == '"'))
			quote = str[i];
		else if (quote && str[i] == quote)
			quote = 0;
		else
			result[j++] = str[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}

/*
** Expand environment variables in a string
** Handles $VAR and $? (exit status)
** Does not expand inside single quotes
*/
char	*expand_variables(char *str, t_env *env, int exit_status)
{
	char	*result;
	char	*tmp;
	int		i;
	int		j;
	char	in_quote;
	char	var_name[256];
	char	*var_value;
	int		k;

	result = malloc(4096);
	i = 0;
	j = 0;
	in_quote = 0;
	while (str[i])
	{
		if (!in_quote && (str[i] == '\'' || str[i] == '"'))
			in_quote = str[i];
		else if (in_quote && str[i] == in_quote)
			in_quote = 0;
		if (str[i] == '$' && in_quote != '\'')
		{
			i++;
			if (str[i] == '?')
			{
				tmp = ft_itoa(exit_status);
				ft_strcpy(&result[j], tmp);
				j += ft_strlen(tmp);
				free(tmp);
				i++;
			}
			else
			{
				k = 0;
				while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
					var_name[k++] = str[i++];
				var_name[k] = '\0';
				var_value = get_env_value(env, var_name);
				if (var_value)
				{
					ft_strcpy(&result[j], var_value);
					j += ft_strlen(var_value);
				}
			}
		}
		else
			result[j++] = str[i++];
	}
	result[j] = '\0';
	return (result);
}

/*
** Expand variables in all commands of the pipeline
** Processes arguments and redirection files
*/
void	expander(t_pipeline *pipeline, t_env *env)
{
	t_cmd	*cmd;
	t_redir	*redir;
	int		i;
	char	*expanded;
	char	*unquoted;

	while (pipeline)
	{
		cmd = pipeline->cmds;
		while (cmd)
		{
			i = 0;
			while (cmd->args[i])
			{
				expanded = expand_variables(cmd->args[i], env,
						g_shell.exit_status);
				unquoted = remove_quotes(expanded);
				free(cmd->args[i]);
				free(expanded);
				cmd->args[i] = unquoted;
				i++;
			}
			redir = cmd->redirs;
			while (redir)
			{
				expanded = expand_variables(redir->file, env,
						g_shell.exit_status);
				unquoted = remove_quotes(expanded);
				free(redir->file);
				free(expanded);
				redir->file = unquoted;
				redir = redir->next;
			}
			cmd = cmd->next;
		}
		pipeline = pipeline->next;
	}
}
