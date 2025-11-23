provide file include explaination of minishell_v021_norminette_semicolone_works project, explain each functions what it do, Project Description, Project Structure

## Merge All C Files in one File ##

  - cat src/**/*.c > code.c
  - grep -h '^#include' code.c | sort -u > tmp_includes.h
  - grep -hv '^#include' code.c >> tmp_includes.h
  - mv tmp_includes.h code.c                     


# minishell_v021_norminette_semicolone_works

Project Documentation

## 1. Project Description

`minishell_v021_norminette_semicolone_works` is a 42‐style mini shell written in C.
It reimplements a subset of Bash behavior while respecting the 42 constraints:

* Single global variable (`g_shell`) for shell state and signal handling.
* Norminette-compliant (function length, arguments, etc.).
* Uses `readline` for interactive input and history.
* Implements:

  * Prompt display and line editing.
  * Command line parsing (words, quotes, operators).
  * Pipes `|`.
  * Logical operators `&&` and `||`.
  * Command separators `;` with Bash-like behavior.
  * Redirections: `<`, `>`, `>>`, `<<` (heredoc).
  * Environment variables management and `$VAR` / `$?` expansion.
  * Builtins: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`, `history`.
  * Basic error messages and exit status handling that match Bash as much as possible.

High-level execution flow for one user input:

1. `read_logical_line`
   Reads one logical command line, handling unclosed quotes and backslash continuation.
2. `lexer`
   Converts the input string into a linked list of tokens (`WORD`, `PIPE`, `AND`, `OR`, `REDIR`, `SEMICOLON`, etc.).
3. `validate_syntax`
   Checks token sequence for syntax errors (double redir, starting with `;`, trailing `;`, etc.).
4. `parser`
   Builds a linked structure of pipelines and commands (`t_pipeline` → `t_cmd` → `t_redir`).
5. `expander`
   Expands environment variables and `$?`, removes quotes, and processes redirections.
6. `executor`
   Runs builtins directly or forks/execs external commands, handling pipes, redirections, heredocs, and `;`/`&&`/`||` logic.
7. Shell loop continues until `exit` builtin or EOF.

---

## 2. Project Structure

Root layout:

* `Makefile`
  Build rules for `minishell`, including compilation of all source files and `libft`.

* `libft/`
  Your custom libft library (string, list, memory helpers, etc.).

* `minishell.h`
  Central header:

  * Includes system headers and `libft`.
  * Defines structs: `t_token`, `t_redir`, `t_cmd`, `t_pipeline`, `t_env`, `t_shell`, `t_pipe_ctx`, `t_exp_ctx`, `t_quote_ctx`.
  * Declares all public functions for each module.
  * Defines error message macros (e.g. `ERR_PIPE`, `ERR_REDIR_IN`, `ERR_SEMICOLON`, …).
  * Declares the global shell instance `extern t_shell g_shell;`.

* `src/`

  * `main.c`
    Program entry point (`main`).
  * `minishell_logo.c`
    ASCII art / UAE flag minishell logo printed at startup.
  * `core/`

    * `shell_loop.c` – main shell loop.
    * `read_logical_line.c` – prompt and multi-line continuation logic.
    * `reader.c` / `shell_utils.c` – helpers for reading and basic line handling.
  * `lexer/`

    * `lexer.c` – token creation and main `lexer` function.
    * `lexer_operator.c` / `lexer_operator_type.c` – handling of operators (`|`, `&&`, `||`, redirs, `;`).
    * `lexer_utils.c` – helpers (`is_whitespace`, `extract_word`).
    * `lexer_unclose.c` – detection of unclosed quotes.
  * `parser/`

    * `parser.c` – parse commands and redirections.
    * `parser_pipeline.c` – build `t_pipeline` list, including `;`, `&&`, `||` logic.
    * `parser_syntax_check.c` / `parser_check_token.c` – syntax validation.
    * `parser_syntax_print.c` / `parser_error.c` – error messages for invalid syntax.
    * `parser_utils.c` – common parser helpers.
  * `expander/`

    * `expander.c` – entry point for expansion.
    * `expander_pipeline.c` – expand all commands in a pipeline.
    * `expander_core.c`, `expander_vars.c` – `$VAR` / `$?` logic.
    * `expander_quotes.c` – quote removal and context.
    * `expander_utils.c` – small helpers for expansion.
  * `executor/`

    * `executor.c` – resolve path and execute external commands.
    * `executor_pipeline.c` / `executor_pipeline_run.c` / `executor_pipeline_loop.c` / `executor_pipeline_utils.c` – multi-command pipeline execution.
    * `executor_commands.c` – run commands list and builtins.
    * `executor_path.c` / `executor_path_search.c` / `executor_path_utils.c` – PATH search and path utilities.
    * `executor_redirections.c` / `executor_redir_io.c` / `executor_redir_heredoc.c` – `<`, `>`, `>>`, `<<` handling.
    * `executor_child_fds.c` / `executor_child_run.c` – child process fork and file descriptor setup.
    * `executor_utils.c` / `executor_error.c` – common helpers and error reporting.
  * `builtins/`

    * `builtins.c` – `is_builtin` and `execute_builtin` dispatcher.
    * `builtin_echo.c`, `builtin_cd.c`, `builtin_cd_utils.c`, `builtin_pwd.c`,
      `builtin_export.c`, `builtin_unset.c`, `builtin_env.c`, `builtin_exit.c`.
  * `environment/`

    * `env.c` – initialize and configure environment + `init_shell`.
    * `env_node.c` – linked-list operations for env.
    * `env_array.c` – convert `t_env` list to `char **` for `execve`.
    * `env_utils.c` – parsing and append helpers.
  * `history/`

    * `history.c` – file-based history save/load and dedup.
    * `builtin_history.c` – `history` builtin implementation.
  * `signals/`

    * `signals.c` – `SIGINT` / `SIGQUIT` handlers and terminal mode (ECHOCTL off).
  * `utils/`

    * `utils.c` – general helpers (`free_array`, `ft_strjoin_free`, `free_env`, `print_logo`).

---

## 3. Core Execution Flow and Functions

### 3.1 MAIN & CORE

**Global state**

* `t_shell g_shell;`
  Single global instance holding:

  * `t_env *env;` – environment as linked list.
  * `int exit_status;` – last command exit code.
  * `int should_exit;` – flag to break main loop.
  * `int sigint_during_read;` – flag set when `Ctrl-C` happens during `readline`.
  * `char *history_path;` – path to history file.

**Functions**

* `int main(int argc, char **argv, char **envp);`
  Entry point:

  * Clears the screen.
  * Prints the logo.
  * Calls `init_shell` to build environment and initial state.
  * Calls `setup_signals` and `init_terminal` (signal and termios setup).
  * Initializes history (`history_init`).
  * Enters `shell_loop`.
  * On exit:

    * Saves history.
    * Clears readline history.
    * Frees environment and `history_path`.
    * Returns `g_shell.exit_status`.

* `void shell_loop(t_shell *shell);`
  Main REPL:

  * Repeatedly calls `read_logical_line` to get the next full line (including continuations).
  * Handles special cases (EOF / empty line) via an internal helper.
  * Skips lines that are all spaces.
  * Adds non-blank lines to history.
  * Calls `process_line` to lex, parse, expand, and execute.
  * Saves history when the loop ends.

* `int is_all_space(const char *s);`
  Checks if a string is only spaces/tabs. Used to ignore pure whitespace lines (no history entry, no execution).

* `void process_line(char *line, t_shell *shell);`
  Full pipeline for one user command line:

  * Runs the lexer on the input.
  * Validates syntax (`validate_syntax`).
  * Builds the pipeline structure (`parser`).
  * Expands variables and quotes (`expander`).
  * Executes the pipeline (`executor`).
  * Cleans up allocated structures.

* `char *read_logical_line(void);`
  Returns one complete logical line:

  * Reads the initial prompt (`minishell>`) using `readline` with colored prompt.
  * If the line has unclosed quotes or requires continuation (for example, trailing backslash), repeatedly reads lines with a secondary prompt (`> `) and joins them correctly (removing the escaping backslash that escapes the newline).
  * If `Ctrl-C` occurs during reading, returns `NULL` and sets `shell->exit_status` to 130, so the shell loop knows to display a fresh prompt without exiting.
  * If `Ctrl-D` at the main prompt, returns `NULL` so the loop can print `exit` and terminate.

* `int needs_continuation(const char *s);`
  Checks if the current line is incomplete:

  * Returns non-zero if there are unclosed quotes.
  * Returns non-zero if there is an odd number of trailing backslashes outside of quotes (means the newline is escaped and you should continue reading).
  * Returns zero if the line is syntactically complete for reading purposes.

---

## 4. Lexer

The lexer converts a raw input string into a linked list of tokens (`t_token`):

* Types: `TOKEN_WORD`, `TOKEN_PIPE`, `TOKEN_AND`, `TOKEN_OR`, `TOKEN_REDIR_IN`, `TOKEN_REDIR_OUT`, `TOKEN_REDIR_APPEND`, `TOKEN_REDIR_HEREDOC`, `TOKEN_SEMICOLON`, `TOKEN_EOF`.

**Functions (from `minishell.h`)**

* `t_token *lexer(char *input);`
  Main entry point:

  * Iterates over the input string.
  * Skips whitespace.
  * For each segment, calls `next_token` (inside `lexer.c`) to produce a token.
  * Appends new tokens to a linked list via `add_token`.
  * Returns the head of the list.

* `t_token *create_token(t_token_type type, char *value);`
  Allocates and initializes one token with given type and string value.

* `void add_token(t_token **tokens, t_token *new_token);`
  Appends `new_token` at the end of the `tokens` list.

* `void free_tokens(t_token *tokens);`
  Frees the entire linked list of tokens (types, values, nodes).

* `int has_unclosed_quotes(char *str);`
  Scans the string and checks whether there is an unmatched single or double quote, respecting escaping rules.

* `int is_whitespace(char c);`
  Returns non-zero if `c` is a space, tab, or similar whitespace.

* `int is_operator(char c);`
  Returns non-zero if `c` is an operator character (`|`, `<`, `>`, `&`, `;`).

* `int extract_word(char *input, char **word);`
  Copies the next word (a sequence of non-whitespace, non-operator characters, respecting quotes) into `*word` and returns its length. Used to create `TOKEN_WORD`.

* `t_token *get_operator_token(char **input);`
  Inspects characters at `*input` and calls the appropriate `try_*` helper to create:

  * `|`, `||`, `&&`, `<`, `>`, `<<`, `>>`, `;`.

* `t_token *try_or_pipe(char **input);`
  Detects `|` and `||` and returns token of type `TOKEN_PIPE` or `TOKEN_OR`.

* `t_token *try_and(char **input);`
  Detects `&&` and returns a `TOKEN_AND` token.

* `t_token *try_inredir(char **input);`
  Detects `<` or `<<` and returns `TOKEN_REDIR_IN` or `TOKEN_REDIR_HEREDOC`.

* `t_token *try_outredir(char **input);`
  Detects `>` or `>>` and returns `TOKEN_REDIR_OUT` or `TOKEN_REDIR_APPEND`.

*(In this version, `try_semicolon` exists in the lexer file to produce `TOKEN_SEMICOLON`, even if it’s not declared in the header. It is a private lexer helper.)*

---

## 5. Parser

The parser converts a token list into:

* A list of pipelines (`t_pipeline`), separated by `;`, `&&`, `||`.
* Each pipeline is a linked list of commands (`t_cmd`).
* Each command has arguments (`char **args`) and redirections (`t_redir`).

**Core parser functions**

* `t_pipeline *parser(t_token *tokens);`
  Main entry:

  * Walks the token list.
  * For each chunk, builds a `t_pipeline` node:

    * Calls `parse_pipe_sequence` to get a linked list of `t_cmd` for one pipeline.
    * Calls `set_logic_and_advance` to record the following logical operator (`;`, `&&`, `||`) and advance the token pointer.
  * Skips extra `;` tokens that separate pipelines.
  * Returns the head of the pipeline list.

* `t_cmd *parse_command(t_token **tokens);`
  Parses a single command:

  * Counts how many words belong to this command.
  * Allocates the `t_cmd` with enough space for arguments.
  * Copies each `TOKEN_WORD` value into `cmd->args`.
  * After each word, consumes any redirections (`<`, `>`, `>>`, `<<`) via `consume_redirs`.
  * Stops when reaching `PIPE`, `AND`, `OR`, `SEMICOLON`, or `EOF`.

* `t_redir *create_redir(t_token_type type, char *file);`
  Creates a redirection node storing:

  * `type` (in, out, append, heredoc).
  * `file` (string with filename or heredoc delimiter).

* `void append_redir(t_redir **head, t_redir *new_redir);`
  Appends a redirection to the `t_redir` linked list of a command.

* `void free_pipeline(t_pipeline *pipeline);`
  Frees all commands and redirections for a pipeline list.

**Syntax checking functions**

* `int validate_syntax(t_token *tokens, t_shell *shell);`
  Checks the global token sequence:

  * First token must not be a separator (`|`, `&&`, `||`, `;`) or redirection.
  * Forbids double redirections and invalid sequences (using helpers below).
  * For semicolon:

    * Forbids `;` followed by another separator or redirection.
    * Forbids a trailing `;` at the end of the line.
  * Last token must not be operator or redirection.
  * On error, prints a syntax error message and sets `g_shell.exit_status` to 258.

* `void print_syntax_error(t_token *token);`
  Prints the appropriate error message:

  * If `token == NULL`, prints `ERR_NEWLINE`.
  * If `token` is `TOKEN_SEMICOLON`, prints `ERR_SEMICOLON`.
  * If `token` is a redirection, prints the corresponding `ERR_REDIR_*`.
  * If `token` is `PIPE`, `AND`, `OR`, prints `ERR_PIPE`, `ERR_AND`, `ERR_OR`.

* `void print_run_error(t_token *t);`
  Prints error for invalid redirection sequences at runtime (e.g. `<<<` or `>>>`).

* `void print_unexpected(char *s);`
  Helper to print `minishell: syntax error near unexpected token 's'`.

* `int is_valid_word(t_token *token);`
  Returns non-zero if a token can be used where a command/filename is expected (usually `TOKEN_WORD`).

* `int is_control_operator(t_token *token);`
  Returns non-zero if the token is `PIPE`, `AND`, or `OR`.

* `int is_redirection(t_token *token);`
  Returns non-zero if token is a redirection type.

* `int is_gt(t_token *t);` / `int is_lt(t_token *t);`
  Helpers to recognize `<` or `>` style tokens.

* `int tok_op_len(t_token *t);`
  Returns length of the operator represented by token (e.g. `1` for `<`, `2` for `<<`).

---

## 6. Expander

The expander replaces `$VAR` and `$?`, and removes quotes, following shell rules.

**Functions**

* `void expander(t_pipeline *pipeline, t_env *env);`
  Entry point:

  * For each `t_pipeline` and each `t_cmd` inside:

    * Expands command arguments via `expand_cmd_args`.
    * Expands filenames in redirections via `expand_redirections`.

* `char *expand_variables(char *str, t_env *env, int exit_status);`
  Processes a single string:

  * Walks through characters with a context struct (`t_exp_ctx`).
  * When it sees `$`, calls `process_dollar`:

    * If `$?`, injects the last exit status.
    * Else, extracts a variable name and replaces with its value.
  * Preserves or ignores `$` depending on quotes.

* `void expand_arg(char **arg, t_env *env, int exit_status);`
  Expands and then unquotes a single argument:

  * Calls `expand_variables`.
  * Calls `remove_quotes`.
  * Frees the original string and updates `*arg`.

* `void expand_cmd_args(t_cmd *cmd, t_env *env, int exit_status);`
  Loops over `cmd->args` and calls `expand_arg` for each.

* `void expand_redirections(t_redir *redir, t_env *env, int exit_status);`
  For each redirection, expands the filename/delimiter string in `redir->file`.

* `void expand_pipeline_cmds(t_cmd *cmds, t_env *env, int exit_status);`
  Helper to expand a whole chain of commands in a pipeline.

* `char *get_env_value(t_env *env, char *key);`
  Searches the environment linked list and returns pointer to the value string for the given key, or `NULL` if not found.

* `void expand_exit_status(char *result, int *j, int exit_status);`
  Converts `exit_status` to string and appends it into the result buffer, updating position `*j`.

* `void expand_var_name(t_exp_ctx *ctx);`
  Extracts a variable name from the input string at `ctx->i`, looks it up in environment, and copies its value into `ctx->result`.

* `void process_dollar(t_exp_ctx *c);`
  Handles a `$` occurrence:

  * Checks whether it is escaped or inside single quotes.
  * If `$?`, calls `expand_exit_status`.
  * Otherwise, calls `expand_var_name`.

* `char *remove_quotes(char *str);`
  Produces a new string with shell quotes removed, following rules for single and double quotes.

---

## 7. Executor

The executor runs pipelines and commands, handling `;`, pipes, redirections, and builtins/external commands.

**Top-level**

* `void executor(t_pipeline *pipeline, t_shell *shell);`
  Walks the `t_pipeline` list:

  * For each pipeline:

    * Calls `execute_pipeline` to run its commands, updating `shell->exit_status`.
    * Checks `pipeline->logic_op`:

      * If `TOKEN_AND` and exit status is non-zero → stop executing further pipelines.
      * If `TOKEN_OR` and exit status is zero → stop executing further pipelines.
      * If `TOKEN_SEMICOLON` or no logic operator → always continue to the next pipeline.

* `int execute_pipeline(t_cmd *cmds, t_shell *shell);`
  Executes a chain of commands:

  * If there is only one command, tries to execute a builtin in the parent (no fork) via `execute_single_builtin_parent`.
  * If not a builtin or there are multiple commands, sets up a pipeline and runs commands via `execute_multi_pipeline`.

**Pipeline helpers**

* `int execute_pipeline_loop(t_cmd *cmds, t_shell *shell, pid_t *pids, int cmd_count);`
  Iterates over commands, for each:

  * Sets up pipes and redirections.
  * Forks a child (`create_child_process`).
  * Executes the command in the child.

* `int execute_single_builtin_parent(t_cmd *cmd, t_shell *shell);`
  If:

  * The command is a builtin.
  * And there are no redirections.
  * Then executes the builtin in the parent process and returns its status.
    If not applicable, returns `-1` so the caller can use the regular fork/exec path.

* `int wait_for_children(pid_t *pids, int count);`
  Waits for all child PIDs in the pipeline and returns the last exit status:

  * Uses `waitpid`.
  * If a process exited normally, extracts `WEXITSTATUS`.
  * If a process was terminated by a signal, returns `128 + signal_number`.

* `int execute_one_command(t_cmd *cmd, int index, t_pipe_ctx *ctx);`
  Called from the pipeline loop:

  * Sets up the right pipe ends for this command.
  * Forks if necessary.
  * For a child process, passes control to `execute_cmd_child`.

* `int init_pipeline(int cmd_count, pid_t **pids);`
  Allocates an array of `pid_t` for the number of commands.

* `int count_commands(t_cmd *cmds);`
  Counts nodes in a `t_cmd` linked list.

**External command execution and PATH logic**

* `void execute_commands(t_cmd *cmd, t_shell *shell);`
  Executes a single command in the child:

  * If builtin, runs it.
  * Otherwise, calls `execute_external`.

* `void execute_external(t_cmd *cmd, t_shell *shell);`
  Handles external (non-builtin) commands:

  * Builds `char **argv` from `cmd->args`.
  * Builds `char **envp` from environment list using `env_to_array`.
  * Resolves the command path via `resolve_command_path`.
  * Checks for errors and permissions.
  * Calls `execute_command` (which wraps `execve`).
  * Updates `shell->exit_status` based on errno (`126` for permission, `127` for not found).

* `void execute_command(char **argv, t_shell *shell, char **envp);`
  Calls `execve` with resolved path and environment array.

* `char *resolve_command_path(char *cmd, t_shell *shell);`
  Resolves the command path:

  * If `cmd` contains `/`, uses it as path.
  * Otherwise, searches through `$PATH` directories via `find_executable`.

* `int handle_path_errors(char *cmd, char *path, t_shell *shell);`
  If `path` is `NULL` or invalid, prints the appropriate error and sets `exit_status` (like “command not found”).

* `int check_execution_permission(char *cmd, char *path, t_shell *shell);`
  Uses `stat`/`access` to verify:

  * File exists.
  * It is not a directory.
  * It is executable by the user.
    On failure, prints messages and updates `exit_status`.

* `char *find_executable(char *cmd, t_env *env);`
  Uses `$PATH` from `env` to build a full path:

  * Iterates over `PATH` entries.
  * Joins directory and `cmd`.
  * Uses `is_exec_file` to find a valid candidate.

* `char *search_in_path(const char *path, const char *cmd);`
  Low-level helper that iterates over `path` string segments.

* `int is_exec_file(const char *path);`
  Returns non-zero if `path` points to a regular file with executable permission.

* `char *join_cmd_path(const char *dir, const char *cmd);`
  Builds a full path string “dir/cmd”.

* `size_t seg_end(const char *path, size_t start);` /
  `char *dup_segment_or_dot(const char *path, size_t start, size_t end);`
  Helpers for splitting `PATH` into segments.

* `void cmd_not_found(char *name);`
  Prints `minishell: <name>: command not found`.

**Redirections and heredoc**

* `int setup_redirections(t_redir *redirs);`
  For each redirection in the list:

  * Opens files.
  * Duplicates file descriptors onto stdin/stdout using `dup2`.
  * On error, prints message and returns non-zero.

* `int handle_heredoc(char *delimiter);`
  Implements `<<`:

  * Reads lines until the delimiter.
  * Writes them into a temporary pipe or file.
  * Returns a file descriptor to be used as stdin for the command.

* `int handle_input(char *file);`
  Opens `file` for reading and returns its fd (or error).

* `int handle_output(char *file, int append);`
  Opens `file` for writing (truncate or append) and returns its fd.

**Child processes and fds**

* `pid_t create_child_process(t_cmd *cmd, t_shell *shell, t_child_io *io);`
  Forks a child:

  * In the child:

    * Calls `setup_child_fds` to connect pipes.
    * Calls `setup_redirections`.
    * Executes `execute_cmd_child`.
  * Returns the child PID to the parent.

* `int setup_child_fds(int pipefd[2], int prev_read_fd, int has_next);`
  Configures stdin/stdout for one stage of the pipeline:

  * If there is a previous pipe, dup its read end to stdin.
  * If there is a next command, dup the current pipe write end to stdout.
  * Closes unnecessary fds.

**General executor helpers**

* `void print_error(const char *function, const char *message);`
  Prints standard error messages like: `minishell: <function>: <message>`.

* `void safe_close(int fd);`
  Calls `close(fd)` if `fd` is not `-1`.

* `void cleanup_pipe(int pipefd[2]);`
  Closes both ends of a pipe safely.

* `void perror_with_cmd(const char *cmd);`
  Calls `perror` with `cmd` prepended.

* `int is_directory(const char *path);` / `int has_slash(const char *s);`
  Helpers to detect directories and check if a command string contains `/`.

---

## 8. Builtins

**Dispatch and detection**

* `int is_builtin(char *cmd);`
  Returns non-zero if `cmd` is one of:

  * `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`, `history`.

* `int execute_builtin(t_cmd *cmd, t_shell *shell);`
  Based on `cmd->args[0]`, calls the corresponding builtin function and returns its exit code.

**Individual builtins**

* `int builtin_echo(char **args);`
  Implements `echo` with support for `-n` (no trailing newline) and prints the remaining arguments separated by spaces.

* `int builtin_cd(char **args, t_env **env);`
  Changes current directory:

  * Resolves target directory (path or `HOME` / `OLDPWD`).
  * Calls `chdir`.
  * Updates `PWD` and `OLDPWD` in environment via helper functions.

* `char *dup_cwd(void);`
  Returns a newly allocated string containing the current working directory.

* `char *resolve_target(char **args, t_env *env, int *print_after);`
  Decides where `cd` should go (`HOME`, `-`, or a path) and whether to print the new directory afterwards.

* `int builtin_pwd(void);`
  Prints the current working directory.

* `int builtin_export(char **args, t_env **env);`
  Adds or updates environment variables:

  * If called without arguments, prints environment in sorted form.
  * If called with `KEY=VALUE` or `KEY+=VALUE`, updates `*env`.

* `int builtin_unset(char **args, t_env **env);`
  Removes variables from `*env` list.

* `int builtin_env(t_env *env);`
  Prints environment variables in the `KEY=VALUE` format.

* `int builtin_exit(char **args, t_shell *shell);`
  Exits the shell:

  * Optional numeric argument for exit code.
  * Sets `shell->should_exit = 1`.
  * Updates `shell->exit_status`.

* `int builtin_history(char **args);`
  Prints the interactive history (via `readline` history API).

---

## 9. Environment Management

**Functions**

* `t_env *init_env(char **envp);`
  Builds the `t_env` linked list from the `envp` array passed to `main`.

* `void increment_shlvl(t_env **env);`
  Reads `SHLVL`, increments it, updates environment (like Bash).

* `void init_shell(t_shell *shell, char **envp);`
  Initializes `shell` structure:

  * Sets `shell->env` by calling `init_env`.
  * Sets `exit_status` to `0`, `should_exit` and `sigint_during_read` to `0`.
  * Sets `history_path` to `NULL`.
  * Calls `increment_shlvl`.

* `t_env *create_env_node(char *key, char *value);`
  Creates one environment node with given key/value.

* `void add_env_node(t_env **env, t_env *new_node);`
  Appends a new node to the env list.

* `void remove_env_node(t_env **env, char *key);`
  Removes a node with matching key.

* `char **env_to_array(t_env *env);`
  Produces a `char **` array in the format `KEY=VALUE`, suitable to pass to `execve`.

* `void parse_env_string(char *env_str, char **key, char **value);`
  Splits `"KEY=VALUE"` into `*key` and `*value`.

* `int append_env(char ***arr, size_t *n, const char *k, const char *v);`
  Appends a key/value pair to an array of strings, updating its length.

---

## 10. Signals and Terminal

**Functions**

* `void setup_signals(void);`
  Installs signal handlers:

  * `SIGINT` → `handle_sigint`.
  * `SIGQUIT` → `handle_sigquit`.

* `void handle_sigint(int sig);`
  For interactive mode:

  * Writes a newline.
  * Clears the current `readline` buffer.
  * Forces `readline` to return (`rl_done = 1`).
  * Sets `g_shell.exit_status = 130`.
  * Sets `g_shell.sigint_during_read = 1` so the reader knows a Ctrl-C happened.

* `void handle_sigquit(int sig);`
  Ignores `SIGQUIT` in the shell main loop.

* `void init_terminal(void);`
  Adjusts terminal settings:

  * Disables `ECHOCTL` so `Ctrl-C` and `Ctrl-D` are not printed as `^C` / `^D`.

---

## 11. History

**Functions**

* `int history_init(t_shell *shell);`
  Determines a history file path (usually in `$HOME`), stores it in `shell->history_path`, and calls `read_history` to load previous lines.

* `void history_add_line(const char *line);`
  Adds line to `readline` history:

  * Ignores blank lines.
  * Avoids duplicating consecutive identical entries (checks last history entry).

* `void history_save(t_shell *shell);`
  If `history_path` is set, writes the history to that file via `write_history`.

---

## 12. Utils

**Functions**

* `void free_array(char **arr);`
  Frees a `char **` array and its elements.

* `char *ft_strjoin_free(char *s1, char const *s2);`
  Joins `s1` and `s2` into a new string and frees `s1`.

* `void free_env(t_env *env);`
  Frees the entire environment linked list (keys, values, and nodes).

* `void print_logo(void);`
  Prints the UAE flag and the stylized `MINISHELL` logo in color at startup.

---

You can save this text as a Markdown file inside your repository, for example:

* `docs/minishell_v021_norminette_semicolone_works.md`
* or replace the existing `README.md` content if you want this to be your main documentation.

## ==================== Test ========================= ##

✅ Final Comprehensive Test Suite
Now that the basic tests work, let's test more advanced scenarios:

```bash
# Test qoute
echo "A\"B"            # A"B
echo "x\"\"y"          # x""y
echo "keep \\ back"    # keep \ back
echo "\$HOME"          # $HOME
echo "$HOME"           # expands
echo '$HOME'           # $HOME
echo 'A'\''B'          # A'B
echo "Test Hello"\n "hello"   # Test Hellon hello



