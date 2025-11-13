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
