# Pseudocode for Minishell

This document provides a comprehensive overview of the pseudocode for a minishell implementation. The minishell is a simplified version of a command-line interpreter like `bash` or `zsh`. The pseudocode is broken down into the core components of the shell.

_
## 1. Main Loop and Input Reading

The main loop is the heart of the shell. It continuously prompts the user for input, reads the command, and then executes it. This process repeats until the user exits the shell.

```plaintext
FUNCTION main_loop():
    WHILE true:
        display_prompt()  // e.g., "minishell> "
        line = read_line() // Read a line of input from the user

        IF line is empty THEN
            CONTINUE // If the user just presses Enter, show the prompt again
        ENDIF

        command = parse_command(line) // See section 2 for parsing
        execute_command(command)      // See section 3 for execution

        IF should_exit THEN // A global flag set by the 'exit' command
            BREAK
        ENDIF
    ENDWHILE

FUNCTION read_line():
    // This function reads a line of input from standard input.
    // It should handle various edge cases, such as EOF (Ctrl-D).
    buffer = read from stdin until newline or EOF
    IF EOF is detected THEN
        print "exit"
        set should_exit to true
    ENDIF
    RETURN buffer
```
## 2. Command Parsing

Parsing involves breaking the raw input string into a structured representation of the command. This includes identifying the command, its arguments, and any redirections or pipes.

```plaintext
FUNCTION parse_command(line):
    tokens = tokenize(line) // Split the line into tokens (words)
    command = create_command_structure()

    // The first token is the command
    command.name = tokens[0]

    // The rest are arguments
    command.arguments = tokens from index 1 to end

    // This is a simplified parsing. A real implementation would need to handle
    // quotes, environment variable expansion, and other shell syntax.

    RETURN command

FUNCTION tokenize(line):
    // This function splits a line into tokens, respecting quotes.
    tokens = []
    current_token = ""
    in_quotes = false
    quote_char = ''

    FOR each character in line:
        IF character is a space AND NOT in_quotes THEN
            IF current_token is not empty THEN
                add current_token to tokens
                current_token = ""
            ENDIF
        ELSE IF character is a quote (' or ") AND NOT in_quotes THEN
            in_quotes = true
            quote_char = character
        ELSE IF character is quote_char AND in_quotes THEN
            in_quotes = false
        ELSE
            append character to current_token
        ENDIF
    ENDFOR

    IF current_token is not empty THEN
        add current_token to tokens
    ENDIF

    RETURN tokens
```
## 3. Command Execution

Once the command is parsed, the shell needs to execute it. This is typically done by creating a new process using `fork()` and then running the command in the child process using `execve()`.

```plaintext
FUNCTION execute_command(command):
    // First, check if the command is a built-in
    IF is_builtin(command.name) THEN
        execute_builtin(command)
        RETURN
    ENDIF

    // If not a built-in, execute it as an external command
    pid = fork()

    IF pid < 0 THEN
        print error "Fork failed"
        RETURN
    ELSE IF pid == 0 THEN // Child process
        // The child process will execute the command.
        // execve replaces the current process image with a new one.
        path_to_executable = find_executable(command.name) // Search for the executable in the PATH

        IF path_to_executable is not found THEN
            print error "Command not found: " + command.name
            exit(127)
        ENDIF

        // The first argument to execve is the path to the executable.
        // The second is the array of arguments, where the first argument
        // is conventionally the name of the program itself.
        argv = [command.name] + command.arguments

        // The third argument is the environment variables.
        envp = get_environment_variables()

        execve(path_to_executable, argv, envp)

        // If execve returns, it must have failed.
        print error "Execve failed"
        exit(1)
    ELSE // Parent process
        // The parent process waits for the child to complete.
        waitpid(pid, &status, 0)
    ENDIF

FUNCTION find_executable(command_name):
    // This function searches for an executable in the directories specified by the PATH environment variable.
    path_env = getenv("PATH")
    directories = split path_env by ":"

    FOR each directory in directories:
        executable_path = directory + "/" + command_name
        IF file_exists(executable_path) AND is_executable(executable_path) THEN
            RETURN executable_path
        ENDIF
    ENDFOR

    RETURN null // Not found
```
## 4. Built-in Commands