#1. Complex Variable Expansion
minishell> export VAR1="Hello"
minishell> export VAR2="World"
minishell> echo $VAR1 $VAR2
Hello World

minishell> echo "$VAR1 $VAR2"
Hello World

minishell> echo '$VAR1 $VAR2'
$VAR1 $VAR2

minishell> echo $VAR1$VAR2
HelloWorld
#2. Exit Status Chain
minishell> ls
# ... files listed ...
minishell> echo $?
0

minishell> ls /nonexistent
ls: /nonexistent: No such file or directory
minishell> echo $?
1

minishell> echo $?
0

minishell> false
minishell> echo $?
1

minishell> true
minishell> echo $?
0

#3. SHLVL Nesting

# Terminal
$ echo $SHLVL
1

# First minishell
$ ./minishell
minishell> echo $SHLVL
2
minishell> env | grep SHLVL
SHLVL=2

# Second minishell (nested)
minishell> ./minishell

minishell> $SHLVL             # => minishell: 2: command not found

minishell> echo $SHLVL
3
minishell> env | grep SHLVL
SHLVL=3

# Exit back
minishell> exit
minishell> echo $SHLVL
2
minishell> exit
$ echo $SHLVL
1
4. Redirections Stress Test
Bash
# Create test files
minishell> echo "Test 1" > test1.txt
minishell> echo "Test 2" > test2.txt
minishell> echo "Test 3" > test3.txt

