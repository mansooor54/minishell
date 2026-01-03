# Minishell Project Overview

## What is Minishell?

Minishell is a simplified shell (like bash or zsh) written in C. A shell is a program that:
1. Shows a prompt and waits for user input
2. Reads what user types
3. Understands and executes the command
4. Shows the result
5. Repeats forever until user exits

**Real-life analogy**: Think of a shell like a waiter in a restaurant:
- Waiter asks "What would you like?" (prompt)
- You say "Bring me coffee" (command)
- Waiter understands and brings coffee (execution)
- Waiter comes back and asks again (loop)

---

## How a Command is Processed (Step by Step)

Let's follow what happens when you type: `echo "Hello $USER" | cat > file.txt`

### Step 1: Reading Input
```
User types: echo "Hello $USER" | cat > file.txt
            ↓
Shell receives this as a string
```

### Step 2: Lexer (Tokenization)
**What**: Breaks the string into meaningful pieces called "tokens"
**Why**: Like breaking a sentence into words

```
Input:  echo "Hello $USER" | cat > file.txt

Tokens created:
┌─────────────────────┐
│ WORD: "echo"        │
├─────────────────────┤
│ WORD: "Hello $USER" │  ← quotes kept for now
├─────────────────────┤
│ PIPE: "|"           │
├─────────────────────┤
│ WORD: "cat"         │
├─────────────────────┤
│ REDIR_OUT: ">"      │
├─────────────────────┤
│ WORD: "file.txt"    │
└─────────────────────┘
```

**Real-life analogy**: Like reading "I want coffee" and identifying:
- "I" = subject
- "want" = verb
- "coffee" = object

### Step 3: Parser (Building Structure)
**What**: Organizes tokens into a tree structure
**Why**: Understands relationships (which command, which file, etc.)

```
From tokens, parser builds:

t_pipeline
└── cmds:
    ├── t_cmd #1
    │   ├── args: ["echo", "Hello $USER"]
    │   └── redirs: NULL
    │   └── next: ───────────────────────┐
    │                                     │
    └── t_cmd #2 ◄────────────────────────┘
        ├── args: ["cat"]
        └── redirs:
            └── t_redir
                ├── type: REDIR_OUT (>)
                └── file: "file.txt"
```

**Real-life analogy**: Understanding that in "Give the book to John":
- Action: Give
- Object: book
- Recipient: John

### Step 4: Expander (Variable Substitution)
**What**: Replaces `$VARIABLE` with actual values
**Why**: Variables like `$USER` need to become real values

```
Before expansion:
  args: ["echo", "Hello $USER"]

After expansion (if USER=mansoor):
  args: ["echo", "Hello mansoor"]

Also removes quotes:
  args: ["echo", "Hello mansoor"]  ← no more quotes
```

**Real-life analogy**: Like replacing "my address" with your actual address "123 Main St"

### Step 5: Executor (Running Commands)
**What**: Actually runs the commands
**Why**: This is the actual work!

```
1. Create a pipe (connection between commands)

2. Fork child #1 for "echo":
   ┌──────────────────────────────────┐
   │ Child Process #1                 │
   │ - stdout → pipe write end        │
   │ - runs: echo "Hello mansoor"     │
   │ - writes "Hello mansoor" to pipe │
   └──────────────────────────────────┘

3. Fork child #2 for "cat":
   ┌───────────────────────────────────┐
   │ Child Process #2                  │
   │ - stdin ← pipe read end           │
   │ - stdout → file.txt               │
   │ - runs: cat                       │
   │ - reads from pipe, writes to file │
   └───────────────────────────────────┘

4. Wait for both children to finish

5. file.txt now contains: "Hello mansoor"
```

---