Built-in commands are commands that are executed directly by the shell itself, without creating a new process. This is necessary for commands that need to modify the shell's internal state, such as `cd` and `exit`.

```plaintext
FUNCTION is_builtin(command_name):
    RETURN command_name is one of "cd", "echo", "pwd", "export", "unset", "env", "exit"

FUNCTION execute_builtin(command):
    IF command.name == "cd" THEN
        // Change the current working directory.
        // If no directory is specified, go to the home directory.
        directory = command.arguments[0] IF command.arguments is not empty ELSE getenv("HOME")
        IF chdir(directory) != 0 THEN
            print error "cd: No such file or directory: " + directory
        ENDIF
    ELSE IF command.name == "echo" THEN
        // Print the arguments to standard output.
        // Handle the -n flag to suppress the trailing newline.
        with_newline = true
        arguments_to_print = command.arguments
        IF command.arguments[0] == "-n" THEN
            with_newline = false
            arguments_to_print = command.arguments from index 1 to end
        ENDIF
        print join(arguments_to_print, " ")
        IF with_newline THEN
            print newline
        ENDIF
    ELSE IF command.name == "pwd" THEN
        // Print the current working directory.
        print getcwd()
    ELSE IF command.name == "export" THEN
        // Set an environment variable.
        // A simple version handles "KEY=VALUE". A more complex one would handle "KEY" and append to existing variables.
        FOR each argument in command.arguments:
            IF argument contains "=" THEN
                key = part before "="
                value = part after "="
                setenv(key, value, 1) // 1 means overwrite if it exists
            ENDIF
        ENDFOR
    ELSE IF command.name == "unset" THEN
        // Unset an environment variable.
        FOR each argument in command.arguments:
            unsetenv(argument)
        ENDFOR
    ELSE IF command.name == "env" THEN
        // Print the environment variables.
        FOR each variable in environment:
            print variable
        ENDFOR
    ELSE IF command.name == "exit" THEN
        // Exit the shell.
        set should_exit to true
    ENDIF
```
## 5. Handling Redirections

Redirections allow the user to change the standard input, standard output, and standard error of a command. This is a crucial feature for any shell.

```plaintext
// This logic needs to be integrated into the parsing and execution phases.

// --- In the parsing phase ---
FUNCTION parse_command(line):
    // ... (previous parsing logic)

    // Look for redirection operators (<, >, >>, <<)
    command.redirects = []
    i = 0
    WHILE i < length(tokens):
        IF tokens[i] is a redirection operator THEN
            redirect = create_redirect_structure()
            redirect.type = tokens[i]
            redirect.filename = tokens[i+1]
            add redirect to command.redirects

            // Remove the redirection operator and filename from the tokens
            // so they are not treated as arguments to the command.
            remove tokens[i] and tokens[i+1] from tokens
        ELSE
            i = i + 1
        ENDIF
    ENDWHILE

    // ... (rest of the parsing logic)
    RETURN command

// --- In the execution phase ---
FUNCTION execute_command(command):
    // ... (before forking)

    pid = fork()

    IF pid == 0 THEN // Child process
        // --- Redirection Handling ---
        // Before calling execve, set up the redirections.
        FOR each redirect in command.redirects:
            IF redirect.type == ">" THEN // Redirect standard output (overwrite)
                fd = open(redirect.filename, O_WRONLY | O_CREAT | O_TRUNC, 0644)
                dup2(fd, STDOUT_FILENO)
                close(fd)
            ELSE IF redirect.type == ">>" THEN // Redirect standard output (append)
                fd = open(redirect.filename, O_WRONLY | O_CREAT | O_APPEND, 0644)
                dup2(fd, STDOUT_FILENO)
                close(fd)
            ELSE IF redirect.type == "<" THEN // Redirect standard input
                fd = open(redirect.filename, O_RDONLY)
                dup2(fd, STDIN_FILENO)
                close(fd)
            ELSE IF redirect.type == "<<" THEN // Here document
                // Create a temporary pipe
                pipe_fds = pipe()
                // Write the here-doc content to the pipe
                write(pipe_fds[1], redirect.here_doc_content)
                close(pipe_fds[1])
                // Redirect stdin from the pipe
                dup2(pipe_fds[0], STDIN_FILENO)
                close(pipe_fds[0])
            ENDIF
        ENDFOR

        // ... (the rest of the child process logic, including execve)
    ELSE // Parent process
        // ... (wait for the child)
    ENDIF
```
## 6. Handling Pipes

