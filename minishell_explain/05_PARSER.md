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

**Real-life analogy**: The parser is like a **recipe organizer**:

The lexer gave us ingredients (tokens):
```
["flour"] ["sugar"] ["mix"] ["eggs"] ["bake"]
```

The parser organizes them into a recipe structure:
```
Recipe:
├── Step 1: Combine ["flour", "sugar"]
│           Action: "mix"
└── Step 2: Add ["eggs"]
            Action: "bake"
```

In shell terms:
```
Tokens:  [ls] [-la] [|] [grep] [.c]

Parser creates:
Pipeline:
├── Command 1: {args: ["ls", "-la"]}
│              └── next →
└── Command 2: {args: ["grep", ".c"]}
```

---

## Data Structures

### t_cmd - Single Command

```c
typedef struct s_cmd
{
    char        **args;     // ["grep", "foo", NULL]
    t_redir     *redirs;    // Linked list of redirections
    struct s_cmd *next;     // Next command in pipeline
    int         expanded;   // Already expanded flag
}   t_cmd;
```

**Visual**:
```
For command: grep foo

┌─────────────────────────────────────────┐
│              t_cmd                       │
├─────────────────────────────────────────┤
│ args ──────► ["grep", "foo", NULL]      │
│              ┌────┬─────┬──────┐        │
│              │ [0]│ [1] │ [2]  │        │
│              │grep│ foo │ NULL │        │
│              └────┴─────┴──────┘        │
│                                          │
│ redirs ────► NULL (no redirections)     │
│                                          │
│ next ──────► NULL (no more commands)    │
│                                          │
│ expanded ──► 0 (not yet expanded)       │
└─────────────────────────────────────────┘
```

### t_redir - Redirection

```c
typedef struct s_redir
{
    t_token_type    type;   // REDIR_IN, REDIR_OUT, etc.
    char            *file;  // Filename
    struct s_redir  *next;  // Next redirection
}   t_redir;
```

**Visual for: `< input.txt > output.txt`**:
```
┌──────────────────────┐    ┌──────────────────────┐
│      t_redir #1      │    │      t_redir #2      │
├──────────────────────┤    ├──────────────────────┤
│ type: REDIR_IN (<)   │───►│ type: REDIR_OUT (>)  │───► NULL
│ file: "input.txt"    │    │ file: "output.txt"   │
└──────────────────────┘    └──────────────────────┘
```

### t_pipeline - Pipeline Container

```c
typedef struct s_pipeline
{
    t_cmd           *cmds;      // Linked list of commands
    t_token_type    logic_op;   // (unused in mandatory)
    struct s_pipeline *next;    // (unused in mandatory)
}   t_pipeline;
```

---

## Parsing Examples - Step by Step

### Example 1: Simple Command

**Input**: `echo hello world`

**Tokens from lexer**:
```
[WORD:"echo"] → [WORD:"hello"] → [WORD:"world"] → NULL
```

**Parser process**:
```
┌──────────────────────────────────────────────────────┐
│ Step 1: create_pipeline()                            │
│         Allocate new t_pipeline                      │
│                                                       │
│ Step 2: parse_pipe_sequence(&tokens)                 │
│         └── parse_command(&tokens)                   │
│             ├── count_args() = 3                     │
│             ├── Allocate args[4] (3 + NULL)          │
│             ├── args[0] = "echo"                     │
│             ├── args[1] = "hello"                    │
│             ├── args[2] = "world"                    │
│             └── args[3] = NULL                       │
│                                                       │
│ Step 3: No PIPE token, done                          │
└──────────────────────────────────────────────────────┘

Result:
┌─────────────────────────────────────┐
│           t_pipeline                │
├─────────────────────────────────────┤
│ cmds ──► t_cmd                      │
│          ├── args: ["echo",         │
│          │         "hello",         │
│          │         "world", NULL]   │
│          ├── redirs: NULL           │
│          └── next: NULL             │
└─────────────────────────────────────┘
```

### Example 2: Pipeline

**Input**: `ls -la | grep .c | wc -l`

