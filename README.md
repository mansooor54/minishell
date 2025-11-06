# Minishell - 42 School Project

A simple shell implementation in C, as beautiful as a shell.

## Project Description

This project implements a basic Unix shell that can:
- Display a prompt and execute commands
- Maintain command history
- Handle pipes and redirections
- Expand environment variables
- Implement built-in commands
- Handle signals (Ctrl-C, Ctrl-D, Ctrl-\)

## Project Structure

```
.
├── main.c                      # Main entry point (in root)
├── minishell.h                 # Main header file (in root)
├── Makefile                    # Build configuration
├── src/                        # Source files directory
│   ├── lexer/                  # Tokenization
│   ├── parser/                 # Command parsing
│   ├── expander/               # Variable expansion
│   ├── executor/               # Command execution
│   ├── builtins/               # Built-in commands
│   ├── signals/                # Signal handling
│   └── utils/                  # Utility functions
└── libft/                      # Custom C library
```

## Compilation

```bash
make        # Compile the project
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Recompile from scratch
```

## Features

### Mandatory Part

#### Core Functionality
- Interactive prompt display
- Working command history (using readline)
- Executable search based on PATH or absolute/relative paths
- Quote handling:
  - Single quotes (`'`) prevent interpretation of meta-characters
  - Double quotes (`"`) prevent interpretation except for `$`

#### Redirections
- `<` : Input redirection
- `>` : Output redirection
- `<<` : Heredoc (read until delimiter)
- `>>` : Append output redirection

#### Pipes
- `|` : Pipe character connecting command outputs to inputs

#### Environment Variables
- `$VARIABLE` : Expands to variable value
- `$?` : Expands to exit status of last command

#### Signal Handling
- `Ctrl-C` : Display new prompt on new line
- `Ctrl-D` : Exit the shell
- `Ctrl-\` : Do nothing

#### Built-in Commands
- `echo` with `-n` option
- `cd` with relative or absolute path
- `pwd` (no options)
- `export` (no options)
- `unset` (no options)
- `env` (no options or arguments)
- `exit` (no options)

### Bonus Part (if mandatory is perfect)
- `&&` and `||` with parenthesis for priorities
- Wildcards `*` for current working directory

## Implementation Details

### Global Variable
The project uses only **one global variable** (`g_shell`) as required by the subject.
This global is necessary for signal handlers to access shell state.

### Memory Management
- All heap-allocated memory is properly freed
- No memory leaks in user code (readline may have its own leaks)
- Proper error handling throughout

### External Functions Used
As specified in the subject:
- readline, rl_clear_history, rl_on_new_line, rl_replace_line, rl_redisplay, add_history
- printf, malloc, free, write, access, open, read, close
- fork, wait, waitpid, wait3, wait4, signal, sigaction, sigemptyset, sigaddset, kill, exit
- getcwd, chdir, stat, lstat, fstat, unlink, execve
- dup, dup2, pipe, opendir, readdir, closedir
- strerror, perror, isatty, ttyname, ttyslot, ioctl, getenv
- tcsetattr, tcgetattr, tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs

# Testing Results - All Passed!
```bash
✓ Basic commands (ls, pwd, echo)
✓ Built-in commands (cd, export, env, exit)
✓ Pipes (ls | grep Make)
✓ Redirections (echo test > file.txt)
✓ Environment variables ($TEST expansion)
✓ Directory navigation (cd -, cd /tmp)
```

## Usage Examples

```bash
$ ./minishell
minishell> echo "Hello World"
Hello World
minishell> ls -la | grep minishell
minishell> cat < input.txt > output.txt
minishell> export MY_VAR=value
minishell> echo $MY_VAR
value
minishell> echo $?
0
minishell> cd /tmp
minishell> pwd
/tmp
minishell> exit
```

## Function Documentation

Each function in the project includes detailed comments explaining:
- **Purpose**: What the function does
- **Parameters**: Input parameters and their meanings
- **Return value**: What the function returns
- **Description**: Detailed behavior and implementation notes

See individual source files for comprehensive function documentation.

## Compliance with 42 Subject

This implementation follows all requirements specified in the minishell subject (version 7.1):
- ✅ Written in C
- ✅ Follows Norm (if applicable)
- ✅ No unexpected crashes (segfault, bus error, double free, etc.)
- ✅ Proper memory management
- ✅ Makefile with required rules and flags (-Wall -Wextra -Werror)
- ✅ Uses only allowed external functions
- ✅ Includes libft
- ✅ One global variable for signal handling
- ✅ All mandatory features implemented

## Authors

42 School Student Project

## Version

7.1
