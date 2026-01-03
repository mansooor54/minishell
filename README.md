# Minishell

A 42 School mini shell written in C that reimplements a subset of Bash behavior.

## Project Description

Minishell is a simple shell implementation following 42 School constraints:

* Single global variable (`g_signal`) for signal handling only (stores signal number)
* Norminette-compliant code
* Uses `readline` for interactive input and history
* Implements:
  * Prompt display and line editing
  * Command line parsing (words, quotes, operators)
  * Pipes `|`
  * Redirections: `<`, `>`, `>>`, `<<` (heredoc)
  * Environment variables management and `$VAR` / `$?` expansion
  * Builtins: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
  * Signal handling (ctrl-C, ctrl-D, ctrl-\)

## Features

| Feature      | Description                                                        |
|--------------|--------------------------------------------------------------------|
| Prompt       | Displays `minishell>` when waiting for input                       |
| History      | Arrow keys navigate command history                                |
| Pipes        | `cmd1 \| cmd2 \| cmd3` pipeline execution                          |
| Redirections | `<` input, `>` output, `>>` append, `<<` heredoc                   |
| Variables    | `$VAR` expansion, `$?` exit status                                 |
| Quotes       | Single quotes preserve literals, double quotes allow `$` expansion |
| Signals      | ctrl-C (new prompt), ctrl-D (exit), ctrl-\ (ignored)               |

## Project Structure

```
minishell/
├── Makefile
├── include/
│   └── minishell.h          # Central header with all declarations
├── libft/                    # Custom libft library
└── src/
    ├── main/                 # Entry point and logo
    ├── core/                 # Shell loop and line reading
    ├── lexer/                # Tokenization
    ├── parser/               # Command parsing and syntax validation
    ├── expander/             # Variable expansion and quote removal
    ├── executor/             # Command execution and pipes
    ├── builtins/             # Built-in commands
    ├── environment/          # Environment variable management
    ├── history/              # Command history
    ├── signals/              # Signal handlers
    └── utils/                # Helper functions
```

## Building

```bash
make        # Build minishell
make clean  # Remove object files
make fclean # Remove all generated files
make re     # Rebuild from scratch
make norm   # Run norminette check
make leak   # Run with valgrind (Linux)
```

## Execution Flow

1. **read_logical_line** - Read input with continuation for unclosed quotes
2. **lexer** - Convert input to tokens (`WORD`, `PIPE`, `REDIR`, etc.)
3. **validate_syntax** - Check for syntax errors
4. **parser** - Build command structure (`t_pipeline` → `t_cmd` → `t_redir`)
5. **expander** - Expand variables, remove quotes
6. **executor** - Run commands with pipes and redirections

## Builtins

| Builtin | Description |
|---------|-------------|
| `echo [-n]` | Print arguments (`-n` for no newline) |
| `cd [path]` | Change directory |
| `pwd` | Print working directory |
| `export [KEY=VALUE]` | Set environment variable |
| `unset KEY` | Remove environment variable |
| `env` | Print environment |
| `exit [code]` | Exit shell with optional exit code |

## Test Examples

```bash
# Basic commands
echo hello world
ls -la | grep minishell

# Pipes
cat Makefile | head -10 | tail -5

# Redirections
echo "test" > out.txt
cat < input.txt
cat << EOF
heredoc content
EOF
echo "append" >> out.txt

# Variable expansion
echo $HOME
echo $USER
echo $?

# Quotes
echo '$HOME'          # Literal: $HOME
echo "$HOME"          # Expanded: /Users/...
echo "Hello $USER"    # Mixed expansion

# Exit status
ls /nonexistent
echo $?               # Shows error code

# Signals
sleep 10              # Press ctrl-C to interrupt
                      # Press ctrl-D to exit shell
```

## Test Suite

### Basic Commands
```bash
./minishell
echo hello
ls
pwd
```

### Pipes
```bash
echo "hello" | cat
echo "test" | cat | cat | cat
ls -1 | grep '\.c' | wc -l
```

### Redirections
```bash
echo "hello" > out.txt
cat out.txt
echo "line2" >> out.txt
cat < out.txt
cat << EOF
hello
world
EOF
```

### Environment Variables
```bash
echo $HOME
echo "$HOME"
echo '$HOME'
export TEST=hello
echo $TEST
unset TEST
echo $?
```

### Builtins
```bash
pwd
cd /tmp
pwd
cd -
export VAR=value
env | grep VAR
unset VAR
exit 42
```

### Signals
```bash
# Ctrl+C at prompt - new prompt appears
# Ctrl+C during command - interrupts command
sleep 10
# Press Ctrl+C
echo $?   # Shows 130

# Ctrl+D at prompt - exits shell
# Ctrl+\ - does nothing
```

## Authors

* **Mansoor Almarzooqi** (malmarzo)
* **Afaf**

42 Abu Dhabi
