/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
		token->value = ft_strdup(value);
	else
		token->value = NULL;
	token->next = NULL;
	return (token);
}

void	add_token(t_token **tokens, t_token *new_token)
{
	t_token	*current;

	if (!*tokens)
	{
		*tokens = new_token;
		return ;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens;
		tokens = tokens->next;
		free(tmp->value);
		free(tmp);
	}
}

static t_token	*next_token(char **input)
{
	t_token	*new_token;
	char	*word;
	int		len;

	while (is_whitespace(**input))
		(*input)++;
	if (!**input)
		return (NULL);
	if (is_operator(**input))
		new_token = get_operator_token(input);
	else
	{
		len = extract_word(*input, &word);
		new_token = create_token(TOKEN_WORD, word);
		free(word);
		*input += len;
	}
	return (new_token);
}

t_token	*lexer(char *input)
{
	t_token	*tokens;
	t_token	*new_token;

	tokens = NULL;
	while (*input)
	{
		new_token = next_token(&input);
		if (!new_token)
			break ;
		add_token(&tokens, new_token);
	}
	return (tokens);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:03:10 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Identify and create operator token
** Handles |, ||, &&, <, <<, >, >>
*/
t_token	*get_operator_token(char **input)
{
	t_token		*tok;

	tok = try_semicolon(input);
	if (tok)
		return (tok);
	tok = try_or_pipe(input);
	if (tok)
		return (tok);
	tok = try_and(input);
	if (tok)
		return (tok);
	tok = try_inredir(input);
	if (tok)
		return (tok);
	return (try_outredir(input));
}/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator_type.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:02:14 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_token	*try_semicolon(char **input)
{
	if (**input == ';')
	{
		(*input)++;
		return (create_token(TOKEN_SEMI, ";"));
	}
	return (NULL);
}

t_token	*try_or_pipe(char **input)
{
	if (**input != '|')
		return (NULL);
	if (*(*input + 1) == '|')
	{
		*input += 2;
		return (create_token(TOKEN_OR, "||"));
	}
	(*input)++;
	return (create_token(TOKEN_PIPE, "|"));
}

t_token	*try_and(char **input)
{
	if (**input == '&' && *(*input + 1) == '&')
	{
		*input += 2;
		return (create_token(TOKEN_AND, "&&"));
	}
	return (NULL);
}

t_token	*try_inredir(char **input)
{
	if (**input == '<' && *(*input + 1) == '<')
	{
		*input += 2;
		return (create_token(TOKEN_REDIR_HEREDOC, "<<"));
	}
	if (**input == '<')
	{
		(*input)++;
		return (create_token(TOKEN_REDIR_IN, "<"));
	}
	return (NULL);
}

t_token	*try_outredir(char **input)
{
	if (**input == '>' && *(*input + 1) == '>')
	{
		*input += 2;
		return (create_token(TOKEN_REDIR_APPEND, ">>"));
	}
	if (**input == '>')
	{
		(*input)++;
		return (create_token(TOKEN_REDIR_OUT, ">"));
	}
	return (NULL);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 16:02:34 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* ====== helpers (static, same variable names) ====== */

static int	enter_quote(char *s, size_t *i, char *in_quote, int *escaped)
{
	if (*in_quote)
		return (0);
	if (s[*i] != '\'' && s[*i] != '"')
		return (0);
	*in_quote = s[*i];
	(*i)++;
	*escaped = 0;
	return (1);
}

static int	handle_inside(char *s, size_t *i, char *in_quote, int *escaped)
{
	if (!*in_quote)
		return (0);
	if (s[*i] == *in_quote && !*escaped)
	{
		*in_quote = 0;
		(*i)++;
		return (1);
	}
	if (*in_quote == '"' && s[*i] == '\\')
	{
		*escaped = !*escaped;
		(*i)++;
		return (1);
	}
	*escaped = 0;
	(*i)++;
	return (1);
}

static int	handle_outside(char *s, size_t *i)
{
	if (s[*i] != '\\')
		return (0);
	(*i)++;
	if (s[*i] != '\0')
		(*i)++;
	return (1);
}

int	has_unclosed_quotes(char *s)
{
	size_t	i;
	char	in_quote;
	int		escaped;

	i = 0;
	in_quote = 0;
	escaped = 0;
	while (s[i])
	{
		if (enter_quote(s, &i, &in_quote, &escaped))
			continue ;
		if (handle_inside(s, &i, &in_quote, &escaped))
			continue ;
		if (handle_outside(s, &i))
			continue ;
		i++;
	}
	return (in_quote != 0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 12:21:28 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t');
}

int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&' || c == ';');
}

static int	is_word_cont(char *s, int i, int in_quote)
{
	if (!s[i])
		return (0);
	if (in_quote)
		return (1);
	if (is_whitespace(s[i]) || is_operator(s[i]))
		return (0);
	return (1);
}

/* scans only, finds length */
static int	measure_word(char *s)
{
	int	i;
	int	in_quote;

	i = 0;
	in_quote = 0;
	while (is_word_cont(s, i, in_quote))
	{
		if (!in_quote && (s[i] == '\'' || s[i] == '"'))
		{
			in_quote = s[i];
			i++;
			continue ;
		}
		if (in_quote && s[i] == in_quote)
		{
			if (!(in_quote == '"' && i > 0 && s[i - 1] == '\\'))
				in_quote = 0;
			i++;
			continue ;
		}
		i++;
	}
	return (i);
}

// /* Collapse escapes outside quotes:  \\x -> \x,  \\ -> \  */
// static char *unescape_outside_quotes(const char *s)
// {
// 	int   i = 0, j = 0;
// 	char  inq = 0;
// 	char *out = malloc(ft_strlen(s) + 1);

// 	if (!out) return NULL;
// 	while (s[i])
// 	{
// 		if (!inq && (s[i] == '\'' || s[i] == '"'))
// 		{
// 			inq = s[i++];
// 			continue;                 /* remove quote chars */
// 		}
// 		if (inq && s[i] == inq)      /* closing quote */
// 		{
// 			inq = 0;
// 			i++;
// 			continue;                 /* remove quote chars */
// 		}
// 		if (!inq && s[i] == '\\' && s[i + 1] != '\0')
// 		{
// 			out[j++] = s[i + 1];     /* eat the backslash, keep next char */
// 			i += 2;
// 			continue;
// 		}
// 		out[j++] = s[i++];
// 	}
// 	out[j] = '\0';
// 	return out;
// }

/* alloc + copy */
// int	extract_word(char *input, char **word)
// {
// 	int   len;
// 	char *raw;
// 	char *un;

// 	len = measure_word(input);                                /* scans span */
// 	raw = malloc((size_t)len + 1);
// 	if (!raw)
// 		return 0;
// 	ft_strncpy(raw, input, len);
// 	raw[len] = '\0';

// 	un = unescape_outside_quotes(raw);                        /* NEW */
// 	free(raw);
// 	if (!un)
// 		return 0;

// 	*word = un;
// 	return len;                                              /* advance input */
// }

int	extract_word(char *input, char **word)
{
	int	len;

	len = measure_word(input);
	*word = malloc((size_t)len + 1);
	if (!*word)
		return (0);
	ft_strncpy(*word, input, len);
	(*word)[len] = '\0';
	return (len);
}/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 14:16:03 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_redir	*parse_single_redirection(t_token **tokens)
{
	t_redir	*redir;

	if (!*tokens)
		return (NULL);
	if ((*tokens)->type != TOKEN_REDIR_IN
		&& (*tokens)->type != TOKEN_REDIR_OUT
		&& (*tokens)->type != TOKEN_REDIR_APPEND
		&& (*tokens)->type != TOKEN_REDIR_HEREDOC)
		return (NULL);
	if (!(*tokens)->next || !(*tokens)->next->value)
	{
		ft_putendl_fd(
			"minishell: syntax error near unexpected token "
			"`newline'",
			2);
		return (NULL);
	}
	redir = create_redir((*tokens)->type, (*tokens)->next->value);
	if (!redir)
		return (NULL);
	*tokens = (*tokens)->next->next;
	return (redir);
}

static int	count_args(t_token *tokens)
{
	int	count;

	count = 0;
	while (tokens && tokens->type == TOKEN_WORD)
	{
		count++;
		tokens = tokens->next;
		while (tokens && (tokens->type == TOKEN_REDIR_IN
				|| tokens->type == TOKEN_REDIR_OUT
				|| tokens->type == TOKEN_REDIR_APPEND
				|| tokens->type == TOKEN_REDIR_HEREDOC))
		{
			if (!tokens->next)
				return (count);
			tokens = tokens->next->next;
		}
	}
	return (count);
}

static t_cmd	*new_cmd(int arg_count)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->expanded = 0;
	cmd->args = malloc(sizeof(char *) * (arg_count + 1));
	if (!cmd->args)
	{
		free(cmd);
		return (NULL);
	}
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	consume_redirs(t_token **tokens, t_cmd *cmd)
{
	t_redir	*new_redir;

	while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN
			|| (*tokens)->type == TOKEN_REDIR_OUT
			|| (*tokens)->type == TOKEN_REDIR_APPEND
			|| (*tokens)->type == TOKEN_REDIR_HEREDOC))
	{
		new_redir = parse_single_redirection(tokens);
		if (!new_redir)
			break ;
		append_redir(&cmd->redirs, new_redir);
	}
}