# Multiple input redirections
minishell> cat < test1.txt < test2.txt < test3.txt
Test 3

# Multiple output redirections
minishell> echo "Final" > out1.txt > out2.txt > out3.txt
minishell> cat out3.txt
Final
minishell> cat out1.txt 2>/dev/null || echo "out1.txt doesn't exist (correct)"
out1.txt doesn't exist (correct)


# Heredoc:
cat << END
hello
world
END

## ============================================================ ##

# Append redirections
minishell> echo "Line 1" > append.txt
minishell> echo "Line 2" >> append.txt
minishell> echo "Line 3" >> append.txt
minishell> cat append.txt
Line 1
Line 2
Line 3
5. Pipes with Redirections
Bash
minishell> echo "hello world" | cat > piped.txt
minishell> cat piped.txt
hello world

minishell> cat test1.txt | cat | cat | cat
Test 1

minishell> echo "test" | cat | cat > multi_pipe.txt
minishell> cat multi_pipe.txt
test
6. Builtin Commands
Bash
# pwd
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell

# cd
minishell> cd src
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell/src
minishell> cd ..
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell

# export
minishell> export TEST="value with spaces"
minishell> echo $TEST
value with spaces

# unset
minishell> unset TEST
minishell> echo $TEST

minishell> echo $?
0

