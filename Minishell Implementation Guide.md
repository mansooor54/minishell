# Minishell Implementation Guide

This document provides a comprehensive guide to the minishell implementation, explaining the architecture, key concepts, and how each component works together.

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Component Details](#component-details)
4. [Mandatory Features](#mandatory-features)
5. [Bonus Features](#bonus-features)
6. [Important Notes](#important-notes)
7. [Compilation and Testing](#compilation-and-testing)

## Overview

The minishell project is a simplified command-line interpreter that mimics the behavior of `bash`. It demonstrates fundamental concepts of operating systems, including process management, file descriptors, signal handling, and parsing.

The implementation follows a clean, modular architecture that separates concerns into distinct phases: **lexing**, **parsing**, **expanding**, and **executing**. This design makes the code easier to understand, maintain, and extend.

## Architecture

The shell processes each line of user input through four main stages:

### 1. Lexer (Tokenization)

The lexer takes the raw input string and breaks it into **tokens**. Each token represents a meaningful unit such as a word, operator, or special character.

**Example:**
```
Input: echo "hello world" | grep hello > output.txt
Tokens: [WORD:echo] [WORD:"hello world"] [PIPE:|] [WORD:grep] [WORD:hello] [REDIR_OUT:>] [WORD:output.txt]
```

**Key Files:**
- `src/lexer/lexer.c`: Token creation and management
- `src/lexer/lexer_utils.c`: Tokenization logic with quote handling

### 2. Parser (Structure Building)

The parser takes the token stream and builds a structured representation of the command. It creates a tree-like structure that represents:
- Individual commands with their arguments
- Redirections for each command
- Pipes connecting commands
- Logical operators (&&, ||) for bonus

**Data Structures:**
- `t_cmd`: Represents a single command with arguments and redirections
- `t_pipeline`: Represents a sequence of commands connected by pipes
- `t_redir`: Represents a redirection operation

**Key Files:**
- `src/parser/parser.c`: Command and redirection parsing
- `src/parser/parser_pipeline.c`: Pipeline and logical operator handling

### 3. Expander (Variable Expansion)

The expander processes the parsed commands and expands:
- Environment variables (`$HOME`, `$PATH`, etc.)
- Exit status variable (`$?`)
- Removes quotes from arguments

**Example:**
```
Before: echo "$HOME/file.txt"
After: echo "/Users/student/file.txt"
```

**Key Files:**
- `src/expander/expander.c`: Variable expansion and quote removal

### 4. Executor (Command Execution)

The executor takes the expanded commands and executes them. It handles:
- Built-in commands (executed in the parent process)
- External commands (executed in child processes via `fork` and `execve`)
- Pipes (using `pipe()` system call)
- Redirections (using `dup2()` system call)
- Logical operators (&&, ||)

**Key Files:**
- `src/executor/executor.c`: Single command execution
- `src/executor/executor_pipeline.c`: Pipeline execution with pipes
- `src/executor/redirections.c`: File redirection handling

## Component Details

### Lexer

The lexer handles several important cases:

1. **Quote Handling**: It recognizes single and double quotes and treats the content inside as a single token.
2. **Operator Recognition**: It identifies special operators like `|`, `<`, `>`, `<<`, `>>`, `&&`, and `||`.
3. **Whitespace Handling**: It skips whitespace between tokens but preserves it inside quotes.

### Parser

The parser builds a hierarchical structure:

1. **Pipeline Level**: Groups commands separated by pipes (`|`).
2. **Command Level**: Each command has arguments and redirections.
3. **Redirection Level**: Each redirection has a type and a target file.

### Expander

The expander performs several transformations:

1. **Variable Expansion**: Replaces `$VAR` with the value of the environment variable.
2. **Exit Status Expansion**: Replaces `$?` with the exit status of the last command.
3. **Quote Removal**: Removes single and double quotes from arguments.

**Important**: Variables inside single quotes are **not** expanded, but variables inside double quotes **are** expanded.

### Executor

The executor is the most complex component:

1. **Built-in Detection**: Checks if the command is a built-in and executes it directly.
2. **External Command Execution**:
   - Forks a child process
   - Sets up redirections in the child
   - Finds the executable in PATH
   - Calls `execve()` to replace the child process
3. **Pipeline Execution**:
   - Creates pipes between commands
   - Forks a child for each command
   - Sets up input from the previous pipe and output to the next pipe
   - Waits for all children to complete

## Mandatory Features

### Built-in Commands

The following built-in commands are implemented:

| Command | Description | Example |
|---------|-------------|---------|
| `echo` | Print arguments to stdout, supports `-n` flag | `echo -n "hello"` |
| `cd` | Change directory | `cd /home/user` |
| `pwd` | Print working directory | `pwd` |
| `export` | Set environment variable | `export VAR=value` |
| `unset` | Remove environment variable | `unset VAR` |
| `env` | Display environment variables | `env` |
| `exit` | Exit the shell | `exit 0` |

### Redirections

| Operator | Description | Example |
|----------|-------------|---------|
| `<` | Redirect input from file | `cat < file.txt` |
| `>` | Redirect output to file (overwrite) | `ls > output.txt` |
| `>>` | Redirect output to file (append) | `echo "text" >> file.txt` |
| `<<` | Here document (read until delimiter) | `cat << EOF` |

### Pipes

Pipes connect the output of one command to the input of another:

```bash
ls -l | grep .c | wc -l
```

### Environment Variables

The shell expands environment variables:

```bash
echo $HOME
echo $PATH
```

### Exit Status

The special variable `$?` contains the exit status of the last command:

```bash
ls
echo $?  # Prints 0 if ls succeeded
```

### Signal Handling

| Signal | Behavior |
|--------|----------|
| `Ctrl-C` (SIGINT) | Display new prompt, do not exit |
| `Ctrl-D` (EOF) | Exit the shell |
| `Ctrl-\` (SIGQUIT) | Do nothing |

## Bonus Features

### Logical Operators

The shell supports logical operators for conditional execution:

| Operator | Description | Example |
|----------|-------------|---------|
| `&&` | Execute next command only if previous succeeded | `make && ./minishell` |
| `||` | Execute next command only if previous failed | `cat file \|\| echo "failed"` |

### Wildcards

The shell supports wildcard expansion for `*` in the current directory:

```bash
ls *.c
cat file*.txt
```

**Note**: This is a basic implementation and may not cover all edge cases.

## Important Notes

### Norminette Compliance

The code follows the Norminette coding standard:
- Maximum 5 functions per file
- Maximum 25 lines per function
- Proper header comments
- Consistent naming conventions

### Memory Management

The implementation is designed to avoid memory leaks:
- All allocated memory is freed before the shell exits
- Token lists, command structures, and environment variables are properly freed
- Child processes clean up their resources before exiting

### File Descriptor Management

The shell properly manages file descriptors:
- Redirections are set up using `dup2()`
- Unused pipe ends are closed
- File descriptors are not leaked in child processes

### Error Handling

The shell handles errors gracefully:
- Invalid commands print error messages
- Failed system calls are checked and reported
- The shell never crashes, even with invalid input

## Compilation and Testing

### Compilation

```bash
make
```

This creates the `minishell` executable.

### Testing

Test the shell with various commands:

```bash
./minishell
minishell> echo "Hello, World!"
minishell> ls -l | grep .c
minishell> export TEST=value
minishell> echo $TEST
minishell> cd ..
minishell> pwd
minishell> exit
```

### Testing Redirections

```bash
minishell> echo "test" > file.txt
minishell> cat < file.txt
minishell> echo "more" >> file.txt
minishell> cat << EOF
> line 1
> line 2
> EOF
```

### Testing Pipes

```bash
minishell> ls | wc -l
minishell> cat file.txt | grep pattern | sort
```

### Testing Logical Operators (Bonus)

```bash
minishell> true && echo "success"
minishell> false || echo "failed"
minishell> make && ./minishell
```

### Testing Signals

- Press `Ctrl-C` to interrupt and get a new prompt
- Press `Ctrl-D` to exit the shell
- Press `Ctrl-\` (should do nothing)

## Conclusion

This minishell implementation provides a solid foundation for understanding how shells work. It covers all mandatory requirements and includes bonus features. The modular architecture makes it easy to extend and modify.

For any questions or issues, refer to the source code comments, which explain what each function does.

---

**Author**: Manus AI  
**Date**: November 2, 2025