**Tokens from lexer**:
```
[WORD:"ls"] → [WORD:"-la"] → [PIPE:"|"] →
[WORD:"grep"] → [WORD:".c"] → [PIPE:"|"] →
[WORD:"wc"] → [WORD:"-l"] → NULL
```

**Parser process**:
```
┌──────────────────────────────────────────────────────┐
│ parse_pipe_sequence(&tokens):                         │
│                                                       │
│ Iteration 1:                                          │
│   parse_command() → {args: ["ls", "-la"]}            │
│   Next token is PIPE → skip it, continue             │
│                                                       │
│ Iteration 2:                                          │
│   parse_command() → {args: ["grep", ".c"]}           │
│   Next token is PIPE → skip it, continue             │
│                                                       │
│ Iteration 3:                                          │
│   parse_command() → {args: ["wc", "-l"]}             │
│   Next token is NULL → done                          │
│                                                       │
│ Link them: cmd1 → cmd2 → cmd3                        │
└──────────────────────────────────────────────────────┘

Result:
┌─────────────────────────────────────────────────────────────┐
│                        t_pipeline                            │
├─────────────────────────────────────────────────────────────┤
│ cmds ──►┌───────────────┐   ┌───────────────┐   ┌──────────┐│
│         │ t_cmd #1      │──►│ t_cmd #2      │──►│ t_cmd #3 ││
│         │ args:         │   │ args:         │   │ args:    ││
│         │ ["ls","-la"]  │   │ ["grep",".c"] │   │ ["wc",   ││
│         │               │   │               │   │  "-l"]   ││
│         └───────────────┘   └───────────────┘   └──────────┘│
└─────────────────────────────────────────────────────────────┘
```

### Example 3: Command with Redirections

**Input**: `cat < input.txt > output.txt`

**Tokens from lexer**:
```
[WORD:"cat"] → [REDIR_IN:"<"] → [WORD:"input.txt"] →
[REDIR_OUT:">"] → [WORD:"output.txt"] → NULL
```

**Parser process**:
```
┌──────────────────────────────────────────────────────┐
│ parse_command(&tokens):                               │
│                                                       │
│ Step 1: count_args()                                  │
│         - See WORD "cat" → count = 1                 │
│         - See REDIR_IN → skip 2 tokens (< + file)    │
│         - See REDIR_OUT → skip 2 tokens (> + file)   │
│         - Result: 1 argument                          │
│                                                       │
│ Step 2: Allocate args[2] (1 + NULL)                  │
│                                                       │
│ Step 3: consume_redirs() - handle < input.txt        │
│         - Create t_redir {REDIR_IN, "input.txt"}     │
│         - Add to cmd->redirs                          │
│                                                       │
│ Step 4: Process WORD "cat"                           │
│         - args[0] = "cat"                             │
│                                                       │
│ Step 5: consume_redirs() - handle > output.txt       │
│         - Create t_redir {REDIR_OUT, "output.txt"}   │
│         - Add to cmd->redirs                          │
│                                                       │
│ Step 6: args[1] = NULL                               │
└──────────────────────────────────────────────────────┘

Result:
┌─────────────────────────────────────────────────────────────┐
│                         t_cmd                                │
├─────────────────────────────────────────────────────────────┤
│ args ──► ["cat", NULL]                                      │
│                                                              │
│ redirs ──► ┌──────────────────┐   ┌──────────────────┐      │
│            │ type: REDIR_IN   │──►│ type: REDIR_OUT  │──►NULL│
│            │ file: "input.txt"│   │ file: "output.txt"│      │
│            └──────────────────┘   └──────────────────┘      │
│                                                              │
│ next ──► NULL                                               │
└─────────────────────────────────────────────────────────────┘
```

---

## Syntax Validation

### Why Validate Before Parsing?

The parser assumes valid input. Invalid syntax would cause crashes or undefined behavior.

### validate_syntax() - The Checks

```c
int validate_syntax(t_token *tokens, t_shell *shell)
{
    // Check 1: First token can't be a pipe
    if (!validate_first_token(tokens))
        return (0);

    // Check 2: Each pair of adjacent tokens must be valid
    while (current && current->next)
    {
        if (!validate_token_pair(current, current->next))
            return (0);
        current = current->next;
    }

    // Check 3: Last token can't be a redirection or pipe
    if (!validate_last_token(last))
        return (0);

    return (1);  // All good!
}
```