Pipes allow the standard output of one command to be used as the standard input of another command. This is a powerful feature for chaining commands together.

```plaintext
// This logic requires a more advanced execution model that can handle a pipeline of commands.

// --- In the parsing phase ---
FUNCTION parse_command(line):
    // The parser should be able to split the line by the pipe `|` operator
    // to create a list of commands in a pipeline.
    pipeline = []
    command_strings = split line by "|"
    FOR each command_string in command_strings:
        command = parse_single_command(command_string) // This is the parser from before
        add command to pipeline
    ENDFOR
    RETURN pipeline

// --- In the execution phase ---
FUNCTION execute_pipeline(pipeline):
    num_commands = length(pipeline)
    IF num_commands == 1 THEN
        execute_command(pipeline[0]) // No pipes, just a single command
        RETURN
    ENDIF

    // For a pipeline, we need to create a pipe between each command.
    // For N commands, we need N-1 pipes.

    // `prev_pipe_read_end` will store the read end of the previous pipe.
    // For the first command, it's -1 (or some other indicator that there's no previous pipe).
    prev_pipe_read_end = -1

    FOR i = 0 to num_commands - 1:
        command = pipeline[i]

        // Create a new pipe for the current command to write to, unless it's the last command.
        is_last_command = (i == num_commands - 1)
        IF NOT is_last_command THEN
            pipe_fds = pipe()
        ENDIF

        pid = fork()

        IF pid == 0 THEN // Child process
            // --- Input Redirection ---
            // If there's a previous pipe, redirect stdin to its read end.
            IF prev_pipe_read_end != -1 THEN
                dup2(prev_pipe_read_end, STDIN_FILENO)
                close(prev_pipe_read_end)
            ENDIF

            // --- Output Redirection ---
            // If this is not the last command, redirect stdout to the write end of the new pipe.
            IF NOT is_last_command THEN
                close(pipe_fds[0]) // Close the unused read end in the child
                dup2(pipe_fds[1], STDOUT_FILENO)
                close(pipe_fds[1])
            ENDIF

            // Handle file redirections for the current command (e.g., `ls > out.txt | wc`)
            setup_redirections(command.redirects)

            // Now, execute the command.
            execute_single_command(command) // This is the execution logic from before
            exit(1) // Should not be reached if execve succeeds

        ELSE // Parent process
            // Close the write end of the new pipe in the parent, as it's only used by the child.
            IF NOT is_last_command THEN
                close(pipe_fds[1])
            ENDIF

            // If there was a previous pipe, close its read end in the parent, as it has been passed to the child.
            IF prev_pipe_read_end != -1 THEN
                close(prev_pipe_read_end)
            ENDIF

            // The read end of the current pipe becomes the `prev_pipe_read_end` for the next iteration.
            IF NOT is_last_command THEN
                prev_pipe_read_end = pipe_fds[0]
            ENDIF
    ENDFOR

    // After the loop, the parent waits for all child processes to complete.
    FOR i = 0 to num_commands - 1:
        wait(NULL)
    ENDFOR
```
## 7. Environment Variable Expansion

Environment variables like `$HOME` or `$PATH` need to be expanded to their values during parsing.

