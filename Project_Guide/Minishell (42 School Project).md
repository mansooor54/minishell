# Minishell (42 School Project)

This is a complete implementation of the Minishell project for 42 School. It is a simplified version of a command-line interpreter like `bash` or `zsh`, built from scratch in C. This project covers both the mandatory and bonus requirements of the 42 curriculum.

## Features

### Mandatory Features

- **Prompt Display**: A prompt is displayed when the shell is ready for a new command.
- **Command History**: The shell keeps a history of commands that can be navigated with the arrow keys.
- **Command Execution**: The shell can find and execute commands from the system `PATH` or with a relative/absolute path.
- **Quote Handling**:
  - Single quotes (`'`) prevent any interpretation of metacharacters.
  - Double quotes (`"`) prevent interpretation of metacharacters except for the dollar sign (`$`) for variable expansion.
- **Redirections**:
  - `<`: Redirect standard input.
  - `>`: Redirect standard output (overwrite).
  - `>>`: Append standard output.
  - `<<`: Here document.
- **Pipes (`|`)**: The output of one command can be piped as the input to another.
- **Environment Variables**: The shell can expand environment variables (e.g., `$HOME`).
- **Exit Status (`$?`)**: The special variable `$?` expands to the exit status of the most recent command.
- **Signal Handling**:
  - `Ctrl-C`: Displays a new prompt without exiting the shell.
  - `Ctrl-D`: Exits the shell.
  - `Ctrl-\`: Does nothing.

### Built-in Commands

- `echo` with the `-n` option.
- `cd` with relative or absolute paths.
- `pwd` with no options.
- `export` to set environment variables.
- `unset` to remove environment variables.
- `env` to display environment variables.
- `exit` to exit the shell.

### Bonus Features

- **Logical Operators**:
  - `&&` (AND): The next command is executed only if the previous one succeeds.
  - `||` (OR): The next command is executed only if the previous one fails.
- **Wildcards (`*`)**: Wildcard expansion for the current directory is supported (basic implementation).

## Architecture

The shell is designed with a modular architecture, following the recommended `lexer` -> `parser` -> `expander` -> `executor` pipeline:

1.  **Lexer**: The input string is tokenized into a list of tokens (words, operators, etc.).
2.  **Parser**: The tokens are parsed into a structured representation of commands, pipelines, and logical operators.
3.  **Expander**: Environment variables and special variables are expanded.
4.  **Executor**: The commands are executed, handling built-ins, external commands, pipes, and redirections.

## How to Compile and Run

### Prerequisites

- `make`
- `gcc`
- `readline` library (on macOS, you can install it with `brew install readline`)

### Compilation

To compile the project, run the following command in the root directory:

```bash
make
```

This will:
1. Compile the `libft` library
2. Compile all minishell source files
3. Link everything together to create the `minishell` executable

### Running the Shell

To start the shell, run the executable:

```bash
./minishell
```

### Cleaning

```bash
make clean   # Remove object files
make fclean  # Remove object files and executables
make re      # Rebuild everything from scratch
```

## Usage Examples

### Basic Commands

```bash
ls -l
cat file.txt
echo "Hello, World!"
```

### Redirections

```bash
ls > file.txt
cat < file.txt
echo "hello" >> file.txt
cat << EOF
line 1
line 2
EOF
```

### Pipes

```bash
ls | grep .c | wc -l
cat file.txt | grep pattern | sort
```

### Environment Variables

```bash
export MYVAR=hello
echo $MYVAR
echo $?
unset MYVAR
```

### Logical Operators (Bonus)

```bash
make && ./minishell
cat non_existent_file || echo "File not found"
ls && echo "success" || echo "failed"
```

## Code Structure

The code is organized into the following directories:

- `include/`: Contains the main header file `minishell.h`.
- `libft/`: Complete libft implementation with all necessary utility functions.
- `src/`: Contains the source code, organized into subdirectories:
  - `lexer/`: Tokenization logic.
  - `parser/`: Parsing logic.
  - `expander/`: Variable expansion logic.
  - `executor/`: Command execution logic.
  - `builtins/`: Built-in command implementations.
  - `signals/`: Signal handling logic.
  - `utils/`: Utility functions (environment management, helper functions).

## Project Structure

```
minishell/
├── Makefile                 # Main compilation rules
├── README.md                # This file
├── include/
│   └── minishell.h         # Main header file
├── libft/
│   ├── Makefile            # Libft compilation rules
│   ├── libft.h             # Libft header
│   └── *.c                 # Libft source files
└── src/
    ├── main.c              # Main shell loop
    ├── lexer/              # Tokenization
    ├── parser/             # Command structure building
    ├── expander/           # Variable expansion
    ├── executor/           # Command execution
    ├── builtins/           # Built-in commands
    ├── signals/            # Signal handling
    └── utils/              # Utility functions
```

## Norminette Compliance

This project adheres to the Norminette coding standard of 42 School:
- Maximum 5 functions per file
- Maximum 25 lines per function
- Proper header comments on all files
- Consistent naming conventions

## Notes

- The project includes a complete `libft` implementation with all necessary string, memory, and conversion functions.
- All utility functions from libft are used throughout the project to avoid code duplication.
- The code is designed to be memory-leak free and handles all file descriptors properly.
- Error handling is implemented throughout to ensure the shell never crashes.

## Testing

Test the shell thoroughly with various commands, redirections, pipes, and edge cases. The shell should behave similarly to `bash` for all mandatory features.

---

**Author**: Manus AI  
**Date**: November 2, 2025