### Syntax Error Examples

**Error 1: Pipe at start**
```
Input: | ls
Error: minishell: syntax error near unexpected token `|'
Exit code: 2

Why? Nothing to pipe FROM
```

**Error 2: Pipe at end**
```
Input: ls |
Error: minishell: syntax error near unexpected token `newline'
Exit code: 2

Why? Nothing to pipe TO
```

**Error 3: Double pipe**
```
Input: ls || grep
Error: minishell: syntax error near unexpected token `|'
Exit code: 2

Why? || is not supported in mandatory
```

**Error 4: Redirection without file**
```
Input: cat >
Error: minishell: syntax error near unexpected token `newline'
Exit code: 2

Why? > needs a filename
```

**Error 5: Consecutive redirections without file**
```
Input: cat > < file
Error: minishell: syntax error near unexpected token `<'
Exit code: 2

Why? > needs a filename, got < instead
```

---

## Key Parser Functions

### parse_command() - Main Command Parser

```c
t_cmd *parse_command(t_token **tokens)
{
    t_cmd   *cmd;
    int     i;
    int     arg_count;

    // Count how many words (to allocate args array)
    arg_count = count_args(*tokens);
    cmd = new_cmd(arg_count);

    i = 0;
    consume_redirs(tokens, cmd);  // Handle leading redirections

    // Process each WORD token
    while (*tokens && (*tokens)->type == TOKEN_WORD)
    {
        cmd->args[i] = ft_strdup((*tokens)->value);
        i++;
        *tokens = (*tokens)->next;
        consume_redirs(tokens, cmd);  // Handle redirections between words
    }

    cmd->args[i] = NULL;
    return (cmd);
}
```

### Why consume_redirs() is called multiple times?

Redirections can appear ANYWHERE in a command:

```
< input cat        ← Before command
cat < input        ← After command name
cat file1 > out file2  ← Between arguments
```

All these are valid and equivalent to:
```
cat file1 file2 < input > out
```

### count_args() - Counting Words

```c
static int count_args(t_token *tokens)
{
    int count = 0;

    while (tokens && is_word_or_redir(tokens))
    {
        if (tokens->type == TOKEN_WORD)
        {
            count++;
            tokens = tokens->next;
        }
        else  // It's a redirection
        {
            // Skip redir token AND its filename
            tokens = tokens->next;  // Skip <, >, <<, >>
            if (tokens)
                tokens = tokens->next;  // Skip filename
        }
    }
    return (count);
}
```

**Example: `cat < input.txt -n > output.txt`**
```
Token: cat         → count = 1
Token: <           → skip
Token: input.txt   → skip (filename)
Token: -n          → count = 2
Token: >           → skip
Token: output.txt  → skip (filename)

Result: 2 arguments ("cat" and "-n")
```

---

## Parser Flow Diagram

```
         Token List from Lexer
                  │
                  ▼
         ┌────────────────────┐
         │  validate_syntax() │
         └─────────┬──────────┘
                   │
          Valid? ──┼── Invalid?
                   │        │
                   │        ▼
                   │   Print error
                   │   exit_status = 2
                   │   return NULL
                   │
                   ▼
         ┌────────────────────┐
         │     parser()       │
         └─────────┬──────────┘
                   │
                   ▼
    ┌──────────────────────────────────┐
    │     parse_pipe_sequence()        │
    │                                   │
    │   ┌─────────────────────────┐    │
    │   │   parse_command() #1    │    │
    │   │                         │    │
    │   │ ┌─────────────────────┐ │    │
    │   │ │ count_args()        │ │    │
    │   │ └─────────────────────┘ │    │
    │   │          ↓              │    │
    │   │ ┌─────────────────────┐ │    │
    │   │ │ new_cmd(count)      │ │    │
    │   │ └─────────────────────┘ │    │
    │   │          ↓              │    │
    │   │ ┌─────────────────────┐ │    │
    │   │ │ consume_redirs()    │←┼────┼─── Repeated for each
    │   │ └─────────────────────┘ │    │    word and between
    │   │          ↓              │    │    arguments
    │   │ ┌─────────────────────┐ │    │
    │   │ │ Copy word to args[] │ │    │
    │   │ └─────────────────────┘ │    │
    │   └─────────────────────────┘    │
    │              │                    │
    │         Is next PIPE?             │
    │              │                    │
    │         Yes──┼──No                │
    │              │    │               │
    │              ▼    └──► Done       │
    │   ┌─────────────────────────┐    │
    │   │   parse_command() #2    │    │
    │   └─────────────────────────┘    │
    │              │                    │
    │         (repeat)                  │
    └──────────────────────────────────┘
                   │
                   ▼
            t_pipeline with
            linked t_cmd list