```plaintext
FUNCTION expand_variables(token):
    // This function takes a token and expands any environment variables in it.
    result = ""
    i = 0
    WHILE i < length(token):
        IF token[i] == '$' THEN
            // Extract the variable name
            var_name = ""
            i = i + 1
            WHILE i < length(token) AND (token[i] is alphanumeric or token[i] == '_'):
                var_name = var_name + token[i]
                i = i + 1
            ENDWHILE

            // Get the value of the variable
            var_value = getenv(var_name)
            IF var_value is null THEN
                var_value = ""
            ENDIF
            result = result + var_value
        ELSE
            result = result + token[i]
            i = i + 1
        ENDIF
    ENDWHILE
    RETURN result

// This function should be called during tokenization or after tokenization
// to expand variables in each token.
```

## 8. Signal Handling

The shell needs to handle signals like `Ctrl-C` (SIGINT) and `Ctrl-\` (SIGQUIT) appropriately.

```plaintext
FUNCTION setup_signals():
    // Set up signal handlers for the shell.
    // In the parent shell process, we want to ignore certain signals
    // or handle them in a specific way.

    // SIGINT (Ctrl-C): Should display a new prompt without terminating the shell.
    signal(SIGINT, handle_sigint)

    // SIGQUIT (Ctrl-\): Should be ignored in the shell.
    signal(SIGQUIT, SIG_IGN)

FUNCTION handle_sigint(signal):
    // When Ctrl-C is pressed, display a new prompt.
    print newline
    display_prompt()

// In the child process (when executing a command), the default signal
// handlers should be restored so that the command can be interrupted.
FUNCTION restore_default_signals():
    signal(SIGINT, SIG_DFL)
    signal(SIGQUIT, SIG_DFL)
```
## 9. Error Handling

Robust error handling is essential for a reliable shell. The shell should handle errors gracefully and provide informative error messages.

```plaintext
FUNCTION handle_error(error_type, context):
    // This function is called whenever an error occurs.
    // It prints an appropriate error message and may set the exit status.

    IF error_type == "command_not_found" THEN
        print "minishell: " + context + ": command not found"
        set_exit_status(127)
    ELSE IF error_type == "file_not_found" THEN
        print "minishell: " + context + ": No such file or directory"
        set_exit_status(1)
    ELSE IF error_type == "permission_denied" THEN
        print "minishell: " + context + ": Permission denied"
        set_exit_status(126)
    ELSE IF error_type == "fork_failed" THEN
        print "minishell: fork: Resource temporarily unavailable"
        set_exit_status(1)
    ELSE IF error_type == "pipe_failed" THEN
        print "minishell: pipe: Resource temporarily unavailable"
        set_exit_status(1)
    ELSE
        print "minishell: An error occurred: " + context
        set_exit_status(1)
    ENDIF

FUNCTION set_exit_status(status):
    // The exit status of the last command is stored in a global variable.
    // It can be accessed via the special variable $?.
    global_exit_status = status
```

## 10. Summary

This pseudocode provides a comprehensive overview of the key components of a minishell implementation. The main components include:

1. **Main Loop and Input Reading**: The shell continuously prompts the user for input and reads commands.
2. **Command Parsing**: The raw input is tokenized and parsed into a structured command representation.
3. **Command Execution**: Commands are executed by forking a new process and using `execve()` to run the program.
4. **Built-in Commands**: Certain commands like `cd`, `echo`, `pwd`, `export`, `unset`, `env`, and `exit` are executed directly by the shell.
5. **Redirections**: The shell supports input and output redirections using `<`, `>`, `>>`, and `<<`.
6. **Pipes**: The shell supports pipelines, allowing the output of one command to be used as the input of another.
7. **Environment Variable Expansion**: Environment variables are expanded to their values during parsing.
8. **Signal Handling**: The shell handles signals like `SIGINT` and `SIGQUIT` appropriately.
9. **Error Handling**: The shell provides informative error messages and sets appropriate exit statuses.

This pseudocode serves as a blueprint for implementing a functional minishell. The actual implementation would require translating this pseudocode into a programming language like C, handling edge cases, and ensuring compliance with the shell's specification.

---

**Author**: Manus AI  
**Date**: November 2, 2025