# env
minishell> env | grep USER
USER=malmarzo

# echo with -n
minishell> echo -n "no newline"
no newlineminishell> 
minishell> echo "with newline"
with newline
7. Edge Cases
Bash
# Empty variable
minishell> echo $NONEXISTENT_VAR
<empty line>

# Dollar at end
minishell> echo test$
test$

# Multiple dollars
minishell> echo $$
<process id>

# Variable in middle of word
minishell> export NAME="John"
minishell> echo Hello$NAME!
HelloJohn!

# Empty command
minishell> 
minishell> 

# Only spaces
minishell>      
minishell> 
8. Quotes Edge Cases
Bash
minishell> echo "test"
test

minishell> echo 'test'
test

minishell> echo "test'test"
test'test

minishell> echo 'test"test'
test"test

minishell> echo "$USER's home is $HOME"
malmarzo's home is /Users/malmarzo

minishell> echo '$USER'"'"'s home is $HOME'
$USER's home is $HOME
9. Signal Handling
Bash
# Ctrl+C during command
minishell> sleep 5
^C
minishell> echo $?
130

# Ctrl+C at prompt
minishell> ^C
minishell> 

# Ctrl+D to exit
minishell> [Press Ctrl+D]
$
10. Memory Leak Test
If you're on Linux with valgrind:
Bash
$ valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell
minishell> echo $USER
malmarzo
minishell> echo $?
0
minishell> cat < test1.txt < test2.txt < test3.txt
Test 3
minishell> echo "test" | cat | cat
test
minishell> exit