## Project Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                           USER INPUT                                 │
│                    "ls -la | grep .c > out.txt"                     │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         1. SHELL LOOP                                │
│                                                                      │
│   while (1) {                                                        │
│       show_prompt();      // "minishell> "                          │
│       line = readline();  // wait for user                          │
│       process(line);      // do everything below                    │
│   }                                                                  │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────┐
│                           2. LEXER                                   │
│                                                                      │
│   Input:  "ls -la | grep .c"                                     k   │
│                                                                      │
│   Output: [WORD:ls] → [WORD:-la] → [PIPE:|] → [WORD:grep] → [WORD:.c]│
│                                                                      │
│   Think of it as: Breaking sentence into words                       │
└──────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────┐
│                          3. PARSER                                   │
│                                                                      │
│   Input:  Token list                                                 │
│                                                                      │
│   Output: Command structure                                          │
│           cmd1 (ls -la) ──pipe──► cmd2 (grep .c)                     │
│                                                                      │
│   Think of it as: Understanding grammar of the sentence              │
└──────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────┐
│                         4. EXPANDER                                  │
│                                                                      │
│   Input:  "echo $HOME"                                               │
│   Output: "echo /Users/mansoor"                                      │
│                                                                      │
│   - Replaces $VAR with values                                        │
│   - Removes quotes                                                   │
│   - Handles $? (exit status)                                         │
└──────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────┐
│                         5. EXECUTOR                                  │
│                                                                      │
│   For each command:                                                  │
│   1. Is it builtin (echo/cd/pwd)? → Run directly                     │
│   2. Is it external (ls/grep)?    → Fork + Execve                    │
│   3. Has pipes?                   → Connect with pipe()              │
│   4. Has redirections?            → Redirect stdin/stdout            │
└──────────────────────────────────────────────────────────────────────┘
```

---

## Directory Structure Explained

```
minishell/
├── Makefile              # How to build the project
│
├── include/
│   └── minishell.h       # All function declarations
│                         # All struct definitions
│                         # All includes
│
├── libft/                # Your libft library
│   └── (libft files)     # ft_strlen, ft_strdup, etc.
│
└── src/
    │
    ├── main/                 # WHERE PROGRAM STARTS
    │   ├── main.c            # main() function
    │   └── minishell_logo.c  # Pretty logo printing
    │
    ├── core/                 # THE MAIN LOOP
    │   ├── shell_loop.c      # while(1) { read; execute; }
    │   ├── read_logical_line.c  # Handle multi-line input
    │   └── ...
    │
    ├── lexer/                # BREAKING INTO TOKENS
    │   ├── lexer.c           # Main lexer
    │   ├── lexer_operator.c  # Handle |, <, >, <<, >>
    │   └── lexer_utils.c     # Helper functions
    │
    ├── parser/               # BUILDING COMMAND STRUCTURE
    │   ├── parser.c          # Main parser
    │   ├── parser_pipeline.c # Handle pipes
    │   └── parser_syntax_check.c  # Check for errors
    │
    ├── expander/             # VARIABLE EXPANSION
    │   ├── expander_core.c   # Main expansion
    │   ├── expander_vars.c   # $VAR handling
    │   └── expander_quotes.c # Quote removal
    │
    ├── executor/         # RUNNING COMMANDS
    │   ├── executor.c        # Main executor
    │   ├── executor_pipeline.c   # Pipe handling
    │   ├── executor_external.c   # Fork + exec
    │   └── executor_redirections.c  # < > >> <<
    │
    ├── builtins/         # BUILT-IN COMMANDS
    │   ├── builtin_echo.c    # echo command
    │   ├── builtin_cd.c      # cd command
    │   ├── builtin_pwd.c     # pwd command
    │   ├── builtin_export.c  # export command
    │   ├── builtin_unset.c   # unset command
    │   ├── builtin_env.c     # env command
    │   └── builtin_exit.c    # exit command
    │
    ├── environment/      # ENVIRONMENT VARIABLES
    │   ├── env.c             # Initialize from envp
    │   ├── env_node.c        # Linked list operations
    │   └── env_utils.c       # Get/set values
    │
    ├── signals/          # CTRL+C, CTRL+D, CTRL+\
    │   └── signals.c         # Signal handlers
    │
    └── utils/            # HELPER FUNCTIONS
        └── utils.c           # free_array, etc.
```

---

## Key Data Structures Explained with Examples

### t_token (Lexer Output)
```c
typedef struct s_token
{
    t_token_type    type;   // What kind? WORD, PIPE, etc.
    char            *value; // The actual text
    struct s_token  *next;  // Pointer to next token
}   t_token;
```

**Visual Example**:
```
Input: "ls | grep foo"

Creates linked list:
┌──────────────┐    ┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ type: WORD   │    │ type: PIPE   │    │ type: WORD   │    │ type: WORD   │
│ value: "ls"  │───►│ value: "|"   │───►│ value: "grep"│───►│ value: "foo" │───► NULL
└──────────────┘    └──────────────┘    └──────────────┘    └──────────────┘
```

### t_cmd (Single Command)
```c
typedef struct s_cmd
{
    char            **args;    // ["ls", "-la", NULL]
    t_redir         *redirs;   // List of redirections
    struct s_cmd    *next;     // Next command in pipe
    int             expanded;  // Already processed?
}   t_cmd;
```

**Visual Example**:
```
Command: "ls -la > out.txt"

