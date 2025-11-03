# Minishell Function Explanations

This document provides detailed explanations for all functions in the minishell project.

## Main Program (main.c)

### `init_shell(t_shell *shell, char **envp)`
**Purpose**: Initialize the shell structure with environment variables and default values.
**Parameters**: 
- shell: Pointer to shell structure to initialize
- envp: Environment variable array from main
**Returns**: void
**Description**: Sets up initial shell state by initializing environment variables from envp, setting exit_status to 0, and should_exit flag to 0.

### `process_line(char *line, t_shell *shell)`
**Purpose**: Process a single line of user input through the shell pipeline.
**Parameters**:
- line: Input string to process
- shell: Pointer to shell structure
**Returns**: void
**Description**: Implements the main shell pipeline: Lexer → Parser → Expander → Executor. Handles memory cleanup at each stage.

### `shell_loop(t_shell *shell)`
**Purpose**: Main interactive REPL (Read-Eval-Print-Loop) of the shell.
**Parameters**: shell: Pointer to shell structure
**Returns**: void
**Description**: Displays prompt, reads input with readline, adds to history, processes commands, and repeats until exit.

### `main(int argc, char **argv, char **envp)`
**Purpose**: Entry point of the minishell program.
**Parameters**: Standard main parameters
**Returns**: Exit status of last executed command
**Description**: Initializes shell, sets up signals, runs main loop, and cleans up before exiting.

## Lexer (src/lexer/)

### `create_token(t_token_type type, char *value)`
**Purpose**: Create a new token with specified type and value.
**Parameters**:
- type: Token type (TOKEN_WORD, TOKEN_PIPE, etc.)
- value: String value for the token
**Returns**: Pointer to new token or NULL on failure
**Description**: Allocates and initializes a token structure, duplicating the value string.

### `add_token(t_token **tokens, t_token *new_token)`
**Purpose**: Append a token to the end of a token list.
**Parameters**:
- tokens: Pointer to head of token list
- new_token: Token to append
**Returns**: void
**Description**: Traverses to the last token and links the new token to it.

### `free_tokens(t_token *tokens)`
**Purpose**: Free all tokens in a linked list.
**Parameters**: tokens: Head of token list
**Returns**: void
**Description**: Iterates through list, freeing both value strings and token structures.

### `lexer(char *input)`
**Purpose**: Tokenize an input string into a linked list of tokens.
**Parameters**: input: Command line string to tokenize
**Returns**: Pointer to head of token list
**Description**: Main lexer function that converts input into tokens, handling operators, words, and quoted strings.

## Parser (src/parser/)

### `parse_redirections(t_token **tokens)`
**Purpose**: Parse redirection operators and their target files from token stream.
**Parameters**: tokens: Pointer to current position in token list
**Returns**: Linked list of redirection structures
**Description**: Extracts all consecutive redirection operators (<, >, <<, >>) and their associated filenames.

### `parse_command(t_token **tokens)`
**Purpose**: Parse a single command with its arguments and redirections.
**Parameters**: tokens: Pointer to current position in token list
**Returns**: Command structure with args and redirections
**Description**: Builds a command structure from tokens until encountering a pipe or end of input.

### `parser(t_token *tokens)`
**Purpose**: Parse tokens into pipeline structures with logical operators.
**Parameters**: tokens: Head of token list
**Returns**: Linked list of pipeline structures
**Description**: Main parser function that handles pipes (|) and logical operators (&&, ||).

### `free_pipeline(t_pipeline *pipeline)`
**Purpose**: Free all memory allocated for pipeline structures.
**Parameters**: pipeline: Head of pipeline list
**Returns**: void
**Description**: Recursively frees all commands, arguments, redirections, and pipeline nodes.

## Expander (src/expander/)

### `get_env_value(t_env *env, char *key)`
**Purpose**: Retrieve environment variable value by key.
**Parameters**:
- env: Environment variable list
- key: Variable name to look up
**Returns**: Value string or NULL if not found
**Description**: Searches through environment list for matching key.

### `expand_variables(char *str, t_env *env, int exit_status)`
**Purpose**: Expand environment variables in a string.
**Parameters**:
- str: String containing variables to expand
- env: Environment variable list
- exit_status: Current exit status for $? expansion
**Returns**: New string with variables expanded
**Description**: Replaces $VAR with values and $? with exit status. Respects quote context (no expansion in single quotes).

### `expander(t_pipeline *pipeline, t_env *env)`
**Purpose**: Expand variables in all commands of a pipeline.
**Parameters**:
- pipeline: Pipeline structure to process
- env: Environment variable list
**Returns**: void
**Description**: Processes all command arguments and redirection files, expanding variables and removing quotes.

## Executor (src/executor/)

### `find_executable(char *cmd, t_env *env)`
**Purpose**: Find executable file in PATH or return absolute/relative path.
**Parameters**:
- cmd: Command name to find
- env: Environment variable list
**Returns**: Full path to executable or NULL if not found
**Description**: Searches PATH directories for command or validates absolute/relative paths.

### `setup_redirections(t_redir *redirs)`
**Purpose**: Set up file redirections for a command.
**Parameters**: redirs: Linked list of redirections
**Returns**: 0 on success, -1 on failure
**Description**: Opens files and duplicates file descriptors for input/output redirections.

### `execute_command(t_cmd *cmd, t_shell *shell)`
**Purpose**: Execute a single command.
**Parameters**:
- cmd: Command structure to execute
- shell: Shell state
**Returns**: void
**Description**: Handles builtins or forks to execute external commands with proper redirections.