# Check valgrind output - should show:
# "All heap blocks were freed -- no leaks are possible"
# Or minimal leaks from readline library (which is acceptable)
11. Performance Test
Bash
# Test with many redirections
minishell> cat < test1.txt < test2.txt < test3.txt < test1.txt < test2.txt
Test 2

# Long pipeline
minishell> echo "test" | cat | cat | cat | cat | cat | cat | cat | cat
test

# Many variables
minishell> export A=1 B=2 C=3 D=4 E=5
minishell> echo $A $B $C $D $E
1 2 3 4 5
12. Error Handling
Bash
# Command not found
minishell> nonexistent_command
minishell: nonexistent_command: command not found
minishell> echo $?
127

# Permission denied (if you have a file without execute permission)
minishell> ./no_execute_permission_file
minishell: ./no_execute_permission_file: Permission denied
minishell> echo $?
126

# Incomplete redirections (should NOT crash)
minishell> echo test >
minishell: syntax error near unexpected token `newline'
minishell> echo $?
2

minishell> cat <
minishell: syntax error near unexpected token `newline'
🎯 Summary of What's Fixed
✅ Bug #1: Parser segfault on incomplete redirections - FIXED

✅ Bug #2: Buffer overflow in variable expansion - FIXED

✅ Bug #3: Memory leak in redirection parsing - FIXED

