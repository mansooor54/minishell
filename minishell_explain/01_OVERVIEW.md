# Minishell Project Overview

## What is Minishell?

Minishell is a simplified shell implementation written in C, following 42 School's requirements. It replicates core functionality of bash, allowing users to execute commands, manage environment variables, and use pipes and redirections.

## Project Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                           USER INPUT                                 │
│                    (typed at minishell> prompt)                      │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         1. SHELL LOOP                                │
│                    (reads input with readline)                       │
│                         src/core/                                    │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────┐
│                           2. LEXER                                   │
│              (converts input string into tokens)                     │
│                         src/lexer/                                   │
│                                                                      │
│   "ls -la | grep foo" → [WORD:ls] [WORD:-la] [PIPE:|] [WORD:grep]   │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────┐
│                          3. PARSER                                   │
│         (builds command structure from tokens)                       │
│                        src/parser/                                   │
│                                                                      │
│   tokens → t_pipeline → t_cmd (with args and redirections)          │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         4. EXPANDER                                  │
│     (expands $VARIABLES and removes quotes)                          │
│                       src/expander/                                  │
│                                                                      │
│   "$HOME/file" → "/Users/john/file"                                 │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         5. EXECUTOR                                  │
│      (runs commands with pipes and redirections)                     │
│                       src/executor/                                  │
│                                                                      │
│   - Handles builtins (echo, cd, pwd, export, unset, env, exit)      │
│   - Forks for external commands                                      │
│   - Sets up pipes between commands                                   │
│   - Sets up file redirections                                        │
└─────────────────────────────────────────────────────────────────────┘
```

## Directory Structure

```
minishell/
├── Makefile              # Build configuration
├── include/
│   └── minishell.h       # Central header with all declarations
├── libft/                # Custom libft library
└── src/
    ├── main/             # Entry point and logo
    │   ├── main.c        # Program entry, initialization
    │   └── minishell_logo.c
    │
    ├── core/             # Shell loop and line reading
    │   ├── shell_loop.c       # Main REPL loop
    │   ├── read_logical_line.c # Handle multi-line input
    │   ├── join_continuation.c # Join continuation lines
    │   └── shell_utils.c
    │
    ├── lexer/            # Tokenization
    │   ├── lexer.c            # Main lexer logic
    │   ├── lexer_operator.c   # Operator detection
    │   ├── lexer_operator_type.c # Specific operators
    │   ├── lexer_utils.c      # Helper functions
    │   └── lexer_unclose.c    # Unclosed quote detection
    │
    ├── parser/           # Command parsing
    │   ├── parser.c           # Build t_cmd from tokens
    │   ├── parser_pipeline.c  # Build pipeline structure
    │   ├── parser_syntax_check.c # Syntax validation
    │   ├── parser_utils.c     # Parser helpers
    │   └── parser_error.c     # Error messages
    │
    ├── expander/         # Variable expansion
    │   ├── expander_core.c    # Main expansion logic
    │   ├── expander_vars.c    # Variable lookup
    │   ├── expander_quotes.c  # Quote removal
    │   ├── expander_utils.c   # Expansion helpers
    │   └── expander_pipeline.c
    │
    ├── executor/         # Command execution
    │   ├── executor.c         # Main entry point
    │   ├── executor_pipeline.c # Pipeline execution
    │   ├── executor_external.c # External commands
    │   ├── executor_commands.c # Command dispatch
    │   ├── executor_path.c    # PATH resolution
    │   ├── executor_redirections.c # File redirections
    │   └── executor_redir_heredoc.c # Heredoc handling
    │
    ├── builtins/         # Built-in commands
    │   ├── builtins.c         # Dispatcher
    │   ├── builtin_echo.c     # echo command
    │   ├── builtin_cd.c       # cd command
    │   ├── builtin_pwd.c      # pwd command
    │   ├── builtin_export.c   # export command
    │   ├── builtin_unset.c    # unset command
    │   ├── builtin_env.c      # env command
    │   └── builtin_exit.c     # exit command
    │
    ├── environment/      # Environment management
    │   ├── env.c              # Init and SHLVL
    │   ├── env_node.c         # Linked list operations
    │   ├── env_utils.c        # Conversion utilities
    │   └── env_array.c
    │
    ├── history/          # Command history
    │   ├── history.c          # History management
    │   ├── history_utils.c
    │   └── history_load.c
    │
    ├── signals/          # Signal handlers
    │   └── signals.c          # SIGINT/SIGQUIT handling
    │
    └── utils/            # Helper functions
        ├── utils.c            # General utilities
        └── utils_num.c        # Number validation
```

## Key Data Structures

### t_token (Lexer Output)
```c
typedef struct s_token
{
    t_token_type    type;   // WORD, PIPE, REDIR_IN, etc.
    char            *value; // The actual string
    struct s_token  *next;  // Linked list
}   t_token;
```

### t_cmd (Single Command)
```c
typedef struct s_cmd
{
    char            **args;    // Command + arguments
    t_redir         *redirs;   // Redirections list
    struct s_cmd    *next;     // Next command in pipe
    int             expanded;  // Already expanded flag
}   t_cmd;
```

### t_pipeline (Pipeline of Commands)
```c
typedef struct s_pipeline
{
    t_cmd               *cmds;     // Commands in pipe
    t_token_type        logic_op;  // (unused in mandatory)
    struct s_pipeline   *next;     // Next pipeline
}   t_pipeline;
```

### t_shell (Shell State)
```c
typedef struct s_shell
{
    t_env   *env;           // Environment variables
    int     exit_status;    // Last command exit code
    int     should_exit;    // Exit flag
    int     interactive;    // Is TTY?
    // ... other fields
}   t_shell;
```

## Global Variable

The only global variable allowed by 42 subject:
```c
volatile sig_atomic_t g_signal = 0;
```
Used exclusively to store the signal number received (e.g., SIGINT = 2).

## Execution Flow Example

Input: `ls -la | grep ".c" > output.txt`

1. **Lexer** creates tokens:
   - `[WORD:"ls"]` `[WORD:"-la"]` `[PIPE:"|"]` `[WORD:"grep"]` `[WORD:".c"]` `[REDIR_OUT:">"]` `[WORD:"output.txt"]`

2. **Parser** builds structure:
   ```
   t_pipeline
   └── t_cmd (ls -la)
       └── next: t_cmd (grep ".c")
                 └── redirs: REDIR_OUT → "output.txt"
   ```

3. **Expander** processes:
   - No variables to expand
   - Removes quotes: `.c` stays as `.c`

4. **Executor**:
   - Creates pipe between commands
   - Forks for `ls -la`, connects stdout to pipe
   - Forks for `grep .c`, connects stdin from pipe, stdout to file
   - Waits for both processes to finish

## Exit Codes

| Code  | Meaning              |
|-------|----------------------|
|  0    | Success              |
|  1    | General error        |
|  2    | Syntax error         |
|  126  | Permission denied    |
|  127  | Command not found    |
| 128+N | Killed by signal N   |
| 130   | Interrupted (Ctrl+C) |