```

---

## Complete Parsing Example

**Input**: `cat file.txt | grep error | head -5 > result.txt`

**Token list**:
```
[cat] → [file.txt] → [|] → [grep] → [error] → [|] →
[head] → [-5] → [>] → [result.txt] → NULL
```

**Step-by-step parsing**:

```
┌─────────────────────────────────────────────────────────────┐
│ parse_pipe_sequence():                                       │
│                                                              │
│ COMMAND 1:                                                   │
│   Tokens: [cat] [file.txt] [|]...                           │
│   count_args() = 2                                           │
│   consume_redirs() → none                                    │
│   args[0] = "cat"                                            │
│   consume_redirs() → none                                    │
│   args[1] = "file.txt"                                       │
│   consume_redirs() → none                                    │
│   args[2] = NULL                                             │
│   Stop at [|], skip it                                       │
│                                                              │
│ COMMAND 2:                                                   │
│   Tokens: [grep] [error] [|]...                             │
│   count_args() = 2                                           │
│   args[0] = "grep"                                           │
│   args[1] = "error"                                          │
│   args[2] = NULL                                             │
│   Stop at [|], skip it                                       │
│                                                              │
│ COMMAND 3:                                                   │
│   Tokens: [head] [-5] [>] [result.txt]                      │
│   count_args():                                              │
│     [head] → count = 1                                       │
│     [-5] → count = 2                                         │
│     [>] → skip                                               │
│     [result.txt] → skip (filename for >)                    │
│   count = 2                                                  │
│   args[0] = "head"                                           │
│   args[1] = "-5"                                             │
│   consume_redirs() at end:                                   │
│     Create t_redir{REDIR_OUT, "result.txt"}                 │
│   args[2] = NULL                                             │
│   No more tokens, done                                       │
└─────────────────────────────────────────────────────────────┘
```

**Result structure**:
```
t_pipeline
└── cmds ──► t_cmd #1           t_cmd #2           t_cmd #3
             │                   │                   │
             ├─ args:           ├─ args:           ├─ args:
             │  ["cat",         │  ["grep",        │  ["head",
             │   "file.txt"]    │   "error"]       │   "-5"]
             │                   │                   │
             ├─ redirs: NULL    ├─ redirs: NULL    ├─ redirs:
             │                   │                   │  └─► REDIR_OUT
             │                   │                   │      "result.txt"
             │                   │                   │
             └─ next ───────────┴─ next ───────────┴─ next: NULL
```

---

## Common Questions

### Q: Why is exit code 2 for syntax errors?

**A**: It's a shell convention! Exit codes have meanings:
- 0: Success
- 1: General error
- 2: Misuse of shell command (syntax error)
- 126: Command cannot execute (permission)
- 127: Command not found

### Q: Can redirections appear before the command?

**A**: Yes! These are equivalent:
```bash
< input cat    # Input first
cat < input    # Command first
```

Both mean: "read from input, run cat"

### Q: What about spaces around redirections?

**A**: Optional! These are all the same:
```bash
cat>file      # No spaces
cat > file    # Spaces
cat> file     # Mixed
```

The lexer handles this by recognizing `>` as an operator that breaks words.

### Q: Why separate syntax check and parsing?

**A**:
1. **Cleaner error messages**: Syntax check knows exactly what's wrong
2. **Simpler parser**: Parser can assume valid input
3. **Early exit**: Don't waste time parsing invalid commands