### `execute_pipeline(t_pipeline *pipeline, t_shell *shell)`
**Purpose**: Execute a pipeline of commands connected by pipes.
**Parameters**:
- pipeline: Pipeline structure
- shell: Shell state
**Returns**: void
**Description**: Creates pipes, forks processes, and connects command outputs to inputs.

### `executor(t_pipeline *pipeline, t_shell *shell)`
**Purpose**: Execute pipelines with logical operators (&&, ||).
**Parameters**:
- pipeline: Pipeline structure
- shell: Shell state
**Returns**: void
**Description**: Main executor that handles conditional execution based on logical operators.

## Builtins (src/builtins/)

### `is_builtin(char *cmd)`
**Purpose**: Check if a command is a builtin.
**Parameters**: cmd: Command name
**Returns**: 1 if builtin, 0 otherwise
**Description**: Compares command name against list of builtin commands.

### `execute_builtin(t_cmd *cmd, t_shell *shell)`
**Purpose**: Execute a builtin command.
**Parameters**:
- cmd: Command structure
- shell: Shell state
**Returns**: Exit status of builtin
**Description**: Dispatches to appropriate builtin function based on command name.

### `builtin_echo(char **args)`
**Purpose**: Print arguments to stdout with optional -n flag.
**Parameters**: args: Argument array
**Returns**: 0 (success)
**Description**: Implements echo command with -n option to suppress newline.

### `builtin_cd(char **args, t_env *env)`
**Purpose**: Change current working directory.
**Parameters**:
- args: Argument array (args[1] is target directory)
- env: Environment variables
**Returns**: 0 on success, 1 on failure
**Description**: Changes to HOME if no argument, otherwise changes to specified directory.

### `builtin_pwd(void)`
**Purpose**: Print current working directory.
**Parameters**: None
**Returns**: 0 on success, 1 on failure
**Description**: Uses getcwd() to get and print current directory.

### `builtin_export(char **args, t_env **env)`
**Purpose**: Set environment variables.
**Parameters**:
- args: Argument array (KEY=VALUE pairs)
- env: Pointer to environment list
**Returns**: 0 on success
**Description**: Adds or updates environment variables. Prints all variables if no arguments.

### `builtin_unset(char **args, t_env **env)`
**Purpose**: Remove environment variables.
**Parameters**:
- args: Argument array (variable names)
- env: Pointer to environment list
**Returns**: 0 on success
**Description**: Removes specified environment variables from the list.

### `builtin_env(t_env *env)`
**Purpose**: Print all environment variables.
**Parameters**: env: Environment list
**Returns**: 0 (success)
**Description**: Prints all environment variables in KEY=VALUE format.

### `builtin_exit(char **args, t_shell *shell)`
**Purpose**: Exit the shell with optional exit code.
**Parameters**:
- args: Argument array (args[1] is optional exit code)
- shell: Shell state
**Returns**: Exit code
**Description**: Sets should_exit flag and returns specified exit code (or last exit status).

## Environment (src/utils/)

### `init_env(char **envp)`
**Purpose**: Initialize environment variable list from envp array.
**Parameters**: envp: Environment array from main
**Returns**: Head of environment list
**Description**: Parses envp strings and creates linked list of environment variables.

### `create_env_node(char *key, char *value)`
**Purpose**: Create a new environment variable node.
**Parameters**:
- key: Variable name
- value: Variable value
**Returns**: Pointer to new node
**Description**: Allocates and initializes an environment node with duplicated strings.

### `add_env_node(t_env **env, t_env *new_node)`
**Purpose**: Add environment node to list.
**Parameters**:
- env: Pointer to environment list head
- new_node: Node to add
**Returns**: void
**Description**: Appends node to end of environment list.

### `remove_env_node(t_env **env, char *key)`
**Purpose**: Remove environment variable by key.
**Parameters**:
- env: Pointer to environment list head
- key: Variable name to remove
**Returns**: void
**Description**: Searches for and removes node with matching key, freeing memory.

### `env_to_array(t_env *env)`
**Purpose**: Convert environment list to NULL-terminated array.
**Parameters**: env: Environment list
**Returns**: Array of KEY=VALUE strings
**Description**: Creates array format needed for execve() system call.

### `free_env(t_env *env)`
**Purpose**: Free all environment nodes.
**Parameters**: env: Head of environment list
**Returns**: void
**Description**: Iterates through list, freeing keys, values, and node structures.

## Signals (src/signals/)

### `handle_sigint(int sig)`
**Purpose**: Handle SIGINT signal (Ctrl-C).
**Parameters**: sig: Signal number
**Returns**: void
**Description**: Displays new prompt on new line without terminating shell. Sets exit status to 130.

### `handle_sigquit(int sig)`
**Purpose**: Handle SIGQUIT signal (Ctrl-\).
**Parameters**: sig: Signal number
**Returns**: void
**Description**: Does nothing in interactive mode (as required by subject).

### `setup_signals(void)`
**Purpose**: Set up signal handlers for the shell.
**Parameters**: None
**Returns**: void
**Description**: Configures SIGINT handler and ignores SIGQUIT.

## Utilities (src/utils/)

### `free_array(char **arr)`
**Purpose**: Free a NULL-terminated array of strings.
**Parameters**: arr: Array to free
**Returns**: void
**Description**: Frees each string in array, then frees array itself.

### `ft_strjoin_free(char *s1, char const *s2)`
**Purpose**: Join two strings and free the first one.
**Parameters**:
- s1: First string (will be freed)
- s2: Second string (not freed)
**Returns**: New joined string
**Description**: Utility function to concatenate strings while managing memory.