t_cmd	*parse_command(t_token **tokens)
{
	t_cmd	*cmd;
	int		i;
	int		arg_count;

	if (!tokens || !*tokens)
		return (NULL);
	arg_count = count_args(*tokens);
	cmd = new_cmd(arg_count);
	if (!cmd)
		return (NULL);
	i = 0;
	while (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		cmd->args[i++] = ft_strdup((*tokens)->value);
		*tokens = (*tokens)->next;
		consume_redirs(tokens, cmd);
	}
	cmd->args[i] = NULL;
	return (cmd);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraser_check_token.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 12:32:12 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 13:52:15 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
/* Token validation functions */
int	is_valid_word(t_token *token)
{
	return (token && token->type == TOKEN_WORD);
}

int	is_control_operator(t_token *token)
{
	return (token && (token->type == TOKEN_PIPE
			|| token->type == TOKEN_AND
			|| token->type == TOKEN_OR));
}

int	is_redirection(t_token *token)
{
	return (token && (token->type == TOKEN_REDIR_IN
			|| token->type == TOKEN_REDIR_OUT
			|| token->type == TOKEN_REDIR_APPEND
			|| token->type == TOKEN_REDIR_HEREDOC));
}

int	is_gt(t_token *t)
{
	return (t && (t->type == TOKEN_REDIR_OUT || t->type == TOKEN_REDIR_APPEND));
}

int	is_lt(t_token *t)
{
	return (t && (t->type == TOKEN_REDIR_IN || t->type == TOKEN_REDIR_HEREDOC));
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:05:53 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:06:32 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	print_unexpected(char *s)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(s, 2);
	ft_putendl_fd("'", 2);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 16:18:14 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Create a new pipeline node
** Initializes pipeline with commands and logical operator
*/
static t_pipeline	*create_pipeline(void)
{
	t_pipeline	*pipeline;

	pipeline = malloc(sizeof(t_pipeline));
	if (!pipeline)
		return (NULL);
	pipeline->cmds = NULL;
	pipeline->logic_op = TOKEN_EOF;
	pipeline->next = NULL;
	return (pipeline);
}

/*
** Parse commands separated by pipes
** Builds a linked list of commands in a single pipeline
*/
static t_cmd	*parse_pipe_sequence(t_token **tokens)
{
	t_cmd	*cmds;
	t_cmd	*new_cmd;
	t_cmd	*current;

	cmds = NULL;
	while (*tokens && (*tokens)->type != TOKEN_AND
		&& (*tokens)->type != TOKEN_OR)
	{
		new_cmd = parse_command(tokens);
		if (!cmds)
			cmds = new_cmd;
		else
		{
			current = cmds;
			while (current->next)
				current = current->next;
			current->next = new_cmd;
		}
		if (*tokens && (*tokens)->type == TOKEN_PIPE)
			*tokens = (*tokens)->next;
		else
			break ;
	}
	return (cmds);
}

/* helpers */
static void	append_pipeline(t_pipeline **head, t_pipeline *new_node)
{
	t_pipeline	*cur;

	if (!new_node)
		return ;
	if (!*head)
	{
		*head = new_node;
		return ;
	}
	cur = *head;
	while (cur->next)
		cur = cur->next;
	cur->next = new_node;
}

/* helpers */
static void	set_logic_and_advance(t_pipeline *pl, t_token **tokens)
{
	if (!tokens || !*tokens)
		return ;
	if ((*tokens)->type == TOKEN_AND || (*tokens)->type == TOKEN_OR)
	{
		pl->logic_op = (*tokens)->type;
		*tokens = (*tokens)->next;
	}
}

t_pipeline	*parser(t_token *tokens)
{
	t_pipeline	*head;
	t_pipeline	*node;

	head = NULL;
	while (tokens)
	{
		node = create_pipeline();
		node->cmds = parse_pipe_sequence(&tokens);
		set_logic_and_advance(node, &tokens);
		append_pipeline(&head, node);
		if (!tokens || tokens->type == TOKEN_EOF)
			break ;
	}
	return (head);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraser_syntax_check.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:08:29 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* Add check for consecutive redirections */
static int	check_consecutive_redirections(t_token *token)
{
	if (is_redirection(token) && is_redirection(token->next))
	{
		print_syntax_error(token);
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}

/* Add validate_token_sequence function */
static int	validate_token_sequence(t_token *t, t_token *next)
{
	if (!check_consecutive_redirections(t))
		return (0);
	if (is_redirection(t) || is_control_operator(t))
	{
		if (!is_valid_word(next))
		{
			print_syntax_error(next);
			g_shell.exit_status = 258;
			return (0);
		}
	}
	return (1);
}

/* Update validate_syntax function */
/* src/parser/paraser_syntax_check.c */

static int	check_invalid_semi(t_token *t)
{
	if (t && t->type == TOKEN_SEMI)
	{
		print_unexpected(";");
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}

/* call inside your main validate loop */
int	validate_syntax(t_token *tokens, t_shell *shell)
{
	t_token	*t;
	t_token	*next;

	(void)shell;
	if (!tokens)
		return (1);
	t = tokens;
	if (!check_invalid_semi(t))
		return (0);
	while (t && t->next)
	{
		next = t->next;
		if (!check_invalid_semi(next))
			return (0);
		/* your existing sequence checks */
		if (!validate_token_sequence(t, next))
			return (0);
		t = t->next;
	}
	if (is_control_operator(t) || is_redirection(t))
	{
		print_syntax_error(NULL);
		g_shell.exit_status = 258;
		return (0);
	}
	return (1);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax_print.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 10:45:12 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	tok_op_len(t_token *t)
{
	if (!t)
		return (0);
	if (t->type == TOKEN_REDIR_APPEND || t->type == TOKEN_REDIR_HEREDOC)
		return (2);
	if (t->type == TOKEN_REDIR_OUT || t->type == TOKEN_REDIR_IN)
		return (1);
	return (0);
}

/* print_unexpected is now in parser_error.c */

/* replace only the '<' branch inside print_run_error(...) */
/* line 43: counts 1 for '<', 2 for '<<' */
/* line 56: return total >= 6 */
static void	handle_lt_run(t_token *t)
{
	int			total;
	t_token		*cur;

	total = 0;
	cur = t;
	while (is_lt(cur))
	{
		total += tok_op_len(cur);
		cur = cur->next;
	}
	if (total <= 3)
		return (print_unexpected("newline"));
	if (total == 4)
		return (print_unexpected("<"));
	if (total == 5)
		return (print_unexpected("<<"));
	return (print_unexpected("<<<"));
}

/* choose correct unexpected token for runs like >>>> or <<<< */
/* line 75: >>>, >>>> → `>>' */
void	print_run_error(t_token *t)
{
	int			total;
	t_token		*cur;

	total = 0;
	cur = t;
	if (is_gt(t))
	{
		while (is_gt(cur))
		{
			total += tok_op_len(cur);
			cur = cur->next;
		}
		if (total > 3)
			ft_putendl_fd(ERR_REDIR_APPEND, 2);
		else if (total == 2 && (!cur || is_control_operator(cur)
				|| is_redirection(cur)))
			ft_putendl_fd(ERR_NEWLINE, 2);
		else
			ft_putendl_fd(ERR_CONSECUTIVE_REDIR, 2);
		return ;
	}
	if (is_lt(t))
		return (handle_lt_run(t));
}

/* replace your redir branch with this */
void	print_syntax_error(t_token *token)
{
	if (!token)
	{
		print_unexpected("newline");
		return ;
	}
	else if (token->type == TOKEN_SEMI)
	{
		ft_putendl_fd(ERR_SEMI, 2);
		ft_putstr_fd("minishell: syntax error near unexpected token `;'\n", 2);
		return ;
	}
	else if (is_redirection(token) && is_redirection(token->next))
	{
		print_run_error(token);
		return ;
	}
	else if (token->type == TOKEN_PIPE)
		ft_putendl_fd(ERR_PIPE, 2);
	else if (token->type == TOKEN_AND)
		ft_putendl_fd(ERR_AND, 2);
	else if (token->type == TOKEN_OR)
		ft_putendl_fd(ERR_OR, 2);
	else if (token->type == TOKEN_REDIR_IN)
		ft_putendl_fd(ERR_REDIR_IN, 2);
	else if (token->type == TOKEN_REDIR_OUT)
		ft_putendl_fd(ERR_REDIR_OUT, 2);
	else if (token->type == TOKEN_REDIR_APPEND)
		ft_putendl_fd(ERR_REDIR_APPEND, 2);
	else if (token->type == TOKEN_REDIR_HEREDOC)
		ft_putendl_fd(ERR_REDIR_HEREDOC, 2);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 14:00:49 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_redir	*create_redir(t_token_type type, char *file)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = ft_strdup(file);
	redir->next = NULL;
	return (redir);
}

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

void	free_pipeline(t_pipeline *pipeline)
{
	t_pipeline	*tmp_pipe;
	t_cmd		*tmp_cmd;
	t_redir		*tmp_redir;

	while (pipeline)
	{
		tmp_pipe = pipeline;
		while (pipeline->cmds)
		{
			tmp_cmd = pipeline->cmds;
			free_array(tmp_cmd->args);
			while (tmp_cmd->redirs)
			{
				tmp_redir = tmp_cmd->redirs;
				tmp_cmd->redirs = tmp_redir->next;
				free(tmp_redir->file);
				free(tmp_redir);
			}
			pipeline->cmds = tmp_cmd->next;
			free(tmp_cmd);
		}
		pipeline = pipeline->next;
		free(tmp_pipe);
	}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 11:56:09 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	execute_external(t_cmd *cmd, t_shell *shell)
{
	struct stat	st;
	char		*path;
	pid_t		pid;
	int			status;

	if (!cmd || !cmd->args || !cmd->args[0])
		return ;

	/* Case 1: direct path given (contains /) */
	if (ft_strchr(cmd->args[0], '/'))
	{
		/* directory check first */
		if (stat(cmd->args[0], &st) == 0 && S_ISDIR(st.st_mode))
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->args[0], 2);
			ft_putendl_fd(": is a directory", 2);
			shell->exit_status = 126;
			return ;
		}
	}
	/* Case 2: normal PATH lookup */
	path = find_executable(cmd->args[0], shell->env);
	if (!path)
	{
		cmd_not_found(cmd->args[0]);
		shell->exit_status = 127;
		return ;
	}

	pid = fork();
	if (pid == -1)
	{
		print_error("fork", strerror(errno));
		free(path);
		shell->exit_status = 127;
		return ;
	}
	if (pid == 0)
	{
		if (setup_redirections(cmd->redirs) == -1)
			exit(1);
		execve(path, cmd->args, env_to_array(shell->env));
		if (errno == EACCES)
			exit(126);
		else
			exit(127);
	}
	free(path);
/* PARENT: read child status and save it */
	status = 0;
	if (waitpid(pid, &status, 0) == -1)
	{
		print_error("waitpid", strerror(errno));
		shell->exit_status = 1;
	}
	else
	{
		if (WIFEXITED(status))
			shell->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			shell->exit_status = 128 + WTERMSIG(status);
	}
}

static void	execute_builtin_with_redir(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
	{
		if (setup_redirections(cmd->redirs) == -1)
			exit(1);
		exit(execute_builtin(cmd, shell));
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		shell->exit_status = WEXITSTATUS(status);
}

/* stderr helpers you likely already have; keep your own if present */
static void put2(const char *s) { if (s) write(2, s, (int)ft_strlen(s)); }

static void perror_with_cmd(const char *cmd)
{
	put2("minishell: ");
	put2(cmd);
	put2(": ");
	put2(strerror(errno));
	put2("\n");
}

static int is_directory(const char *path)
{
	struct stat st;

	if (stat(path, &st) == -1)
		return 0;
	if (S_ISDIR(st.st_mode))
		return 1;
	return 0;
}

static int has_slash(const char *s)
{
	int i;

	if (!s)
		return 0;
	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == '/')
			return 1;
		i++;
	}
	return 0;
}


void	execute_commands(t_cmd *cmd, t_shell *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (is_builtin(cmd->args[0]))
	{
		if (cmd->redirs)
			execute_builtin_with_redir(cmd, shell);
		else
			shell->exit_status = execute_builtin(cmd, shell);
	}
	else
		execute_external(cmd, shell);
}

void execute_command(char **argv, t_shell *shell, char **envp)
{
	char *path;

	if (!argv || !argv[0] || !argv[0][0])
	{
		shell->exit_status = 0;
		return;
	}

	/* resolve path: direct if contains '/', else search PATH */
	path = NULL;
	if (has_slash(argv[0]) == 1)
		path = ft_strdup(argv[0]);
	else
		path = find_executable(argv[0], shell->env);

	/* not found -> 127 */
	if (!path)
	{
		cmd_not_found(argv[0]);
		shell->exit_status = 127;
		return;
	}

	/* directory -> 126 (e.g., //) */
	if (is_directory(path) == 1)
	{
		put2("minishell: ");
		put2(argv[0]);
		put2(": is a directory\n");
		free(path);
		shell->exit_status = 126;
		return;
	}

	/* permission / existence checks before execve */
	if (access(path, X_OK) == -1)
	{
		if (errno == EACCES)
		{
			perror_with_cmd(argv[0]);   /* “Permission denied” */
			free(path);
			shell->exit_status = 126;   /* found but not executable */
			return;
		}
		/* any other reason (e.g., removed between resolve and access) */
		perror_with_cmd(argv[0]);
		free(path);
		shell->exit_status = 127;
		return;
	}

	/* try to execute */
	execve(path, argv, envp);

	/* execve only returns on error */
	if (errno == EACCES)
		shell->exit_status = 126;
	else
		shell->exit_status = 127;
	perror_with_cmd(argv[0]);
	free(path);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_fds.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 13:11:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	setup_input_fd(int prev_read_fd)
{
	if (prev_read_fd >= 0)
	{
		if (dup2(prev_read_fd, STDIN_FILENO) == -1)
		{
			print_error("dup2", "failed to redirect stdin");
			return (-1);
		}
		safe_close(prev_read_fd);
	}
	return (0);
}

/*
** setup_output_fd - Redirect stdout to next pipe if needed
**
** @param pipefd: Current pipe file descriptors
** @param has_next: 1 if there's a next command, 0 if last
**
** Return: 0 on success, -1 on error
*/
static int	setup_output_fd(int pipefd[2], int has_next)
{
	if (has_next)
	{
		safe_close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			print_error("dup2", "failed to redirect stdout");
			return (-1);
		}
		safe_close(pipefd[1]);
	}
	return (0);
}

/*
** setup_child_fds - Set up all file descriptors for child process
**
** @param pipefd: Current pipe fds
** @param prev_read_fd: Previous pipe read end
** @param has_next: 1 if not last command
**
** Return: 0 on success, -1 on error
*/
int	setup_child_fds(int pipefd[2], int prev_read_fd, int has_next)
{
	if (setup_input_fd(prev_read_fd) == -1)
		return (-1);
	if (setup_output_fd(pipefd, has_next) == -1)
		return (-1);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_run.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/13 14:25:43 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** execute_builtin_child - Execute builtin command in child process
**
** @param cmd: Command structure
** @param shell: Shell state
*/
static void	execute_builtin_child(t_cmd *cmd, t_shell *shell)
{
	int	exit_code;

	exit_code = execute_builtin(cmd, shell);
	exit(exit_code);
}

/*
** execute_external_child - Execute external command in child process
**
** @param cmd: Command structure
** @param shell: Shell state
*/
static void	execute_external_child(t_cmd *cmd, t_shell *shell, char *path)
{
	char	**envp;

	envp = env_to_array(shell->env);
	if (!envp)
	{
		print_error("env_to_array", "allocation failed");
		free(path);
		exit(1);
	}
	execve(path, cmd->args, envp);
	free_array(envp);
	free(path);
	print_error("execve", strerror(errno));
	exit(126);
}

/*
** execute_cmd_child - Execute command in child process
**
** This function handles the complete child execution including
** redirections and command type detection.
**
** @param cmd: Command to execute
** @param shell: Shell state
*/
static void	execute_cmd_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd || !cmd->args || !cmd->args[0])
		exit(0);
	if (setup_redirections(cmd->redirs) == -1)
		exit(1);
	if (is_builtin(cmd->args[0]))
		execute_builtin_child(cmd, shell);
	else
	{
		path = find_executable(cmd->args[0], shell->env);
		if (!path)
		{
			cmd_not_found(cmd->args[0]);  // argv[0] == "\\", shown as "\"
			shell->exit_status = 127;
			return ;
		}
		execute_external_child(cmd, shell, path);
	}
}

/*
** create_child_process - Fork and execute command in child
**
** @param cmd: Command to execute
** @param shell: Shell state
** @param pipefd: Current pipe fds
** @param prev_read_fd: Previous pipe read end
** @param has_next: 1 if not last command
**
** Return: Child PID on success, -1 on error
*/
/* was: (t_cmd*, t_shell*, int pipefd[2], int prev_read_fd, int has_next) */
pid_t	create_child_process(t_cmd *cmd, t_shell *shell, t_child_io *io)
{
	pid_t	pid;
	int		pipefd[2];

	pid = fork();
	if (pid == -1)
	{
		print_error("fork", strerror(errno));
		return (-1);
	}
	if (pid == 0)
	{
		/* CHILD PROCESS: restore default signal behavior */
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);

		pipefd[0] = io->pipe_rd;
		pipefd[1] = io->pipe_wr;
		if (setup_child_fds(pipefd, io->prev_rd, io->has_next) == -1)
			exit(1);

		execute_cmd_child(cmd, shell);
		exit(1); /* should not reach here if execve succeeds */
	}
	return (pid);
}/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_error.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:12:28 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:41:19 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	cmd_not_found(char *name)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(name, 2);
	ft_putendl_fd(": command not found", 2);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 00:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:39:20 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** find_executable - Locate executable file in PATH or validate path
** This function searches for an executable command in the system PATH or
** validates an absolute/relative path. It implements the following logic:
**
*/

/* regular file + X_OK */
static int	is_exec_file(const char *path)
{
	struct stat	st;

	if (!path)
		return (0);
	if (stat(path, &st) != 0)
		return (0);
	if (!S_ISREG(st.st_mode))
		return (0);
	if (access(path, X_OK) != 0)
		return (0);
	return (1);
}

/* dir + "/" + cmd */
static char	*join_cmd_path(const char *dir, const char *cmd)
{
	size_t	a;
	size_t	b;
	char	*s;

	a = ft_strlen(dir);
	b = ft_strlen(cmd);
	s = malloc(a + b + 2);
	if (!s)
		return (NULL);
	ft_memcpy(s, dir, a);
	s[a] = '/';
	ft_memcpy(s + a + 1, cmd, b);
	s[a + b + 1] = '\0';
	return (s);
}

/* end index of PATH segment */
static size_t	seg_end(const char *path, size_t start)
{
	size_t	i;

	i = start;
	while (path[i] && path[i] != ':')
		i++;
	return (i);
}

/* "." if empty, else substring */
static char	*dup_segment_or_dot(const char *path, size_t start, size_t end)
{
	if (end == start)
		return (ft_strdup("."));
	return (ft_substr(path, start, end - start));
}

/* try dir/cmd and return full if executable */
static char	*probe_dir_for_cmd(const char *dir, const char *cmd)
{
	char	*full;

	full = join_cmd_path(dir, cmd);
	if (!full)
		return (NULL);
	if (is_exec_file(full))
		return (full);
	free(full);
	return (NULL);
}

/* iterate PATH entries */
static char	*search_in_path(const char *path, const char *cmd)
{
	size_t	i;
	size_t	j;
	char	*dir;
	char	*hit;

	if (!path)
		return (NULL);
	i = 0;
	while (1)
	{
		j = seg_end(path, i);
		dir = dup_segment_or_dot(path, i, j);
		if (!dir)
			return (NULL);
		hit = probe_dir_for_cmd(dir, cmd);
		free(dir);
		if (hit) return (hit);
		if (!path[j])
			break ;
		i = j + 1;
	}
	return (NULL);
}

/* top-level resolver */
char	*find_executable(char *cmd, t_env *env)
{
	char	*path;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (is_exec_file(cmd))
			return (ft_strdup(cmd));
		return (NULL);
	}
	path = get_env_value(env, "PATH");
	return (search_in_path(path, cmd));
}
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_loop.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 15:46:41 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** init_pipeline - Initialize pipeline execution
**
** Allocates memory for pid array.
**
** @param cmd_count: Number of commands
** @param pids: Pointer to store allocated pid array
**
** Return: 0 on success, -1 on error
*/
int	init_pipeline(int cmd_count, pid_t **pids)
{
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		print_error("malloc", "failed to allocate pid array");
		return (-1);
	}
	return (0);
}