t_cmd:
┌─────────────────────────────────────┐
│ args: ──────► ["ls", "-la", NULL]   │
│               [0]   [1]    [2]      │
│                                     │
│ redirs: ────► t_redir               │
│               ├── type: REDIR_OUT   │
│               └── file: "out.txt"   │
│                                     │
│ next: NULL (no more commands)       │
└─────────────────────────────────────┘
```

### t_pipeline (Connected Commands)
```c
typedef struct s_pipeline
{
    t_cmd               *cmds;     // First command
    t_token_type        logic_op;  // (not used in mandatory)
    struct s_pipeline   *next;     // (not used in mandatory)
}   t_pipeline;
```

**Visual Example for `ls | grep .c | wc -l`**:
```
t_pipeline
└── cmds: ─────────────────────────────────────────────────┐
                                                           │
    ┌──────────────────────────────────────────────────────▼─┐
    │ t_cmd                                                   │
    │ args: ["ls", NULL]                                      │
    │ next: ─────────────────────────────────────────────────┐│
    └────────────────────────────────────────────────────────┘│
                                                              │
    ┌─────────────────────────────────────────────────────────▼┐
    │ t_cmd                                                    │
    │ args: ["grep", ".c", NULL]                               │
    │ next: ─────────────────────────────────────────────────┐ │
    └────────────────────────────────────────────────────────┘ │
                                                               │
    ┌──────────────────────────────────────────────────────────▼┐
    │ t_cmd                                                     │
    │ args: ["wc", "-l", NULL]                                  │
    │ next: NULL                                                │
    └───────────────────────────────────────────────────────────┘
```

---

## The Global Variable Explained

```c
volatile sig_atomic_t g_signal = 0;
```

**Why only ONE global?**
- 42 subject allows only one global variable
- It must be used only for signals
- Signals interrupt normal execution, so we need a way to communicate

**Example: What happens when you press Ctrl+C**
```
Step 1: You're typing at prompt
        minishell> echo hel|     ← cursor here

Step 2: You press Ctrl+C
        ↓
        Operating system sends SIGINT signal to our program

Step 3: Signal handler runs immediately (interrupts normal code)
        ┌─────────────────────────────┐
        │ handle_sigint(int sig)      │
        │ {                           │
        │     g_signal = sig;  // 2   │ ← stores signal number
        │     write(1, "\n", 1);      │ ← print newline
        │     rl_redisplay();         │ ← show new prompt
        │ }                           │
        └─────────────────────────────┘

Step 4: Shell loop checks g_signal
        if (g_signal == SIGINT)       ← "was Ctrl+C pressed?"
            exit_status = 130;        ← set correct exit code
            g_signal = 0;             ← reset for next time

Step 5: Fresh prompt appears
        minishell>                    ← ready for new input
