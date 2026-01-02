# Parser

## Files
- `src/parser/parser.c`
- `src/parser/parser_pipeline.c`
- `src/parser/parser_utils.c`
- `src/parser/parser_syntax_check.c`
- `src/parser/parser_syntax_utils.c`
- `src/parser/parser_syntax_print.c`
- `src/parser/parser_check_token.c`
- `src/parser/parser_error.c`

---

## What is the Parser?

The parser takes the token list from the lexer and builds a structured representation (Abstract Syntax Tree / AST) that the executor can understand.

```
Tokens:  [WORD:"ls"] → [PIPE:"|"] → [WORD:"grep"] → [WORD:"foo"]

Parser Output:
    t_pipeline
    └── cmds:
        ├── t_cmd { args: ["ls"], redirs: NULL }
        │   └── next:
        └── t_cmd { args: ["grep", "foo"], redirs: NULL }
```

---

## Data Structures

### t_cmd (Single Command)
```c
typedef struct s_cmd
{
    char        **args;     // ["grep", "foo", NULL]
    t_redir     *redirs;    // Linked list of redirections
    struct s_cmd *next;     // Next command in pipeline
    int         expanded;   // Already expanded flag
}   t_cmd;
```

### t_redir (Redirection)
```c
typedef struct s_redir
{
    t_token_type    type;   // REDIR_IN, REDIR_OUT, etc.
    char            *file;  // Filename
    struct s_redir  *next;  // Next redirection
}   t_redir;
```

### t_pipeline (Pipeline)
```c
typedef struct s_pipeline
{
    t_cmd           *cmds;      // Commands in this pipeline
    t_token_type    logic_op;   // (unused in mandatory)
    struct s_pipeline *next;    // (unused in mandatory)
}   t_pipeline;
```

---

## parser_pipeline.c

### parser()
```c
t_pipeline *parser(t_token *tokens)
{
    t_pipeline *head;
    t_pipeline *node;

    head = NULL;
    while (tokens)
    {
        if (!tokens || tokens->type == TOKEN_EOF)
            break;
        node = create_pipeline();
        node->cmds = parse_pipe_sequence(&tokens);
        set_logic_and_advance(node, &tokens);
        append_pipeline(&head, node);
        if (!tokens || tokens->type == TOKEN_EOF)
            break;
    }
    return (head);
}
```

**Purpose**: Main parser entry point - builds pipeline list from tokens.

---

### create_pipeline()
```c
static t_pipeline *create_pipeline(void)
{
    t_pipeline *pipeline;

    pipeline = malloc(sizeof(t_pipeline));
    if (!pipeline)
        return (NULL);
    pipeline->cmds = NULL;
    pipeline->logic_op = TOKEN_EOF;
    pipeline->next = NULL;
    return (pipeline);
}
```

**Purpose**: Allocate a new pipeline node.

---

### parse_pipe_sequence()
```c
t_cmd *parse_pipe_sequence(t_token **tokens)
{
    t_cmd *cmds;
    t_cmd *new_cmd;
    t_cmd *current;

    cmds = NULL;
    while (*tokens)
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
            *tokens = (*tokens)->next;  // Skip pipe token
        else
            break;
    }
    return (cmds);
}
```

**Purpose**: Parse a sequence of commands separated by pipes.

**Logic**:
1. Parse first command
2. If next token is PIPE, skip it and parse next command
3. Link commands together
4. Stop when no more pipes

---

## parser.c

### parse_command()
```c
t_cmd *parse_command(t_token **tokens)
{
    t_cmd   *cmd;
    int     i;
    int     arg_count;

    if (!tokens || !*tokens)
        return (NULL);
    arg_count = count_args(*tokens);
    cmd = new_cmd(arg_count);
    if (!cmd)
        return (NULL);
    i = 0;
    consume_redirs(tokens, cmd);          // Handle leading redirections
    while (*tokens && (*tokens)->type == TOKEN_WORD)
    {
        cmd->args[i] = ft_strdup((*tokens)->value);
        if (!cmd->args[i])
            break;
        i++;
        *tokens = (*tokens)->next;
        consume_redirs(tokens, cmd);      // Handle redirections between words
    }
    cmd->args[i] = NULL;
    return (cmd);
}
```

**Purpose**: Parse a single command (words + redirections).

**Why consume_redirs before and during**:
Redirections can appear anywhere in a command:
- `< input.txt cat` (before command)
- `cat < input.txt` (after command)
- `cat file1 > out.txt file2` (between arguments)

---

### count_args()
```c
static int count_args(t_token *tokens)
{
    int count;

    count = 0;
    while (tokens && (tokens->type == TOKEN_WORD
            || tokens->type == TOKEN_REDIR_IN
            || tokens->type == TOKEN_REDIR_OUT
            || tokens->type == TOKEN_REDIR_APPEND
            || tokens->type == TOKEN_REDIR_HEREDOC))
    {
        if (tokens->type == TOKEN_WORD)
        {
            count++;
            tokens = tokens->next;
        }
        else if (tokens->next)
            tokens = tokens->next->next;  // Skip redir + filename
        else
            break;
    }
    return (count);
}
```

**Purpose**: Count word tokens (skip redirection pairs).

**Why**: Need to allocate correct size for args array.

---

### consume_redirs()
```c
static void consume_redirs(t_token **tokens, t_cmd *cmd)
{
    t_redir *new_redir;

    while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN
            || (*tokens)->type == TOKEN_REDIR_OUT
            || (*tokens)->type == TOKEN_REDIR_APPEND
            || (*tokens)->type == TOKEN_REDIR_HEREDOC))
    {
        new_redir = parse_single_redirection(tokens);
        if (!new_redir)
            break;
        append_redir(&cmd->redirs, new_redir);
    }
}
```