/*
** update_prev_fd - Update previous read fd for next iteration
**
** Closes old prev_read_fd and updates it with new pipe read end.
**
** @param prev_read_fd: Current previous read fd
** @param pipefd: Current pipe fds
** @param has_next: 1 if there's a next command
**
** Return: New prev_read_fd value
*/
int	update_prev_fd(int prev_read_fd, int pipefd[2], int has_next)
{
	safe_close(prev_read_fd);
	if (has_next)
	{
		safe_close(pipefd[1]);
		return (pipefd[0]);
	}
	return (-1);
}

/*
** execute_one_command - Execute one command in the pipeline
**
** Creates pipe if needed, forks child, and updates file descriptors.
**
** @param cmd: Current command
** @param shell: Shell state
** @param pids: Array to store child pid
** @param index: Index in pids array
** @param prev_read_fd: Pointer to previous read fd
**
** Return: 0 on success, -1 on error
*/
/* was: (t_cmd*, t_shell*, pid_t*, int, int*) */

/* prepare pipe + child io */
static int	prepare_child_io(t_cmd *cmd, int prev_rd,
				int pipefd[2], t_child_io *io)
{
	int	has_next;

	has_next = 0;
	if (cmd && cmd->next)
		has_next = 1;
	io->has_next = has_next;
	io->prev_rd = prev_rd;
	if (has_next)
	{
		if (pipe(pipefd) == -1)
		{
			print_error("pipe", strerror(errno));
			return (-1);
		}
		io->pipe_rd = pipefd[0];
		io->pipe_wr = pipefd[1];
	}
	else
	{
		io->pipe_rd = -1;
		io->pipe_wr = -1;
	}
	return (0);
}