✅ SHLVL: Correctly increments with nested shells - WORKING

✅ Variable expansion: $? and $VAR work correctly - WORKING

✅ Redirections: Multiple redirections handled properly - WORKING
📝 Next Steps (Optional Improvements)
If you want to make your minishell even better, consider:
Heredoc with variable expansion (if delimiter is unquoted)
Wildcards (* expansion)
Logical operators (&& and ||)
Subshells ((command))
Better error messages (more descriptive)


```bash
Minishell Test Suite (42 Subject Compliance)
1. Prompt and Basic Command Execution
1.1 Prompt and simple commands
./minishell
<press Enter>
echo hello


Expected:

Prompt always appears.

Empty line reprints prompt.

echo hello → hello.

# 1.2 Exit status
true
echo $?
false
echo $?
ls /this/path/does/not/exist
echo $?


Expected:
Correct exit statuses (0 for success, non-zero for errors).

# 2. History
# 2.1 Arrow key navigation
echo one
echo two
echo three
<Up arrow multiple times>


Expected:
three → two → one

# 3. PATH Resolution
# 3.1 PATH search
ls
which ls
echo $PATH


Expected:
ls works through PATH.

# 3.2 Relative path
cd /tmp
echo 'echo HI' > myscript.sh
chmod +x myscript.sh
./myscript.sh

# 3.3 Absolute path
/bin/echo ABSOLUTE

# 3.4 Command not found
thiscommanddoesnotexist


Expected:
minishell: ... command not found

# 4. Quotes and Special Characters
# 4.1 Single quotes
echo '$HOME'


Expected:
Literal $HOME.

# 4.2 Double quotes
echo "$HOME"

#4.3 Mixed quotes
echo "HOME=" $HOME
echo "HOME='$HOME'"
echo '$HOME'"$HOME"

# 4.4 Unclosed quotes
echo "hello

"

Expected:
Syntax error OR waiting for closing quote (your choice, must not execute partial command).

# 4.5 Semicolon and backslash
\ls
;
echo hi\there


Expected:
Consistent behavior (not required by subject).

# 5. Redirections
# 5.1 Output >
echo "hello" > out1
cat out1

# 5.2 Append >>
echo "line1" > out2
echo "line2" >> out2
cat out2

# 5.3 Input <
printf "line A\nline B\n" > in1
cat < in1

# 5.4 Combined < and >
cat < in1 > out3
cat out3

# 5.5 Here-document <<
# Unquoted delimiter
cat << EOF
line $HOME
EOF

# Quoted delimiter
cat << "EOF"
line $HOME
EOF


Expected:
Quoted delimiter → no variable expansion.

# 5.6 Redirection syntax errors
>
>>
<<<
<<
echo hi >

# 6. Pipes
# 6.1 Simple pipe
echo "hello" | cat

# 6.2 Multi-pipe
echo "test" | cat | cat | cat

# 6.3 Pipe heavy example
ls -1 | grep '\.c' | wc -l

# 6.4 Redirect + pipe
echo "abc" | cat > piped_out
cat piped_out

# 7. Environment Variables
# 7.1 Basic expansion
echo $HOME
echo "$HOME"
echo '$HOME'

# 7.2 Undefined variable
echo $DOES_NOT_EXIST

# 7.3 $?
false
echo $?
true
echo $?
ls /no/such/file
echo $?

# 7.4 Combining text
echo "status=$?"

# 8. Signals (Interactive Only)
# 8.1 Ctrl-C at prompt

Press Ctrl-C
Expected:
New prompt + exit code 130.

# 8.2 Ctrl-C during a command
sleep 10
press Ctrl-C

# 8.3 Ctrl-D
press Ctrl-D
Expected:
Exit minishell.

8.4 Ctrl-\
press Ctrl-\
Expected:
Do nothing.

# 9. Builtins
# 9.1 echo
echo hello world
echo -n hello
echo -n -n -n abc
echo "-n" hello

# 9.2 cd
pwd
cd ..
pwd
cd /
pwd
cd /no/such/dir
echo $?

# 9.3 pwd
pwd
/bin/pwd

# 9.4 export
export TESTVAR=hello
echo $TESTVAR
export TESTVAR=world
echo $TESTVAR
export ABC
env | grep ABC
export

# 9.5 unset
export TEMPVAR=123
echo $TEMPVAR
unset TEMPVAR
echo $TEMPVAR

# 9.6 env
env
env | grep HOME
env BADARG

# 9.7 exit
# Numeric:
exit 42

# Overflow:
exit 99999999999999999999 //diffrent result in bash

# Too many args:
exit 1 2

# 10. Non-required Features (Ensure no crash)
# 10.1 && and ||
true && echo yes
false || echo yes

# 10.2 Semicolon //diffrent result in bash
echo hi ; echo bye
;

# 11. How to Use This Test Plan

Run each test in bash.

Run the same in minishell.

Compare:

Output

Exit status (echo $?)

File contents

Ensure no crashes or memory leaks (your code only).