**Purpose**: Consume all consecutive redirections and add to command.

---

### parse_single_redirection()
```c
static t_redir *parse_single_redirection(t_token **tokens)
{
    t_redir *redir;

    if (!*tokens)
        return (NULL);
    if ((*tokens)->type != TOKEN_REDIR_IN
        && (*tokens)->type != TOKEN_REDIR_OUT
        && (*tokens)->type != TOKEN_REDIR_APPEND
        && (*tokens)->type != TOKEN_REDIR_HEREDOC)
        return (NULL);
    if (!(*tokens)->next || !(*tokens)->next->value)
    {
        ft_putendl_fd("minishell: syntax error near unexpected token `newline'", 2);
        return (NULL);
    }
    redir = create_redir((*tokens)->type, (*tokens)->next->value);
    if (!redir)
        return (NULL);
    *tokens = (*tokens)->next->next;  // Skip redir + filename
    return (redir);
}
```

**Purpose**: Parse one redirection (operator + filename).

---

## parser_syntax_check.c

### validate_syntax()
```c
int validate_syntax(t_token *tokens, t_shell *shell)
{
    t_token *current;
    t_token *last;

    (void)shell;
    if (!tokens)
        return (1);
    if (!validate_first_token(tokens))
        return (0);
    current = tokens;
    last = tokens;
    while (current && current->next)
    {
        if (!validate_token_pair(current, current->next))
            return (0);
        current = current->next;
        last = current;
    }
    if (!validate_last_token(last))
        return (0);
    return (1);
}
```

**Purpose**: Validate token sequence before parsing.

**Checks**:
1. First token cannot be a pipe
2. Adjacent tokens must be valid pairs
3. Last token cannot be a redirection without filename

---

### validate_first_token()
```c
static int validate_first_token(t_token *first)
{
    if (is_separator_token(first))
    {
        print_syntax_error(first);
        return (0);
    }
    return (1);
}
```

**Purpose**: First token cannot be `|`.

**Example errors**:
```
| ls        ← Error: pipe at start
ls | grep   ← OK
```

---

### validate_last_token()
```c
static int validate_last_token(t_token *last)
{
    if (last && (is_separator_token(last) || is_redirection(last)))
    {
        print_syntax_error(NULL);
        return (0);
    }
    return (1);
}
```

**Purpose**: Cannot end with pipe or redirection.

**Example errors**:
```
ls |            ← Error: nothing after pipe
ls >            ← Error: no filename after >
ls > file.txt   ← OK
```

---

## Parser Flow Diagram

```
     Token List
          │
          ▼
    ┌─────────────┐
    │  validate_  │─── Error ──► Exit with code 2
    │   syntax()  │
    └──────┬──────┘
           │ OK
           ▼
    ┌─────────────┐
    │  parser()   │
    └──────┬──────┘
           │
           ▼
    ┌─────────────────────────────────────────┐
    │         parse_pipe_sequence()            │
    │                                          │
    │   ┌─────────────────────────────────┐   │
    │   │    parse_command() [first]      │   │
    │   └───────────────┬─────────────────┘   │
    │                   │                      │
    │        ──── If PIPE token ────           │
    │                   │                      │
    │   ┌─────────────────────────────────┐   │
    │   │    parse_command() [second]     │   │
    │   └───────────────┬─────────────────┘   │
    │                   │                      │
    │               (repeat)                   │
    └─────────────────────────────────────────┘
           │
           ▼
    t_pipeline with linked t_cmd list
```

---

## Parsing Examples

### Simple Command
```
Input:  echo hello world
Tokens: [WORD:"echo"] → [WORD:"hello"] → [WORD:"world"]

Result:
    t_pipeline
    └── cmds: t_cmd
            ├── args: ["echo", "hello", "world", NULL]
            ├── redirs: NULL
            └── next: NULL
```

### Pipeline
```
Input:  ls -la | grep ".c" | wc -l
Tokens: [WORD:"ls"] [WORD:"-la"] [PIPE] [WORD:"grep"] [WORD:".c"] [PIPE] [WORD:"wc"] [WORD:"-l"]

Result:
    t_pipeline
    └── cmds: t_cmd { args: ["ls", "-la"] }
              └── next: t_cmd { args: ["grep", ".c"] }
                        └── next: t_cmd { args: ["wc", "-l"] }
```

### With Redirections
```
Input:  cat < input.txt > output.txt
Tokens: [WORD:"cat"] [REDIR_IN] [WORD:"input.txt"] [REDIR_OUT] [WORD:"output.txt"]

Result:
    t_pipeline
    └── cmds: t_cmd
            ├── args: ["cat", NULL]
            ├── redirs: t_redir { type: REDIR_IN, file: "input.txt" }
            │           └── next: t_redir { type: REDIR_OUT, file: "output.txt" }
            └── next: NULL
```

---

## Key Concepts

### Why Syntax Check Before Parse?
The parser assumes valid input. Catching syntax errors early:
1. Provides clear error messages
2. Prevents undefined behavior in parser
3. Follows bash behavior (error before execution)

### Redirection Order Matters
```
cat file1 > out.txt file2
```
This is valid! It means: `cat file1 file2` with stdout to `out.txt`.

The parser collects redirections separately from args, preserving order for the executor.

### Exit Code 2 for Syntax Errors
Shell convention: syntax errors return exit code 2.
```
$ |
minishell: syntax error near unexpected token `|'
$ echo $?
2
```