```

---

## Complete Example: From Input to Output

**User types**: `cat < input.txt | grep hello > output.txt`

### Visual Step-by-Step:

```
┌────────────────────────────────────────────────────────────────────┐
│ STEP 1: LEXER                                                      │
│                                                                    │
│ Input string: "cat < input.txt | grep hello > output.txt"          │
│                                                                    │
│ Output tokens:                                                     │
│ [WORD:cat]──►[REDIR_IN:<]──►[WORD:input.txt]──►[PIPE:|]──►         │
│ [WORD:grep]──►[WORD:hello]──►[REDIR_OUT:>]──►[WORD:output.txt]     │
└────────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────────┐
│ STEP 2: PARSER                                                     │
│                                                                    │
│ Builds this structure:                                             │
│                                                                    │
│ t_pipeline                                                         │
│ └── cmds:                                                          │
│     ├── t_cmd #1                                                   │
│     │   ├── args: ["cat", NULL]                                    │
│     │   ├── redirs: [type:REDIR_IN, file:"input.txt"]             │
│     │   └── next: ────────────────────────┐                        │
│     │                                      ▼                       │
│     └── t_cmd #2                                                   │
│         ├── args: ["grep", "hello", NULL]                          │
│         ├── redirs: [type:REDIR_OUT, file:"output.txt"]           │
│         └── next: NULL                                             │
└────────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────────┐
│ STEP 3: EXPANDER                                                   │
│                                                                    │
│ No $variables in this example, so args stay the same.              │
│ If we had "$USER", it would become "mansoor"                       │
└────────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────────┐
│ STEP 4: EXECUTOR                                                   │
│                                                                    │
│ (a) Create a pipe:                                                 │
│     ┌─────────┐                                                    │
│     │  PIPE   │  pipe[0] = read end                                │
│     │ [0] [1] │  pipe[1] = write end                               │
│     └─────────┘                                                    │
│                                                                    │
│ (b) Fork child #1 for "cat":                                       │
│     ┌────────────────────────────────────────────┐                 │
│     │ CHILD PROCESS #1                           │                 │
│     │                                            │                 │
│     │ Step 1: Open "input.txt"                   │                 │
│     │ Step 2: dup2(file, STDIN)  ← read from file│                 │
│     │ Step 3: dup2(pipe[1], STDOUT) ← write to pipe                │
│     │ Step 4: execve("/bin/cat", ["cat"], env)   │                 │
│     │                                            │                 │
│     │ Data flow: input.txt ──► cat ──► pipe      │                 │
│     └────────────────────────────────────────────┘                 │
│                                                                    │
│ (c) Fork child #2 for "grep":                                      │
│     ┌────────────────────────────────────────────┐                 │
│     │ CHILD PROCESS #2                           │                 │
│     │                                            │                 │
│     │ Step 1: dup2(pipe[0], STDIN) ← read from pipe                │
│     │ Step 2: Open "output.txt"                  │                 │
│     │ Step 3: dup2(file, STDOUT) ← write to file │                 │
│     │ Step 4: execve("/bin/grep", ["grep","hello"], env)           │
│     │                                            │                 │
│     │ Data flow: pipe ──► grep ──► output.txt    │                 │
│     └────────────────────────────────────────────┘                 │
│                                                                    │
│ (d) Parent waits for both children to finish                       │
│                                                                    │
│ (e) Final data flow:                                               │
│     input.txt ──► cat ──► pipe ──► grep ──► output.txt             │
└────────────────────────────────────────────────────────────────────┘
```

---

## Exit Codes Explained with Examples

| Code | Meaning | Example Command | Result |
|------|---------|-----------------|--------|
| 0 | Success | `ls` | Files listed successfully |
| 1 | General error | `cd nonexistent` | Directory doesn't exist |
| 2 | Syntax error | `\| ls` | Pipe at start is invalid |
| 126 | Permission denied | `./script.sh` | File exists but not executable |
| 127 | Command not found | `asdfgh` | No such command |
| 130 | Interrupted (Ctrl+C) | `sleep 100` + Ctrl+C | User interrupted |

**How to check exit code in minishell**:
```bash
minishell> ls /nonexistent
ls: /nonexistent: No such file or directory
minishell> echo $?
1

minishell> ls
Makefile  src  include
minishell> echo $?
0

minishell> |
minishell: syntax error near unexpected token `|'
minishell> echo $?
2
```

---

## Quick Reference Tables

### Token Types
| Type | Symbol | Example | Meaning |
|------|--------|---------|---------|
| TOKEN_WORD | (text) | `ls`, `-la` | Command or argument |
| TOKEN_PIPE | `\|` | `ls \| grep` | Connect commands |
| TOKEN_REDIR_IN | `<` | `cat < file` | Read from file |
| TOKEN_REDIR_OUT | `>` | `echo > file` | Write to file (overwrite) |
| TOKEN_REDIR_APPEND | `>>` | `echo >> file` | Write to file (append) |
| TOKEN_REDIR_HEREDOC | `<<` | `cat << EOF` | Read until delimiter |

### Builtin Commands
| Command | Purpose | Example |
|---------|---------|---------|
| `echo` | Print text | `echo hello world` |
| `cd` | Change directory | `cd /home` |
| `pwd` | Print current directory | `pwd` |
| `export` | Set environment variable | `export VAR=value` |
| `unset` | Remove environment variable | `unset VAR` |
| `env` | Print all variables | `env` |
| `exit` | Exit the shell | `exit 0` |

### Quote Behavior
| Quote | Variable Expansion | Example Input | Output |
|-------|-------------------|---------------|--------|
| None | Yes | `echo $USER` | `mansoor` |
| Double `"` | Yes | `echo "$USER"` | `mansoor` |
| Single `'` | No | `echo '$USER'` | `$USER` |