/* close fds and move read end to prev_rd */
static void	finalize_after_fork(t_child_io *io, int pipefd[2], int *prev_rd)
{
	safe_close(*prev_rd);
	if (io->has_next)
	{
		safe_close(pipefd[1]);
		*prev_rd = pipefd[0];
	}
	else
		*prev_rd = -1;
}

int	execute_one_command(t_cmd *cmd, int index, t_pipe_ctx *ctx)
{
	int			pipefd[2];
	t_child_io	io;

	if (prepare_child_io(cmd, *ctx->prev_rd, pipefd, &io) == -1)
		return (-1);
	ctx->pids[index] = create_child_process(cmd, ctx->shell, &io);
	if (ctx->pids[index] == -1)
	{
		if (io.has_next)
		{
			safe_close(pipefd[0]);
			safe_close(pipefd[1]);
		}
		return (-1);
	}
	finalize_after_fork(&io, pipefd, ctx->prev_rd);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_run.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/13 11:42:04 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** execute_pipeline_loop - Main loop for pipeline execution
**
** Iterates through commands, creating pipes and forking children.
**
** @param cmds: Command list
** @param shell: Shell state
** @param pids: Array to store child pids
** @param cmd_count: Number of commands
**
** Return: 0 on success, -1 on error
*/
int	execute_pipeline_loop(t_cmd *cmds, t_shell *shell,
				pid_t *pids, int cmd_count)
{
	int			i;
	int			prev_read_fd;
	t_cmd		*current;
	t_pipe_ctx	ctx;

	i = 0;
	current = cmds;
	prev_read_fd = -1;
	ctx.shell = shell;
	ctx.pids = pids;
	ctx.prev_rd = &prev_read_fd;
	while (current && i < cmd_count)
	{
		if (execute_one_command(current, i, &ctx) == -1)
			return (-1);
		current = current->next;
		i++;
	}
	return (0);
}

/* helper: run multi-command pipeline */
static int	execute_multi_pipeline(t_cmd *cmds, t_shell *shell, int count)
{
	pid_t	*pids;
	int		ret;

	if (init_pipeline(count, &pids) == -1)
		return (1);
	if (execute_pipeline_loop(cmds, shell, pids, count) == -1)
	{
		free(pids);
		return (1);
	}
	ret = wait_for_children(pids, count);
	free(pids);
	shell->exit_status = ret;
	return (ret);
}

/* run one builtin without forking if no redirs */
int	execute_single_builtin_parent(t_cmd *cmd, t_shell *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (!is_builtin(cmd->args[0]))
		return (-1);
	if (cmd->redirs != NULL)
		return (-1);
	if (!cmd->expanded)
	{
		expand_cmd_args(cmd, shell->env, shell->exit_status);
		cmd->expanded = 1;
	}
	return (execute_builtin(cmd, shell));
}

/* public entry used by executor() */
int	execute_pipeline(t_cmd *cmds, t_shell *shell)
{
	int	count;
	int	ret;

	if (!cmds)
		return (0);
	count = count_commands(cmds);
	if (count == 1)
	{
		ret = execute_single_builtin_parent(cmds, shell);
		if (ret != -1)
		{
			shell->exit_status = ret;
			return (ret);
		}
		execute_commands(cmds, shell);
		return (shell->exit_status);
	}
	return (execute_multi_pipeline(cmds, shell, count));
}

int	wait_for_children(pid_t *pids, int count)
{
	int	i;
	int	status;
	int	last_status;

	last_status = 0;
	i = 0;
	while (i < count)
	{
		if (pids[i] > 0 && waitpid(pids[i], &status, 0) == -1)
			print_error("waitpid", strerror(errno));
		else if (pids[i] > 0 && WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (pids[i] > 0 && WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
		i++;
	}
	return (last_status);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 15:47:05 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** print_error - Print formatted error message to stderr
**
** @param function: Name of the function that failed
** @param message: Error message or strerror(errno)
*/
void	print_error(const char *function, const char *message)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)function, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd((char *)message, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

/*
** safe_close - Safely close a file descriptor
**
** Closes fd if valid (>= 0) and prints error if close fails.
**
** @param fd: File descriptor to close
*/
void	safe_close(int fd)
{
	if (fd >= 0)
	{
		if (close(fd) == -1)
			print_error("close", strerror(errno));
	}
}

/*
** cleanup_pipe - Close both ends of a pipe
**
** @param pipefd: Array containing pipe file descriptors [read, write]
*/
void	cleanup_pipe(int pipefd[2])
{
	safe_close(pipefd[0]);
	safe_close(pipefd[1]);
}

/*
** count_commands - Count the number of commands in a pipeline
**
** This helper function traverses the linked list of commands and counts
** how many commands are present. This count is used to determine:
** - Whether we need to create pipes (count > 1)
** - How many child processes to wait for
** - Optimization: single commands can skip pipe creation
**
** @param cmds: Head of the command linked list
**
** Return: Number of commands in the list (0 if cmds is NULL)
*/
int	count_commands(t_cmd *cmds)
{
	int		count;
	t_cmd	*current;

	count = 0;
	current = cmds;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/*
** create_pipe_if_needed - Create pipe if not last command
**
** @param current: Current command
** @param pipefd: Array to store pipe fds
**
** Return: 0 on success, -1 on error
*/
int	create_pipe_if_needed(t_cmd *current, int pipefd[2])
{
	if (current->next)
	{
		if (pipe(pipefd) == -1)
		{
			print_error("pipe", strerror(errno));
			return (-1);
		}
	}
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 12:50:35 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_heredoc(char *delimiter)
{
	int		pipe_fd[2];
	char	*line;
	char	*expanded;

	if (pipe(pipe_fd) == -1)
		return (-1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		/* expand variables in heredoc line, like bash does for unquoted EOF */
		expanded = expand_variables(line, g_shell.env, g_shell.exit_status);
		if (!expanded)
		{
			free(line);
			close(pipe_fd[1]);
			close(pipe_fd[0]);
			return (-1);
		}
		write(pipe_fd[1], expanded, ft_strlen(expanded));
		write(pipe_fd[1], "\n", 1);
		free(line);
		free(expanded);
	}
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	return (0);
}

static int	handle_input(char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(file, 2);
		ft_putendl_fd(": No such file or directory", 2);
		return (-1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	handle_output(char *file, int append)
{
	int	fd;
	int	flags;

	if (append)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd = open(file, flags, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(file, 2);
		ft_putendl_fd(": Permission denied", 2);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	setup_redirections(t_redir *redirs)
{
	while (redirs)
	{
		if (redirs->type == TOKEN_REDIR_IN)
		{
			if (handle_input(redirs->file) == -1)
				return (-1);
		}
		else if (redirs->type == TOKEN_REDIR_OUT)
		{
			if (handle_output(redirs->file, 0) == -1)
				return (-1);
		}
		else if (redirs->type == TOKEN_REDIR_APPEND)
		{
			if (handle_output(redirs->file, 1) == -1)
				return (-1);
		}
		else if (redirs->type == TOKEN_REDIR_HEREDOC)
		{
			if (handle_heredoc(redirs->file) == -1)
				return (-1);
		}
		redirs = redirs->next;
	}
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* return heap string; never NULL, returns "" if src is NULL */
static char	*safe_strdup_or_empty(const char *src)
{
	if (src == NULL)
		return (ft_strdup(""));
	return (ft_strdup((char *)src));
}

/* update or add env key=value, never store NULL values */
static int	set_env_kv(t_env **env, const char *key, const char *value)
{
	t_env	*cur;

	if (!env || !key)
		return (1);
	cur = *env;
	while (cur)
	{
		if (ft_strcmp(cur->key, (char *)key) == 0)
		{
			free(cur->value);
			cur->value = safe_strdup_or_empty(value);
			return (0);
		}
		cur = cur->next;
	}
	add_env_node(env, create_env_node((char *)key,
			safe_strdup_or_empty(value)));
	return (0);
}

/* handle chdir + print errors */
static int	change_directory(char *target)
{
	if (chdir(target) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(target, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		return (1);
	}
	return (0);
}

/* helper: update OLDPWD and PWD after successful chdir */
static void	update_pwd_vars(t_env **env, char *oldpwd, int print_after)
{
	char	*newpwd;

	if (oldpwd)
		set_env_kv(env, "OLDPWD", oldpwd);
	newpwd = dup_cwd();
	if (newpwd)
	{
		set_env_kv(env, "PWD", newpwd);
		if (print_after)
			ft_putendl_fd(newpwd, 1);
	}
	free(newpwd);
}

/* main builtin_cd
** Change directory builtin command
** Changes to HOME if no argument, otherwise changes to specified directory
** Returns 0 on success, 1 on failure
*/
int	builtin_cd(char **args, t_env **env)
{
	char	*oldpwd;
	char	*target;
	int		print_after;

	oldpwd = dup_cwd();
	target = resolve_target(args, *env, &print_after);
	if (!target)
		return (1);
	if (change_directory(target))
	{
		free(target);
		free(oldpwd);
		return (1);
	}
	update_pwd_vars(env, oldpwd, print_after);
	free(oldpwd);
	free(target);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:45:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 13:45:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Dup_cwd: Duplicate current working directory
** Returns heap-allocated string with current directory or NULL on failure
*/
char	*dup_cwd(void)
{
	char	buf[4096];

	if (getcwd(buf, sizeof(buf)) == NULL)
		return (NULL);
	return (ft_strdup(buf));
}

/*
** Resolve cd target directory
** Handles: no args (HOME), "-" (OLDPWD), or explicit path
** Sets print_after flag for "-" case
** Returns heap-allocated target path or NULL on error
*/
static char	*dup_env_or_err(t_env *env, const char *key, const char *errmsg)
{
	char	*v;

	v = get_env_value(env, (char *)key);
	if (!v)
	{
		ft_putendl_fd((char *)errmsg, 2);
		return (NULL);
	}
	return (ft_strdup(v));
}

char	*resolve_target(char **args, t_env *env, int *print_after)
{
	*print_after = 0;
	if (!args[1])
		return (dup_env_or_err(env, "HOME", "minishell: cd: HOME not set"));
	if (ft_strcmp(args[1], "-") == 0)
	{
		*print_after = 1;
		return (dup_env_or_err(env, "OLDPWD", "minishell: cd: OLDPWD not set"));
	}
	return (ft_strdup(args[1]));
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Echo builtin command
** Prints arguments to stdout, supports -n flag to suppress newline
** Returns 0 on success
*/
static int	is_dash_n(char *s)
{
	int	i;

	if (!s || s[0] != '-' || s[1] != 'n')
		return (0);
	i = 2;
	while (s[i] == 'n')
		i++;
	return (s[i] == '\0');
}

int	builtin_echo(char **args)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (args[i] && is_dash_n(args[i]))
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], 1);
		if (args[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
	if (newline)
		ft_putstr_fd("\n", 1);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Environment variables builtin command
** Prints all environment variables in KEY=VALUE format
** Returns 0 on success
*/
int	builtin_env(t_env *env)
{
	while (env)
	{
		if (env->value)
		{
			ft_putstr_fd(env->key, 1);
			ft_putstr_fd("=", 1);
			ft_putendl_fd(env->value, 1);
		}
		env = env->next;
	}
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 11:09:40 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	parse_sign(const char *s, int *i)
{
	int	sign;

	sign = 1;
	if (s[*i] == '+' || s[*i] == '-')
	{
		if (s[*i] == '-')
			sign = -1;
		(*i)++;
	}
	return (sign);
}

static int	parse_digits(const char *s, int *i, long long *val)
{
	long long	v;
	int			j;

	j = *i;
	if (!ft_isdigit((unsigned char)s[j]))
		return (0);
	v = 0;
	while (ft_isdigit((unsigned char)s[j]))
	{
		v = v * 10 + (s[j] - '0');
		j++;
	}
	*i = j;
	*val = v;
	return (1);
}

/*
** Check if string is a valid number
** Returns 1 if numeric, 0 otherwise
*/
static int	is_numeric_ll(const char *s, long long *out)
{
	long long	val;
	int			sign;
	int			i;

	if (!s || !*s)
		return (0);
	i = 0;
	sign = parse_sign(s, &i);
	if (!parse_digits(s, &i, &val))
		return (0);
	if (s[i] != '\0')
		return (0);
	*out = val * sign;
	return (1);
}

/*
** Exit builtin command
** Exits the shell with optional exit code
** Returns exit code or 0
*/
int	builtin_exit(char **args, t_shell *shell)
{
	long long	ll;
	int			exit_code;

	ft_putendl_fd("exit", 1);
	if (args[1])
	{
		if (!is_numeric_ll(args[1], &ll))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(args[1], 2);
			ft_putendl_fd(": numeric argument required", 2);
			shell->should_exit = 1;
			return (255);
		}
		if (args[2])
		{
			ft_putendl_fd("minishell: exit: too many arguments", 2);
			return (1);
		}
		exit_code = (unsigned char)ll;
		shell->should_exit = 1;
		return (exit_code);
	}
	shell->should_exit = 1;
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Parse KEY=VALUE format
** Splits the string at '=' and returns key and value
*/
static void	parse_export_arg(char *arg, char **key, char **value)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (eq)
	{
		*key = ft_substr(arg, 0, eq - arg);
		*value = ft_strdup(eq + 1);
	}
	else
	{
		*key = ft_strdup(arg);
		*value = NULL;
	}
}

/*
** Update or add environment variable
** If key exists, update value; otherwise create new node
*/
static void	update_or_add_env(t_env **env, char *key, char *value)
{
	t_env	*current;
	t_env	*new_node;

	current = *env;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (value)
			{
				free(current->value);
				current->value = ft_strdup(value);
			}
			return ;
		}
		current = current->next;
	}
	new_node = create_env_node(key, value);
	add_env_node(env, new_node);
}

/*
** Export builtin command
** Sets environment variables in KEY=VALUE format
** Returns 0 on success
*/
int	builtin_export(char **args, t_env **env)
{
	int		i;
	char	*key;
	char	*value;

	if (!args[1])
		return (builtin_env(*env));
	i = 1;
	while (args[i])
	{
		parse_export_arg(args[i], &key, &value);
		update_or_add_env(env, key, value);
		free(key);
		free(value);
		i++;
	}
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Print working directory builtin command
** Prints the current working directory to stdout
** Returns 0 on success, 1 on failure
*/
int	builtin_pwd(void)
{
	char	cwd[4096];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		ft_putendl_fd("minishell: pwd: error getting current directory", 2);
		return (1);
	}
	ft_putendl_fd(cwd, 1);
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	valid_ident_unset(char *s)
{
	int	i;

	if (!s || !(ft_isalpha((unsigned char)s[0]) || s[0] == '_'))
		return (0);
	i = 1;
	while (s[i])
	{
		if (!(ft_isalnum((unsigned char)s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

/*
** Unset builtin command
** Removes environment variables by key
** Returns 0 on success
*/
int	builtin_unset(char **args, t_env **env)
{
	int	i;

	if (!args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		if (valid_ident_unset(args[i]))
			remove_env_node(env, args[i]);
		i++;
	}
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 11:21:11 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Check if a command is a built-in
** Returns 1 if builtin, 0 otherwise
*/
int	is_builtin(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	if (!ft_strcmp(cmd, "history"))
		return (1);
	return (0);
}

/*
** Execute a built-in command
** Dispatches to the appropriate builtin function
*/
int	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		return (builtin_echo(cmd->args));
	if (ft_strcmp(cmd->args[0], "cd") == 0)
		return (builtin_cd(cmd->args, &shell->env));
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(cmd->args[0], "export") == 0)
		return (builtin_export(cmd->args, &shell->env));
	if (ft_strcmp(cmd->args[0], "unset") == 0)
		return (builtin_unset(cmd->args, &shell->env));
	if (ft_strcmp(cmd->args[0], "env") == 0)
		return (builtin_env(shell->env));
	if (ft_strcmp(cmd->args[0], "exit") == 0)
		return (builtin_exit(cmd->args, shell));
	if (ft_strcmp(cmd->args[0], "history") == 0)
		return (builtin_history(cmd->args));
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 15:08:35 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern t_shell	g_shell;

/* outside quotes: continue ONLY if an odd number of trailing '\' */
/* Return 1 if line ends with a backslash outside quotes → needs continuation */
static int	trailing_backslash_needs_more(const char *s)
{
	size_t	i;
	char	in_quote;
	int		escaped;

	if (!s || !*s)
		return (0);
	i = 0;
	in_quote = 0;
	escaped = 0;
	while (s[i] != '\0')
	{
		if (!in_quote && (s[i] == '\'' || s[i] == '"'))
		{
			in_quote = s[i];
			escaped = 0;
		}
		else if (in_quote)
		{
			if (s[i] == in_quote && !escaped)
				in_quote = 0;
			if (in_quote == '"' && s[i] == '\\' && !escaped)
				escaped = 1;
			else
				escaped = 0;
		}
		i++;
	}
	if (in_quote)
		return (1);
	i = ft_strlen(s);
	while (i > 0 && (s[i - 1] == ' ' || s[i - 1] == '\t'))
		i--;
	if (i == 0)
		return (0);
	if (s[i - 1] == '\\')
		return (1);
	return (0);
}

static int	needs_continuation(const char *s)
{
	if (has_unclosed_quotes((char *)s))
		return (1);
	return (trailing_backslash_needs_more(s));
}

/* Remove one trailing '\' (if any), then append the next line */
static char	*join_continuation(char *line, char *next)
{
	size_t	len;
	char	*tmp;
	char	*nptr;

	nptr = next;
	if (!nptr)
		nptr = "";
	if (!line)
		return (ft_strdup(nptr));
	len = ft_strlen(line);
	while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t'))
		len--;
	if (len > 0 && line[len - 1] == '\\')
		line[len - 1] = '\0';
	tmp = ft_strjoin(line, nptr);
	free(line);
	return (tmp);
}

/*
** \001 and \002 are control characters (SOH and STX).
** Readline uses them to know “this part does not take cursor space”.
** \001 and \002: tell readline that the color codes don't take cursor space
*/
char    *read_logical_line(void)
{
    char    *line;
    char    *more;

	/* start fresh: no pending SIGINT */
	g_shell.sigint_during_read = 0;
    line = readline("\001\033[1;33m\002minishell> \001\033[0m\002");
    if (!line)
        return (NULL);
	if (g_shell.sigint_during_read)
	{
		free(line);
		g_shell.exit_status = 130;
		return (NULL);
	}
    while (needs_continuation(line))
    {
		/* reset flag before each continuation read */
		g_shell.sigint_during_read = 0;
		more = readline("> ");
		/* Ctrl-C during continuation: cancel whole command */
		if (g_shell.sigint_during_read)
		{
			if (more)
				free(more);
			free(line);
			return (NULL);
		}
		if (!more)
        {
            free(line);
			g_shell.exit_status = 130;
            return (NULL);
        }
        line = join_continuation(line, more);
        free(more);
    }
    return (line);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 14:31:55 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** increment_shlvl - Increment the SHLVL environment variable
**
** SHLVL tracks the nesting level of shells. Each time a new shell
** is started, SHLVL should be incremented by 1.
**
** @param env: Pointer to environment linked list
**
** Return: void
*/
void	increment_shlvl(t_env **env)
{
	t_env	*current;
	int		shlvl_value;

	current = *env;
	while (current)
	{
		if (ft_strcmp(current->key, "SHLVL") == 0)
		{
			shlvl_value = ft_atoi(current->value);
			if (shlvl_value < 0)
				shlvl_value = 0;
			shlvl_value++;
			free(current->value);
			current->value = ft_itoa(shlvl_value);
			return ;
		}
		current = current->next;
	}
	add_env_node(env, create_env_node("SHLVL", "1"));
}

/*
** Initialize environment from envp array
** Creates linked list of environment variables
*/
t_env	*init_env(char **envp)
{
	t_env	*env;
	int		i;
	char	*key;
	char	*value;
	char	*eq_pos;

	env = NULL;
	i = 0;
	while (envp[i])
	{
		eq_pos = ft_strchr(envp[i], '=');
		if (eq_pos)
		{
			key = ft_substr(envp[i], 0, eq_pos - envp[i]);
			value = ft_strdup(eq_pos + 1);
			add_env_node(&env, create_env_node(key, value));
			free(key);
			free(value);
		}
		i++;
	}
	return (env);
}

void	init_shell(t_shell *shell, char **envp)
{
	shell->env = init_env(envp);
	shell->exit_status = 0;
	shell->should_exit = 0;
	shell->sigint_during_read = 0;
	increment_shlvl(&shell->env);
	history_init(shell->env);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_array.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 11:20:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*build_keyvalue_pair(const char *k, const char *v)
{
	size_t	lenk;
	size_t	lenv;
	char	*kv;

	if (!k || !*k || v == NULL)
		return (NULL);
	lenk = ft_strlen(k);
	lenv = ft_strlen(v);
	kv = malloc(lenk + lenv + 2);
	if (!kv)
		return (NULL);
	ft_memcpy(kv, k, lenk);
	kv[lenk] = '=';
	ft_memcpy(kv + lenk + 1, v, lenv);
	kv[lenk + lenv + 1] = '\0';
	return (kv);
}

/* main: append key=value to env array */
/* pseudo, adapt to your code */
/* build "KEY=VALUE" and push into NULL-terminated env array */
int	append_env(char ***arr, size_t *n, const char *k, const char *v)
{
	char	*kv;
	char	**newarr;
	size_t	i;

	if (!arr || !n)
		return (0);
	kv = build_keyvalue_pair(k, v);
	if (!kv)
		return (0);
	newarr = malloc(sizeof(char *) * (*n + 2));
	if (!newarr)
		return (free(kv), -1);
	i = 0;
	while (i < *n)
	{
		newarr[i] = (*arr)[i];
		i++;
	}
	newarr[*n] = kv;
	newarr[*n + 1] = NULL;
	free(*arr);
	*arr = newarr;
	*n += 1;
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:02:18 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Create a new environment variable node
** Allocates and initializes with key and value
*/
t_env	*create_env_node(char *key, char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	if (value)
		node->value = ft_strdup(value);
	else
		node->value = NULL;
	node->next = NULL;
	return (node);
}

/*
** Add environment node to the end of the list
** Traverses to last node and appends new node
*/
void	add_env_node(t_env **env, t_env *new_node)
{
	t_env	*current;

	if (!*env)
	{
		*env = new_node;
		return ;
	}
	current = *env;
	while (current->next)
		current = current->next;
	current->next = new_node;
}

/*
** Remove environment variable by key
** Frees the node and updates the list
*/
void	remove_env_node(t_env **env, char *key)
{
	t_env	*current;
	t_env	*prev;

	current = *env;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*env = current->next;
			free(current->key);
			free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:04:12 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Parse environment string (KEY=VALUE)
** Splits at '=' and returns key and value
*/
void	parse_env_string(char *env_str, char **key, char **value)
{
	char	*eq;

	eq = ft_strchr(env_str, '=');
	if (eq)
	{
		*key = ft_substr(env_str, 0, eq - env_str);
		*value = ft_strdup(eq + 1);
	}
	else
	{
		*key = ft_strdup(env_str);
		*value = NULL;
	}
}

/*
** Count environment variables
** Returns the number of nodes in the list
*/
static int	count_env(t_env *env)
{
	int	count;

	count = 0;
	while (env)
	{
		if (env->value)
			count++;
		env = env->next;
	}
	return (count);
}

/*
** Convert environment list to array
** Creates char** array in KEY=VALUE format for execve
*/
char	**env_to_array(t_env *env)
{
	char	**envp;
	char	*tmp;
	int		i;

	envp = malloc(sizeof(char *) * (count_env(env) + 1));
	if (!envp)
		return (NULL);
	i = 0;
	while (env)
	{
		if (env->value)
		{
			tmp = ft_strjoin(env->key, "=");
			envp[i] = ft_strjoin(tmp, env->value);
			free(tmp);
			i++;
		}
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:26:29 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_core.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:23:36 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* forward decl only; real body is in expander_utils.c */
void	process_dollar(t_exp_ctx *c);

static int	init_ctx(t_exp_ctx *c, char *s, t_env *env, int st)
{
	size_t	cap;

	if (!s)
		return (0);
	c->str = s;
	c->env = env;
	c->exit_status = st;
	c->in_quote = 0;
	c->i = 0;
	c->j = 0;
	cap = ft_strlen(s) * 10 + 4096;
	c->result = malloc(cap);
	return (c->result != NULL);
}

/* Helper: check if current character is a quote */
static int	is_quote(t_exp_ctx *c)
{
	char	ch;

	ch = c->str[c->i];
	return (ch == '\'' || ch == '"');
}

/* Helper: handle quote toggling during expansion */
static void	handle_quote(t_exp_ctx *c)
{
	if (!c->in_quote)
		c->in_quote = c->str[c->i];
	else if (c->in_quote == c->str[c->i])
		c->in_quote = 0;
	c->result[c->j++] = c->str[c->i++];
}

static int	should_expand(t_exp_ctx *c)
{
	return (c->str[c->i] == '$' && c->in_quote != '\'');
}

char	*expand_variables(char *str, t_env *env, int exit_status)
{
	t_exp_ctx	c;

	if (!init_ctx(&c, str, env, exit_status))
		return (NULL);
	while (str[c.i])
	{
		if (is_quote(&c))
			handle_quote(&c);
		else if (should_expand(&c))
			process_dollar(&c);
		else
			c.result[c.j++] = c.str[c.i++];
	}
	c.result[c.j] = '\0';
	return (c.result);
}
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

void	expander(t_pipeline *pipeline, t_env *env)
{
	while (pipeline)
	{
		expand_pipeline_cmds(pipeline->cmds, env, g_shell.exit_status);
		pipeline = pipeline->next;
	}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:23:36 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* Helper: check if a character is a quote (for quote removal) */
static int	is_quote(char ch)
{
	return (ch == '\'' || ch == '"');
}

/* Helper: handle quote removal logic */
static int	handle_quote(t_quote_ctx *c, char *s)
{
	if (!c->quote && is_quote(s[c->i]))
	{
		c->quote = s[c->i++];
		return (1);
	}
	if (c->quote && s[c->i] == c->quote)
	{
		c->quote = 0;
		c->i++;
		return (1);
	}
	return (0);
}

static int	handle_bs_outside(t_quote_ctx *c, char *s)
{
	if (c->quote || s[c->i] != '\\')
		return (0);
	if (s[c->i + 1] == '\n')
	{
		c->i += 2;                 /* remove backslash-newline */
		return (1);
	}
	if (s[c->i + 1] == '\0')
	{
		c->res[c->j++] = '\\';     /* PRESERVE trailing '\' */
		c->i += 1;
		return (1);
	}
	c->res[c->j++] = s[c->i + 1];  /* unescape next char */
	c->i += 2;
	return (1);
}

static int	handle_bs_in_dq(t_quote_ctx *c, char *s)
{
	char	nx;

	if (c->quote != '"' || s[c->i] != '\\')
		return (0);
	nx = s[c->i + 1];
	if (nx == '\\' || nx == '"' || nx == '`' || nx == '$')
	{
		c->res[c->j++] = nx;
		c->i += 2;
		return (1);
	}
	if (nx == '\n')
	{
		c->i += 2;
		return (1);
	}
	return (0);
}

char	*remove_quotes(char *str)
{
	t_quote_ctx	c;
	size_t		len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	c.res = malloc(len + 1);
	if (!c.res)
		return (NULL);
	c.i = 0;
	c.j = 0;
	c.quote = 0;
	while (str[c.i])
	{
		if (handle_quote(&c, str))
			continue ;
		if (handle_bs_outside(&c, str))
			continue ;
		if (handle_bs_in_dq(&c, str))
			continue ;
		c.res[c.j++] = str[c.i++];
	}
	c.res[c.j] = '\0';
	return (c.res);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:23:36 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	process_dollar(t_exp_ctx *c)
{
	if (c->i > 0 && c->str[c->i - 1] == '\\' && c->in_quote != '\'')
	{
		c->result[c->j++] = c->str[c->i++];
		return ;
	}
	if (c->in_quote == '\'')
	{
		c->result[c->j++] = c->str[c->i++];
		return ;
	}
	c->i++;
	if (c->str[c->i] == '?')
	{
		expand_exit_status(c->result, &c->j, c->exit_status);
		c->i++;
		return ;
	}
	expand_var_name(c);
}

void	expand_arg(char **arg, t_env *env, int exit_status)
{
	char	*expanded;
	char	*unquoted;

	expanded = expand_variables(*arg, env, exit_status);
	unquoted = remove_quotes(expanded);
	free(*arg);
	free(expanded);
	*arg = unquoted;
}

void	expand_cmd_args(t_cmd *cmd, t_env *env, int exit_status)
{
	int	i;

	i = 0;
	while (cmd->args[i])
	{
		expand_arg(&cmd->args[i], env, exit_status);
		i++;
	}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_vars.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:10:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:23:36 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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

void	expand_exit_status(char *result, int *j, int exit_status)
{
	char	*tmp;
	int		len;

	if (!result || !j)
		return ;
	tmp = ft_itoa(exit_status);
	if (!tmp)
		return ;
	len = ft_strlen(tmp);
	ft_strcpy(&result[*j], tmp);
	*j += len;
	free(tmp);
}

/* reads name at c->str[c->i], appends value (or nothing) into c->result,
   and advances c->i past the name, updates c->j accordingly */
void	expand_var_name(t_exp_ctx *ctx)
{
	int		start;
	char	*key;
	char	*val;

	start = ctx->i;
	while (ft_isalnum((unsigned char)ctx->str[ctx->i])
		|| ctx->str[ctx->i] == '_')
		ctx->i++;
	if (ctx->i == start)
	{
		ctx->result[ctx->j++] = '$';
		return ;
	}
	key = ft_substr(ctx->str, start, ctx->i - start);
	val = get_env_value(ctx->env, key);
	if (val)
		while (*val)
			ctx->result[ctx->j++] = *val++;
	free(key);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_history.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 14:33:12 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	builtin_history(char **args)
{
	HIST_ENTRY	**list;
	int			i;

	(void)args;
	list = history_list();
	if (!list)
		return (0);
	i = 0;
	while (list[i])
	{
		ft_putnbr_fd(i + 1, 1);
		ft_putstr_fd("  ", 1);
		ft_putendl_fd(list[i]->line, 1);
		i++;
	}
	return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 14:41:56 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static char	*g_hist = NULL;

static int	is_blank(const char *s)
{
	int	i;

	if (!s)
		return (1);
	i = 0;
	while (s[i] == ' ' || s[i] == '\t')
		i++;
	return (s[i] == '\0');
}

static char	*history_path_from_env(t_env *env)
{
	char	*home;
	char	*p;
	char	*tmp;

	home = get_env_value(env, "HOME");
	if (!home)
		return (ft_strdup(".minishell_history"));
	tmp = ft_strjoin(home, "/");
	if (!tmp)
		return (NULL);
	p = ft_strjoin(tmp, ".minishell_history");
	free(tmp);
	return (p);
}

int	history_init(t_env *env)
{
	g_hist = history_path_from_env(env);
	if (!g_hist)
		return (0);
	read_history(g_hist);
	stifle_history(1000);
	return (1);
}

void	history_add_line(const char *line)
{
	HIST_ENTRY	*last;

	if (is_blank(line))
		return ;
	last = NULL;
	if (history_length > 0)
		last = history_get(history_length);
	if (!last || ft_strcmp(last->line, line) != 0)
		add_history((char *)line);
}

void	history_save(void)
{
	if (!g_hist)
		return ;
	write_history(g_hist);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 14:32:20 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Handle SIGINT (Ctrl-C)
** Displays new prompt without terminating shell
*/
void	handle_sigint(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	g_shell.exit_status = 130;
	g_shell.sigint_during_read = 1;
}

/*
** Handle SIGQUIT (Ctrl-\)
** Does nothing in interactive mode
*/
void	handle_sigquit(int sig)
{
	(void)sig;
}

/*
** This function:
** ^C is printed by the terminal itself (ECHOCTL), not by your handle_sigint.
	To make minishell behave like bash (no ^C shown), you must disable ECHOCTL 
	on the terminal.
Reads current terminal settings.
Turns off the ECHOCTL flag so the kernel stops printing ^C.
Applies the new settings.
No ternary operator is used.
Press Ctrl-C at the prompt.
You should now see only a new line and a fresh
 minishell> prompt, with no ^C, just like bash.
*/
void	init_terminal(void)
{
	struct termios	t;

	if (tcgetattr(STDIN_FILENO, &t) == -1)
		return ;
	if (t.c_lflag & ECHOCTL)
	{
		t.c_lflag &= ~ECHOCTL;
		tcsetattr(STDIN_FILENO, TCSANOW, &t);
	}
}

/*
** Setup signal handlers for the shell
** SIGINT displays new prompt, SIGQUIT is ignored
*/
void	setup_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/06 15:06:08 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Free array of strings
** Frees each string and the array itself
*/
void	free_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

/*
** Join two strings and free the first one
** Useful for chaining multiple joins
*/
char	*ft_strjoin_free(char *s1, char const *s2)
{
	char	*res;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
	{
		if (!s2)
			return (NULL);
		return (ft_strdup(s2));
	}
	if (!s2)
	{
		res = ft_strdup(s1);
		free(s1);
		return (res);
	}
	res = ft_strjoin(s1, s2);
	free(s1);
	return (res);
}

/*
** Free all environment variables
** Frees keys, values, and nodes
*/
void	free_env(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env;
		env = env->next;
		free(tmp->key);
		free(tmp->value);
		free(tmp);
	}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 15:04:45 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_shell	g_shell;

static int	is_all_space(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		if (!ft_isspace((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	process_line(char *line, t_shell *shell)
{
	t_token		*tokens;
	t_pipeline	*pipeline;

	if (!line || !*line)
		return ;
	if (has_unclosed_quotes(line))
	{
		ft_putendl_fd("minishell: syntax error: unclosed quotes", 2);
		shell->exit_status = 2;
		return ;
	}
	tokens = lexer(line);
	if (!tokens)
		return ;
	if (!validate_syntax(tokens, &g_shell))
	{
		free_tokens(tokens);
		return ;
	}
	pipeline = parser(tokens);
	free_tokens(tokens);
	if (!pipeline)
		return ;
	expander(pipeline, shell->env);
	executor(pipeline, shell);
	free_pipeline(pipeline);
}

void	shell_loop(t_shell *shell)
{
	char	*line;

	while (!shell->should_exit)
	{
		line = read_logical_line();
		if (!line)
		{
			/* Ctrl-C: exit_status set to 130 → just show new prompt */
			if (shell->exit_status == 130)
				continue;
			/* Real EOF (Ctrl-D): exit minishell like bash */
			ft_putendl_fd("exit", 1);
			break ;
		}
		if (*line && !is_all_space(line))
		{
			history_add_line(line);
			process_line(line, shell);
		}
		free(line);
	}
	history_save();
}

/* main.c */
int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	write(1, "\033[2J\033[H", 7);
	print_logo();
	init_shell(&g_shell, envp);
	setup_signals();
	init_terminal();
	shell_loop(&g_shell);
	rl_clear_history();
	free_env(g_shell.env);
	return (g_shell.exit_status);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 14:28:41 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <signal.h>
# include <termios.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"

/* Error messages as constants */
# define ERR_NEWLINE "minishell: syntax error near unexpected token `newline'"
# define ERR_PIPE "minishell: syntax error near unexpected token `|'"
# define ERR_AND "minishell: syntax error near unexpected token `&&'"
# define ERR_OR "minishell: syntax error near unexpected token `||'"
# define ERR_REDIR_IN "minishell: syntax error near unexpected token `<'"
# define ERR_REDIR_OUT "minishell: syntax error near unexpected token `>'"
# define ERR_REDIR_APPEND "minishell: syntax error near unexpected token `>>'"
# define ERR_REDIR_HEREDOC "minishell: syntax error near unexpected token `<<'"
# define ERR_TRIPLE_LT "minishell: syntax error near unexpected token `<<<'"
# define ERR_MULTIPLE_REDIR_IN \
	"minishell: syntax error near unexpected token `<<<'"
# define ERR_MULTIPLE_REDIR_OUT \
	"minishell: syntax error near unexpected token `>>>'"
# define ERR_CONSECUTIVE_REDIR \
	"minishell: syntax error near unexpected token `>'"
# define ERR_SEMI "minishell: syntax error near unexpected token `;'"

/* ===================== TOKENS ===================== */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_HEREDOC,
	TOKEN_SEMI,
	TOKEN_EOF
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

/* ===================== PARSED NODES ===================== */
typedef struct s_redir
{
	t_token_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd	*next;
	int				expanded; /* 0/1 guard to avoid double expansion */
}	t_cmd;

typedef struct s_pipeline
{
	t_cmd				*cmds;
	t_token_type		logic_op;
	struct s_pipeline	*next;
}	t_pipeline;

/* ===================== ENV ===================== */
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

/* ===================== SHELL STATE ===================== */
typedef struct s_shell
{
	t_env	*env;
	int		exit_status;
	int		should_exit;
	int		sigint_during_read;
}	t_shell;

extern t_shell	g_shell;

/* ===================== EXECUTION AUX TYPES ===================== */
typedef struct s_child_io
{
	int	prev_rd;
	int	pipe_rd;
	int	pipe_wr;
	int	has_next;
}	t_child_io;

typedef struct s_pipe_ctx
{
	t_shell	*shell;
	pid_t	*pids;
	int		*prev_rd;
}	t_pipe_ctx;

/* ===================== EXPANSION CTX ===================== */
typedef struct s_exp_ctx
{
	char	*str;
	char	*result;
	int		i;
	int		j;
	char	in_quote;
	t_env	*env;
	int		exit_status;
}	t_exp_ctx;

typedef struct s_quote_ctx
{
	int		i;
	int		j;
	char	quote;
	char	*str;
	char	*res;
}	t_quote_ctx;

/* ===================== LEXER ===================== */
/* Tokenize input string into a linked list of tokens */
t_token		*lexer(char *input);
t_token		*create_token(t_token_type type, char *value);
void		add_token(t_token **tokens, t_token *new_token);
void		free_tokens(t_token *tokens);
int			has_unclosed_quotes(char *str);
t_token		*get_operator_token(char **input);
int			is_whitespace(char c);
int			is_operator(char c);
int			extract_word(char *input, char **word);
t_token		*try_or_pipe(char **input);
t_token		*try_and(char **input);
t_token		*try_inredir(char **input);
t_token		*try_outredir(char **input);
t_token		*try_semicolon(char **input);

/* ===================== PARSER ===================== */
t_pipeline	*parser(t_token *tokens);
t_cmd		*parse_command(t_token **tokens);
t_redir		*parse_redirections(t_token **tokens);
void		append_redir(t_redir **head, t_redir *new_redir);
void		free_pipeline(t_pipeline *pipeline);
t_redir		*create_redir(t_token_type type, char *file);
int			validate_syntax(t_token *tokens, t_shell *shell);
void		print_syntax_error(t_token *token);
int			is_valid_word(t_token *token);
int			is_control_operator(t_token *token);
int			is_redirection(t_token *token);
int			is_gt(t_token *t);
int			is_lt(t_token *t);
int			tok_op_len(t_token *t);
void		print_run_error(t_token *t);
void		print_unexpected(char *s);

/* ===================== EXPANDER ===================== */
char		*get_env_value(t_env *env, char *key);
char		*expand_variables(char *str, t_env *env, int exit_status);
void		expander(t_pipeline *pipeline, t_env *env);
void		expand_exit_status(char *result, int *j, int exit_status);
void		expand_var_name(t_exp_ctx *ctx);
void		process_dollar(t_exp_ctx *c);
void		expand_arg(char **arg, t_env *env, int exit_status);
void		expand_cmd_args(t_cmd *cmd, t_env *env, int exit_status);
void		expand_redirections(t_redir *redir, t_env *env, int exit_status);
void		expand_pipeline_cmds(t_cmd *cmds, t_env *env, int exit_status);
char		*remove_quotes(char *str);

/* ===================== EXECUTOR ===================== */
void		executor(t_pipeline *pipeline, t_shell *shell);
int			init_pipeline(int cmd_count, pid_t **pids);
int			create_pipe_if_needed(t_cmd *current, int pipefd[2]);
int			count_commands(t_cmd *cmds);
int			wait_for_children(pid_t *pids, int count);
int			execute_pipeline_loop(t_cmd *cmds, t_shell *shell,
				pid_t *pids, int cmd_count);
int			execute_single_builtin_parent(t_cmd *cmd, t_shell *shell);
int			execute_pipeline(t_cmd *cmds, t_shell *shell);
void		execute_commands(t_cmd *cmd, t_shell *shell);
void		execute_command(char **argv, t_shell *shell, char **envp);
int			setup_redirections(t_redir *redirs);
int			setup_child_fds(int pipefd[2], int prev_read_fd, int has_next);
char		*find_executable(char *cmd, t_env *env);
void		print_error(const char *function, const char *message);
void		safe_close(int fd);
void		cleanup_pipe(int pipefd[2]);
pid_t		create_child_process(t_cmd *cmd, t_shell *shell, t_child_io *io);
int			update_prev_fd(int prev_read_fd, int pipefd[2], int has_next);
int			execute_one_command(t_cmd *cmd, int index, t_pipe_ctx *ctx);
void		cmd_not_found(char *name);

/* ===================== BUILTINS ===================== */
int			is_builtin(char *cmd);
int			execute_builtin(t_cmd *cmd, t_shell *shell);
int			builtin_echo(char **args);
int			builtin_cd(char **args, t_env **env);
char		*dup_cwd(void);
char		*resolve_target(char **args, t_env *env, int *print_after);
int			builtin_pwd(void);
int			builtin_export(char **args, t_env **env);
int			builtin_unset(char **args, t_env **env);
int			builtin_env(t_env *env);
int			builtin_exit(char **args, t_shell *shell);

/* ===================== ENV MANAGEMENT ===================== */
t_env		*init_env(char **envp);
void		increment_shlvl(t_env **env);
t_env		*create_env_node(char *key, char *value);
void		add_env_node(t_env **env, t_env *new_node);
void		remove_env_node(t_env **env, char *key);
char		**env_to_array(t_env *env);
void		free_env(t_env *env);
int			append_env(char ***arr, size_t *n, const char *k, const char *v);
void		parse_env_string(char *env_str, char **key, char **value);

/* ===================== SIGNALS ===================== */
void		setup_signals(void);
void		handle_sigint(int sig);
void		handle_sigquit(int sig);
void        init_terminal(void);

/* ===================== UTILS ===================== */
void		free_array(char **arr);
char		*ft_strjoin_free(char *s1, char const *s2);
void		print_logo(void);
char		*read_joined_line(t_shell *shell);

/* ===================== CORE ===================== */
void		init_shell(t_shell *shell, char **envp);
void		shell_loop(t_shell *shell);
char		*read_logical_line(void);

/* history */
int			history_init(t_env *env);
void		history_add_line(const char *line);
void		history_save(void);
int			builtin_history(char **args);

#endif
