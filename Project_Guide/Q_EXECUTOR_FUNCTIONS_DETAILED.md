# Executor Functions - Comprehensive Documentation

This document provides detailed explanations for all functions in the executor module of the minishell project.

---

## Table of Contents

1. [executor.c](#executorc)
   - [find_executable](#find_executable)
   - [execute_external](#execute_external)
   - [execute_builtin_with_redir](#execute_builtin_with_redir)
   - [execute_command](#execute_command)

2. [executor_pipeline.c](#executor_pipelinec)
   - [count_commands](#count_commands)
   - [exec_pipe_cmd](#exec_pipe_cmd)
   - [execute_pipeline](#execute_pipeline)
   - [executor](#executor)

3. [redirections.c](#redirectionsc)
   - [handle_heredoc](#handle_heredoc)
   - [handle_input](#handle_input)
   - [handle_output](#handle_output)
   - [setup_redirections](#setup_redirections)

---

## executor.c

### find_executable

**Function Signature:**
```c
char *find_executable(char *cmd, t_env *env)
```

**Purpose:**  
Locate an executable file in the system PATH or validate an absolute/relative path.

**Parameters:**
- `cmd`: The command name to search for (e.g., "ls" or "/bin/ls")
- `env`: Linked list of environment variables containing PATH

**Return Value:**
- Allocated string with full path to executable
- NULL if command is not found or invalid
- NULL if cmd is NULL or empty
- Duplicated cmd if it contains '/' (absolute/relative path)
- Full path from PATH if found and executable

**Detailed Description:**

This function implements the command lookup logic used by shells to find executables. It follows this algorithm:

1. **Validation**: Check if cmd is NULL or empty string, return NULL if so

2. **Path Detection**: If the command contains a '/' character, it's treated as an absolute or relative path:
   - Absolute path: `/bin/ls`
   - Relative path: `./program` or `../bin/command`
   - In this case, return a duplicate of the path without searching PATH

3. **PATH Lookup**: If no '/' is present, search the PATH environment variable:
   - Get PATH value from environment (e.g., "/usr/bin:/bin:/usr/local/bin")
   - Split PATH by ':' delimiter into array of directories
   - For each directory:
     - Construct full path: directory + "/" + cmd
     - Check if file exists and is executable using `access(path, X_OK)`
     - If yes, free the paths array and return the full path
     - If no, free this path and try next directory
   - If not found in any directory, free paths array and return NULL

**Memory Management:**
- Allocates memory for the returned path (caller must free)
- Internally allocates and frees the paths array from ft_split
- Frees unsuccessful path attempts during search

**Examples:**
```c
// Absolute path
find_executable("/bin/ls", env);        // Returns: strdup("/bin/ls")

// Relative path
find_executable("./my_program", env);   // Returns: strdup("./my_program")

// Command name
find_executable("ls", env);             // Returns: "/bin/ls" (if found in PATH)

// Not found
find_executable("nonexistent", env);    // Returns: NULL
```

**Edge Cases:**
- Empty command: Returns NULL
- PATH not set: Returns NULL
- Command with '/' but doesn't exist: Returns the path anyway (execve will fail later)
- Multiple matches in PATH: Returns first match

---

### execute_external

**Function Signature:**
```c
static void execute_external(t_cmd *cmd, t_shell *shell)
```

**Purpose:**  
Execute an external (non-builtin) command by forking a child process and using execve.

**Parameters:**
- `cmd`: Command structure containing args and redirections
- `shell`: Shell state structure containing environment and exit status

**Return Value:**  
void (updates `shell->exit_status`)

**Detailed Description:**

This function handles the execution of external commands (programs not built into the shell). It implements the fork-exec pattern:

1. **Find Executable**:
   - Call `find_executable()` to locate the command
   - If not found:
     - Print error: "minishell: <cmd>: command not found"
     - Set exit status to 127 (standard "command not found" code)
     - Return without forking

2. **Fork Process**:
   - Call `fork()` to create a child process
   - Parent and child now run concurrently

3. **Child Process** (pid == 0):
   - Set up redirections using `setup_redirections()`
   - If redirections fail, exit with status 1
   - Convert environment list to array format using `env_to_array()`
   - Call `execve(path, args, envp)` to replace process with the command
   - If execve returns (only happens on error), exit with status 1

4. **Parent Process** (pid != 0):
   - Wait for child process using `waitpid()`
   - Extract exit status using `WIFEXITED()` and `WEXITSTATUS()` macros
   - Update shell's exit_status with child's exit code
   - Free the allocated path string

**Exit Status Codes:**
- 0: Success
- 1-125: Command-specific error
- 126: Command found but not executable
- 127: Command not found
- 128+n: Terminated by signal n

**Process Flow Diagram:**
```
Parent Shell
    |
    ├─ find_executable() ──> path or NULL
    |
    ├─ fork() ──┬─> Child Process
    |           │   ├─ setup_redirections()
    |           │   ├─ env_to_array()
    |           │   ├─ execve(path, args, envp)
    |           │   └─ exit(1) [if execve fails]
    |           │
    |           └─> Parent Process
    |               ├─ waitpid(child)
    |               ├─ capture exit status
    |               └─ update shell->exit_status
    |
    └─ free(path)
```

**Examples:**
```c
// Execute ls -la
t_cmd cmd;
cmd.args = {"ls", "-la", NULL};
cmd.redirs = NULL;
execute_external(&cmd, shell);
// Child runs: execve("/bin/ls", {"ls", "-la", NULL}, envp)

// Execute with redirection
cmd.args = {"cat", "file.txt", NULL};
cmd.redirs = [output redirection to "output.txt"];
execute_external(&cmd, shell);
// Child sets up redirection, then runs cat
```

**Error Handling:**
- Command not found: Prints error, sets exit_status = 127
- Fork failure: Not explicitly handled (would return -1)
- Redirection failure: Child exits with status 1
- Execve failure: Child exits with status 1

---

### execute_builtin_with_redir

**Function Signature:**
```c
static void execute_builtin_with_redir(t_cmd *cmd, t_shell *shell)
```

**Purpose:**  
Execute a builtin command that has redirections by forking to preserve parent shell's file descriptors.

**Parameters:**
- `cmd`: Command structure with builtin name, args, and redirections
- `shell`: Shell state structure

**Return Value:**  
void (updates `shell->exit_status`)

**Detailed Description:**

This function solves a critical problem: **How do we apply redirections to builtin commands without affecting the parent shell?**

**The Problem:**
- Redirections modify file descriptors (stdin, stdout, stderr)
- If we apply redirections in the parent shell, subsequent commands would be affected
- Example: If `echo hello > file.txt` redirects stdout in parent, the next prompt wouldn't display!

**The Solution:**
- Fork a child process
- Apply redirections in the child (child's FDs are independent)
- Execute builtin in the child
- Parent waits and captures exit status

**Process Flow:**

1. **Fork**:
   - Create child process with `fork()`
   - Parent and child have separate file descriptor tables

2. **Child Process**:
   - Call `setup_redirections()` to modify stdin/stdout/stderr
   - Call `execute_builtin()` to run the builtin
   - Exit with builtin's return value

3. **Parent Process**:
   - Wait for child using `waitpid()`
   - Extract exit status
   - Update shell's exit_status

**Why Fork for Builtins with Redirections?**

Without forking:
```c
// BAD: Don't do this
setup_redirections(cmd->redirs);  // Redirects parent's stdout
execute_builtin(cmd, shell);      // Builtin output goes to file
// Now parent's stdout is still redirected! Next prompt won't show!
```

With forking:
```c
// GOOD: Fork first
pid = fork();
if (pid == 0) {
    setup_redirections(cmd->redirs);  // Only child's stdout redirected
    exit(execute_builtin(cmd, shell));
}
waitpid(pid, &status, 0);  // Parent's stdout unchanged
```

**Comparison with execute_builtin_without_redir:**

| Aspect | With Redirections | Without Redirections |
|--------|------------------|---------------------|
| Fork? | Yes | No |
| Why? | Preserve parent FDs | Need to modify parent state |
| Examples | `echo hello > file` | `cd /tmp`, `export VAR=val` |
| Exit status | From child process | Direct return value |

**Examples:**

```c
// echo with output redirection
t_cmd cmd;
cmd.args = {"echo", "hello", NULL};
cmd.redirs = [redirection to "output.txt"];
execute_builtin_with_redir(&cmd, shell);
// Forks, child redirects stdout to output.txt, runs echo, exits
// Parent's stdout unchanged

// pwd with redirection
cmd.args = {"pwd", NULL};
cmd.redirs = [redirection to "current_dir.txt"];
execute_builtin_with_redir(&cmd, shell);
// Current directory written to file, not terminal
```

**Special Cases:**

Some builtins should NOT be forked even with redirections (not in this implementation):
- `cd`: Must change parent's working directory
- `export`: Must modify parent's environment
- `unset`: Must modify parent's environment

However, this implementation forks for ALL builtins with redirections, which means:
- `cd /tmp > file` won't actually change the parent's directory
- This is a known limitation/design choice

---

### execute_command

**Function Signature:**
```c
void execute_command(t_cmd *cmd, t_shell *shell)
```

**Purpose:**  
Main command execution dispatcher that routes commands to builtin or external execution.

**Parameters:**
- `cmd`: Command structure containing:
  - `args`: NULL-terminated array of arguments (args[0] is command name)
  - `redirs`: Linked list of redirections (can be NULL)
- `shell`: Shell state structure containing environment and exit status

**Return Value:**  
void (updates `shell->exit_status`)

**Detailed Description:**

This is the primary entry point for executing a single command. It acts as a dispatcher that determines the execution strategy based on:
1. Whether the command is a builtin
2. Whether redirections are present

**Execution Decision Tree:**

```
execute_command()
    |
    ├─ Validate command
    |   └─ Return if NULL or no args
    |
    ├─ is_builtin(cmd->args[0])?
    |   |
    |   ├─ YES (Builtin)
    |   |   |
    |   |   ├─ Has redirections?
    |   |   |   |
    |   |   |   ├─ YES → execute_builtin_with_redir()
    |   |   |   |         (Fork to preserve parent FDs)
    |   |   |   |
    |   |   |   └─ NO → execute_builtin() directly
    |   |   |            (Run in parent to modify state)
    |   |   |
    |   |
    |   └─ NO (External)
    |       └─ execute_external()
    |          (Always fork and execve)
```

**Validation:**
- Checks if cmd is NULL
- Checks if cmd->args is NULL
- Checks if cmd->args[0] is NULL (no command name)
- Returns early if any validation fails

**Builtin Execution Strategy:**

The function makes an important distinction for builtins:

**Without Redirections:**
- Execute directly in parent process
- Allows builtins to modify parent state
- Examples:
  - `cd /tmp` - Changes parent's working directory
  - `export VAR=value` - Modifies parent's environment
  - `exit` - Exits the parent shell

**With Redirections:**
- Fork and execute in child process
- Prevents redirections from affecting parent
- Examples:
  - `echo hello > file.txt` - Output to file, parent stdout unchanged
  - `pwd > dir.txt` - Write to file, parent stdout unchanged

**External Command Execution:**
- Always uses `execute_external()`
- Always forks (external commands can't run in parent)
- Always uses execve to replace child process

**Flow Examples:**

Example 1: `ls -la` (external, no redirections)
```
execute_command()
  → is_builtin("ls")? NO
  → execute_external()
    → fork() + execve("/bin/ls", ...)
```

Example 2: `cd /tmp` (builtin, no redirections)
```
execute_command()
  → is_builtin("cd")? YES
  → Has redirections? NO
  → execute_builtin() in parent
    → chdir("/tmp") in parent process
```

Example 3: `echo hello > file.txt` (builtin, with redirections)
```
execute_command()
  → is_builtin("echo")? YES
  → Has redirections? YES
  → execute_builtin_with_redir()
    → fork()
    → child: redirect stdout → file.txt
    → child: execute_builtin()
    → parent: wait and capture exit status
```

Example 4: `cat < input.txt > output.txt` (external, with redirections)
```
execute_command()
  → is_builtin("cat")? NO
  → execute_external()
    → fork()
    → child: setup_redirections()
    → child: execve("/bin/cat", ...)
```

**Why This Design?**

The dual execution strategy for builtins is necessary because:

1. **State Modification Builtins** (cd, export, unset, exit):
   - MUST run in parent to affect shell state
   - Cannot fork or changes would be lost

2. **Output Builtins** (echo, pwd, env):
   - CAN run in parent normally
   - MUST fork if redirections present (to preserve parent FDs)

This design ensures:
- Builtins can modify shell state when needed
- Redirections don't corrupt the parent shell's I/O
- External commands always run in isolated child processes

---

## executor_pipeline.c

### count_commands

**Function Signature:**
```c
static int count_commands(t_cmd *cmds)
```

**Purpose:**  
Count the number of commands in a linked list.

**Parameters:**
- `cmds`: Head of the command linked list

**Return Value:**  
Number of commands in the list (0 if cmds is NULL)

**Detailed Description:**

This is a simple utility function that traverses a linked list of commands and counts them. The count is used for:

1. **Optimization**: If count == 1, skip pipe creation entirely
2. **Resource Allocation**: Know how many pipes to create (count - 1)
3. **Process Management**: Know how many child processes to wait for

**Algorithm:**
```c
count = 0
while (cmds != NULL):
    count++
    cmds = cmds->next
return count
```

**Examples:**
```c
// Single command: "ls"
count_commands(cmd_list);  // Returns: 1

// Pipeline: "ls | grep txt"
count_commands(cmd_list);  // Returns: 2

// Long pipeline: "cat file | grep pattern | sort | uniq"
count_commands(cmd_list);  // Returns: 4

// Empty list
count_commands(NULL);      // Returns: 0
```

**Usage in Pipeline Execution:**
```c
cmd_count = count_commands(cmd);
if (cmd_count == 1) {
    // Optimization: no pipes needed
    execute_command(cmd, shell);
    return;
}
// Otherwise, create (cmd_count - 1) pipes
```

---

### exec_pipe_cmd

**Function Signature:**
```c
static void exec_pipe_cmd(t_cmd *cmd, int *prev_fd, int *pipe_fd, t_shell *shell)
```

**Purpose:**  
Execute a single command within a pipeline with proper pipe I/O setup.

**Parameters:**
- `cmd`: Command to execute
- `prev_fd`: File descriptors from previous pipe [read_end, write_end]
  - `prev_fd[0]` = -1 if this is the first command
- `pipe_fd`: File descriptors for next pipe [read_end, write_end]
  - `pipe_fd[1]` = -1 if this is the last command
- `shell`: Shell state for environment and builtins

**Return Value:**  
Never returns (always calls `exit()`)

**Detailed Description:**

This function is the heart of pipeline execution. It runs in a child process and sets up the "plumbing" to connect commands via pipes.

**Pipeline Concept:**

In a pipeline like `cmd1 | cmd2 | cmd3`:
- cmd1's stdout → pipe1 → cmd2's stdin
- cmd2's stdout → pipe2 → cmd3's stdin
- cmd3's stdout → terminal (or file if redirected)

**I/O Setup Process:**

1. **Input from Previous Command**:
   ```c
   if (prev_fd[0] != -1) {
       dup2(prev_fd[0], STDIN_FILENO);  // Redirect stdin to read from prev pipe
       close(prev_fd[0]);                // Close original FD
   }
   ```
   - If not the first command, read from previous pipe
   - `dup2()` makes stdin (FD 0) point to the pipe's read end
   - Close the original FD (no longer needed)

2. **Output to Next Command**:
   ```c
   if (pipe_fd[1] != -1) {
       close(pipe_fd[0]);                // Close read end (not needed in this process)
       dup2(pipe_fd[1], STDOUT_FILENO);  // Redirect stdout to write to next pipe
       close(pipe_fd[1]);                // Close original FD
   }
   ```
   - If not the last command, write to next pipe
   - Close read end (this process only writes)
   - `dup2()` makes stdout (FD 1) point to the pipe's write end

3. **File Redirections**:
   ```c
   setup_redirections(cmd->redirs);
   ```
   - Applied AFTER pipe setup
   - Can override pipe I/O
   - Example: `cmd1 | cmd2 > file` redirects cmd2's output to file, not pipe

4. **Command Execution**:
   - If builtin: `exit(execute_builtin(cmd, shell))`
   - If external: `execve(path, args, envp)`

**Pipe File Descriptor States:**

For a 3-command pipeline `cmd1 | cmd2 | cmd3`:

| Command | prev_fd[0] | pipe_fd[1] | stdin source | stdout destination |
|---------|-----------|-----------|--------------|-------------------|
| cmd1    | -1        | pipe1[1]  | terminal     | pipe1 → cmd2      |
| cmd2    | pipe1[0]  | pipe2[1]  | pipe1 ← cmd1 | pipe2 → cmd3      |
| cmd3    | pipe2[0]  | -1        | pipe2 ← cmd2 | terminal          |

**Example Execution:**

Pipeline: `ls -la | grep txt | wc -l`

**cmd1 (ls -la)**:
```c
prev_fd[0] = -1        // First command, no previous pipe
pipe_fd[1] = pipe1[1]  // Write to pipe1

// Setup:
// prev_fd[0] == -1, so stdin stays as terminal
dup2(pipe1[1], STDOUT_FILENO);  // stdout → pipe1
close(pipe1[0]);                 // Don't need read end
close(pipe1[1]);                 // Close after dup2

// Execute:
execve("/bin/ls", {"ls", "-la", NULL}, envp);
// ls output goes to pipe1
```

**cmd2 (grep txt)**:
```c
prev_fd[0] = pipe1[0]  // Read from pipe1
pipe_fd[1] = pipe2[1]  // Write to pipe2

// Setup:
dup2(pipe1[0], STDIN_FILENO);   // stdin ← pipe1
close(pipe1[0]);
dup2(pipe2[1], STDOUT_FILENO);  // stdout → pipe2
close(pipe2[0]);
close(pipe2[1]);

// Execute:
execve("/bin/grep", {"grep", "txt", NULL}, envp);
// Reads from pipe1 (ls output), writes to pipe2
```

**cmd3 (wc -l)**:
```c
prev_fd[0] = pipe2[0]  // Read from pipe2
pipe_fd[1] = -1        // Last command, no next pipe

// Setup:
dup2(pipe2[0], STDIN_FILENO);   // stdin ← pipe2
close(pipe2[0]);
// pipe_fd[1] == -1, so stdout stays as terminal

// Execute:
execve("/usr/bin/wc", {"wc", "-l", NULL}, envp);
// Reads from pipe2 (grep output), writes to terminal
```

**Why This Function Never Returns:**

This function is called in a child process and must either:
- `exit()` after builtin execution
- `execve()` which replaces the process image

If `execve()` returns, it means it failed, so we `exit(1)`.

**Error Handling:**
- Redirection failure: `exit(1)`
- Command not found: `exit(127)`
- Execve failure: `exit(1)`

---

### execute_pipeline

**Function Signature:**
```c
void execute_pipeline(t_pipeline *pipeline, t_shell *shell)
```

**Purpose:**  
Execute a pipeline of commands connected by pipes, managing process creation and pipe coordination.

**Parameters:**
- `pipeline`: Pipeline structure containing linked list of commands
- `shell`: Shell state structure

**Return Value:**  
void (updates `shell->exit_status` with last command's exit status)

**Detailed Description:**

This function orchestrates the execution of a complete pipeline. It handles:
- Creating pipes between commands
- Forking child processes for each command
- Managing file descriptors to prevent leaks
- Waiting for all processes to complete
- Capturing the exit status of the last command

**Execution Algorithm:**

```
1. Count commands in pipeline
2. If only 1 command:
   - Use execute_command() (no pipes needed)
   - Return

3. Initialize:
   - prev_fd = {-1, -1}  // No previous pipe initially
   - i = 0               // Command counter

4. For each command:
   a. If not last command:
      - Create pipe: pipe(pipe_fd)
   b. Else:
      - Set pipe_fd = {-1, -1}  // No next pipe

   c. Fork child process

   d. In child:
      - Call exec_pipe_cmd() with prev_fd and pipe_fd
      - Never returns

   e. In parent:
      - Close prev_fd[0] if valid (done reading from previous pipe)
      - Close pipe_fd[1] if valid (done writing to current pipe)
      - Save pipe_fd[0] as prev_fd[0] for next iteration
      - Move to next command
      - Increment counter

5. Wait for all child processes (i times)

6. Capture exit status of last process
```

**Pipe Management:**

The key to understanding this function is tracking the pipe file descriptors:

```
Iteration 1 (cmd1):
  pipe_fd = pipe(new)     // Create pipe1
  fork()
    child: uses pipe_fd[1] for stdout
    parent: closes pipe_fd[1], saves pipe_fd[0]
  prev_fd[0] = pipe_fd[0]

Iteration 2 (cmd2):
  prev_fd[0] = pipe1[0]   // From previous iteration
  pipe_fd = pipe(new)     // Create pipe2
  fork()
    child: uses prev_fd[0] for stdin, pipe_fd[1] for stdout
    parent: closes prev_fd[0], closes pipe_fd[1], saves pipe_fd[0]
  prev_fd[0] = pipe_fd[0]

Iteration 3 (cmd3):
  prev_fd[0] = pipe2[0]   // From previous iteration
  pipe_fd = {-1, -1}      // Last command, no pipe
  fork()
    child: uses prev_fd[0] for stdin, stdout stays terminal
    parent: closes prev_fd[0]
```

**File Descriptor Lifecycle:**

For pipeline `cmd1 | cmd2 | cmd3`:

| FD | Created | Used by Child | Closed by Parent | Purpose |
|----|---------|--------------|------------------|---------|
| pipe1[0] | Before cmd1 fork | cmd2 (stdin) | After cmd2 fork | cmd1 → cmd2 |
| pipe1[1] | Before cmd1 fork | cmd1 (stdout) | After cmd1 fork | cmd1 → cmd2 |
| pipe2[0] | Before cmd2 fork | cmd3 (stdin) | After cmd3 fork | cmd2 → cmd3 |
| pipe2[1] | Before cmd2 fork | cmd2 (stdout) | After cmd2 fork | cmd2 → cmd3 |

**Why Close FDs in Parent?**

Critical for preventing:
1. **File descriptor leaks**: Each pipe creates 2 FDs, must close when done
2. **Deadlocks**: If parent keeps write end open, child waiting for EOF won't finish
3. **Resource exhaustion**: System has limited FDs (typically 1024 per process)

**Process Synchronization:**

```c
while (i-- > 0)
    wait(&status);
```

- Waits for all child processes
- Order doesn't matter (any child can finish first)
- Last `wait()` captures the final status
- Exit status of pipeline = exit status of last command (bash behavior)

**Example Execution:**

Pipeline: `cat file.txt | grep error | wc -l`

```
Timeline:

T0: Parent creates pipe1
T1: Parent forks child1 (cat)
    - child1: stdin=terminal, stdout=pipe1[1]
    - parent: closes pipe1[1], saves pipe1[0]

T2: Parent creates pipe2
T3: Parent forks child2 (grep)
    - child2: stdin=pipe1[0], stdout=pipe2[1]
    - parent: closes pipe1[0], closes pipe2[1], saves pipe2[0]

T4: Parent sets pipe_fd={-1,-1} (last command)
T5: Parent forks child3 (wc)
    - child3: stdin=pipe2[0], stdout=terminal
    - parent: closes pipe2[0]

T6: Parent waits for 3 children
T7: All children finish, parent captures wc's exit status
```

**Optimization for Single Command:**

```c
if (cmd_count == 1) {
    execute_command(cmd, shell);
    return;
}
```

- Avoids pipe creation overhead
- Directly executes single command
- Example: `ls -la` (no pipe needed)

**Exit Status Handling:**

```c
if (WIFEXITED(status))
    shell->exit_status = WEXITSTATUS(status);
```

- `WIFEXITED()`: Check if process exited normally
- `WEXITSTATUS()`: Extract exit code
- Only last command's status is captured (bash behavior)
- Example: `false | true` has exit status 0 (from `true`)

---

### executor

**Function Signature:**
```c
void executor(t_pipeline *pipeline, t_shell *shell)
```

**Purpose:**  
Main executor function that handles pipelines connected by logical operators (&& and ||).

**Parameters:**
- `pipeline`: Linked list of pipelines connected by logical operators
  - Each pipeline has a `logic_op` field (TOKEN_AND, TOKEN_OR, or TOKEN_EOF)
- `shell`: Shell state structure

**Return Value:**  
void (updates `shell->exit_status`)

**Detailed Description:**

This is the top-level executor that implements **short-circuit evaluation** for logical operators. It's a BONUS feature that extends the mandatory pipeline execution.

**Logical Operators:**

**AND Operator (&&)**:
- Execute first pipeline
- If exit status != 0 (failure), **stop** execution
- If exit status == 0 (success), **continue** to next pipeline
- Example: `make && ./program`
  - Only runs `./program` if `make` succeeds
  - If make fails, program doesn't run

**OR Operator (||)**:
- Execute first pipeline
- If exit status == 0 (success), **stop** execution
- If exit status != 0 (failure), **continue** to next pipeline
- Example: `test -f file || touch file`
  - Only runs `touch file` if `test -f file` fails
  - If file exists, touch doesn't run

**Execution Algorithm:**

```
while (pipeline != NULL):
    execute_pipeline(pipeline, shell)
    
    if (pipeline->logic_op == TOKEN_AND):
        if (shell->exit_status != 0):
            break  // AND failed, stop execution
    
    if (pipeline->logic_op == TOKEN_OR):
        if (shell->exit_status == 0):
            break  // OR succeeded, stop execution
    
    pipeline = pipeline->next
```

**Truth Tables:**

**AND (&&)**:
| First Command | Second Command | Executes Second? |
|--------------|----------------|------------------|
| Success (0)  | -              | Yes              |
| Failure (!0) | -              | No               |

**OR (||)**:
| First Command | Second Command | Executes Second? |
|--------------|----------------|------------------|
| Success (0)  | -              | No               |
| Failure (!0) | -              | Yes              |

**Examples:**

**Example 1: AND chain**
```bash
mkdir temp && cd temp && touch file.txt
```

Execution:
```
1. execute_pipeline("mkdir temp")
   - exit_status = 0 (success)
   - logic_op = TOKEN_AND
   - 0 != 0? No, continue

2. execute_pipeline("cd temp")
   - exit_status = 0 (success)
   - logic_op = TOKEN_AND
   - 0 != 0? No, continue

3. execute_pipeline("touch file.txt")
   - exit_status = 0 (success)
   - logic_op = TOKEN_EOF (end)
   - Done

Result: All three commands executed
```

**Example 2: AND with failure**
```bash
mkdir /root/test && echo "success"
```

Execution:
```
1. execute_pipeline("mkdir /root/test")
   - exit_status = 1 (permission denied)
   - logic_op = TOKEN_AND
   - 1 != 0? Yes, BREAK

Result: "echo success" never executes
```

**Example 3: OR chain**
```bash
command1 || command2 || command3
```

Execution:
```
1. execute_pipeline("command1")
   - exit_status = 1 (failure)
   - logic_op = TOKEN_OR
   - 1 == 0? No, continue

2. execute_pipeline("command2")
   - exit_status = 0 (success)
   - logic_op = TOKEN_OR
   - 0 == 0? Yes, BREAK

Result: command3 never executes (command2 succeeded)
```

**Example 4: Mixed operators**
```bash
cmd1 && cmd2 || cmd3
```

Execution:
```
Scenario A: cmd1 succeeds, cmd2 succeeds
1. cmd1: exit=0, op=AND, 0!=0? No, continue
2. cmd2: exit=0, op=OR, 0==0? Yes, BREAK
Result: cmd3 doesn't run

Scenario B: cmd1 succeeds, cmd2 fails
1. cmd1: exit=0, op=AND, 0!=0? No, continue
2. cmd2: exit=1, op=OR, 1==0? No, continue
3. cmd3: executes
Result: cmd3 runs

Scenario C: cmd1 fails
1. cmd1: exit=1, op=AND, 1!=0? Yes, BREAK
Result: cmd2 and cmd3 don't run
```

**Pipeline Structure:**

```c
typedef struct s_pipeline {
    t_cmd       *cmds;      // Linked list of commands in this pipeline
    t_token_type logic_op;  // TOKEN_AND, TOKEN_OR, or TOKEN_EOF
    struct s_pipeline *next; // Next pipeline in chain
} t_pipeline;
```

Example for `cmd1 | cmd2 && cmd3 || cmd4`:
```
pipeline1:
  cmds: [cmd1, cmd2]
  logic_op: TOKEN_AND
  next: pipeline2

pipeline2:
  cmds: [cmd3]
  logic_op: TOKEN_OR
  next: pipeline3

pipeline3:
  cmds: [cmd4]
  logic_op: TOKEN_EOF
  next: NULL
```

**Exit Status Propagation:**

The final exit status is the exit status of the **last executed pipeline**:

```bash
true && false && echo "never runs"
# exit_status = 1 (from false)

false || true
# exit_status = 0 (from true)

false || false || true
# exit_status = 0 (from true)
```

**Comparison with Bash:**

This implementation matches bash behavior for:
- Short-circuit evaluation
- Exit status propagation
- Operator precedence (left-to-right)

**Note on Bonus:**

This feature is part of the BONUS requirements. It should only be evaluated if the mandatory part is PERFECT. The mandatory part only requires:
- Single pipelines (no logical operators)
- Example: `cmd1 | cmd2 | cmd3`

---

## redirections.c

### handle_heredoc

**Function Signature:**
```c
static int handle_heredoc(char *delimiter)
```

**Purpose:**  
Handle heredoc input redirection (<<) by reading multi-line input until a delimiter is encountered.

**Parameters:**
- `delimiter`: String that marks the end of heredoc input

**Return Value:**
- 0 on success
- -1 if pipe creation fails

**Detailed Description:**

Heredoc (here document) is a special type of input redirection that allows multi-line input directly in the command line. It reads lines until a specific delimiter is encountered.

**Syntax:**
```bash
command << DELIMITER
line 1
line 2
line 3
DELIMITER
```

**Implementation:**

1. **Create Pipe**:
   ```c
   pipe(pipe_fd);
   ```
   - Creates a pipe to store heredoc content
   - pipe_fd[0] = read end
   - pipe_fd[1] = write end

2. **Read Lines**:
   ```c
   while (1) {
       line = readline("> ");
       if (!line || strcmp(line, delimiter) == 0)
           break;
       write(pipe_fd[1], line, strlen(line));
       write(pipe_fd[1], "\n", 1);
       free(line);
   }
   ```
   - Prompt with "> " (heredoc prompt)
   - Read line using readline()
   - Stop if:
     - readline returns NULL (Ctrl-D pressed)
     - Line matches delimiter exactly
   - Write each line to pipe with newline
   - Free the line

3. **Setup Input**:
   ```c
   close(pipe_fd[1]);              // Done writing
   dup2(pipe_fd[0], STDIN_FILENO); // Redirect stdin to pipe
   close(pipe_fd[0]);              // Close original FD
   ```
   - Close write end (no more input)
   - Redirect stdin to read from pipe
   - Command will read from pipe instead of terminal

**Example Usage:**

```bash
cat << EOF
Hello World
This is a heredoc
Multiple lines
EOF
```

Execution:
```
1. User types: cat << EOF
2. Shell calls handle_heredoc("EOF")
3. Prompt changes to "> "
4. User types:
   > Hello World
   > This is a heredoc
   > Multiple lines
   > EOF
5. Heredoc stops at "EOF"
6. All lines written to pipe
7. stdin redirected to pipe
8. cat reads from pipe and displays content
```

**Important Notes:**

**1. History Handling:**
According to the 42 subject, heredoc input should NOT be added to history:
```c
line = readline("> ");  // Uses readline but...
// DON'T call add_history(line)
```

**2. Delimiter Matching:**
```c
if (strcmp(line, delimiter) == 0)
```
- Exact match required
- No variable expansion in delimiter
- Case-sensitive
- No leading/trailing spaces

**3. Variable Expansion:**
In bash, heredoc supports variable expansion:
```bash
cat << EOF
User: $USER
Home: $HOME
EOF
```

This implementation does NOT expand variables in heredoc (simplification). To add this feature, you would need to call the expander on each line before writing to the pipe.

**4. Ctrl-D Handling:**
```c
if (!line)  // readline returns NULL on Ctrl-D
    break;
```
- If user presses Ctrl-D before delimiter, heredoc stops
- This matches bash behavior (warning: here-document delimited by end-of-file)

**Error Handling:**

```c
if (pipe(pipe_fd) == -1)
    return (-1);
```
- Only error checked: pipe creation failure
- Rare but possible if system runs out of file descriptors

**Memory Management:**
- Each line is allocated by readline()
- Each line must be freed after writing to pipe
- Delimiter line is also freed

**Example Scenarios:**

**Scenario 1: Normal heredoc**
```bash
wc -l << END
line 1
line 2
line 3
END
```
Output: `3` (three lines)

**Scenario 2: Empty heredoc**
```bash
cat << EOF
EOF
```
Output: (nothing)

**Scenario 3: Heredoc with special characters**
```bash
grep "error" << DELIM
no error here
this has error in it
DELIM
```
Output: `this has error in it`

**Scenario 4: Ctrl-D before delimiter**
```bash
cat << EOF
line 1
line 2
^D
```
Behavior: Stops reading, uses what was entered

---

### handle_input

**Function Signature:**
```c
static int handle_input(char *file)
```

**Purpose:**  
Handle input redirection from a file (<) by opening the file and redirecting stdin.

**Parameters:**
- `file`: Path to the file to read from

**Return Value:**
- 0 on success
- -1 if file cannot be opened

**Detailed Description:**

This function implements the `<` input redirection operator. It makes a command read its input from a file instead of the terminal.

**Implementation:**

1. **Open File**:
   ```c
   fd = open(file, O_RDONLY);
   ```
   - O_RDONLY: Open for reading only
   - Returns file descriptor (>= 0) on success
   - Returns -1 on failure

2. **Error Handling**:
   ```c
   if (fd == -1) {
       ft_putstr_fd("minishell: ", 2);
       ft_putstr_fd(file, 2);
       ft_putendl_fd(": No such file or directory", 2);
       return (-1);
   }
   ```
   - Print error message to stderr (fd 2)
   - Format: "minishell: <filename>: No such file or directory"
   - Return -1 to indicate failure

3. **Redirect stdin**:
   ```c
   dup2(fd, STDIN_FILENO);
   ```
   - Makes stdin (fd 0) point to the file
   - Now any read from stdin reads from the file
   - Original stdin (terminal) is no longer accessible

4. **Close Original FD**:
   ```c
   close(fd);
   ```
   - After dup2, we have two FDs pointing to the file
   - Close the original to prevent FD leak
   - stdin still works (it's a duplicate)

**Example Usage:**

```bash
cat < input.txt
```

Execution:
```
1. Open "input.txt" for reading → fd = 3
2. dup2(3, 0) → stdin now reads from input.txt
3. close(3) → only stdin (fd 0) remains
4. cat reads from stdin → actually reads from input.txt
5. cat outputs file contents to stdout
```

**Error Cases:**

**Case 1: File doesn't exist**
```bash
cat < nonexistent.txt
```
Output:
```
minishell: nonexistent.txt: No such file or directory
```
Exit status: 1 (command doesn't execute)

**Case 2: No read permission**
```bash
cat < /root/secret.txt
```
Output:
```
minishell: /root/secret.txt: No such file or directory
```
Note: open() returns -1 for both "not found" and "permission denied"

**Case 3: Directory instead of file**
```bash
cat < /tmp
```
Output:
```
minishell: /tmp: No such file or directory
```
Note: open() fails because directories can't be opened with O_RDONLY for reading

**Multiple Input Redirections:**

```bash
cat < file1.txt < file2.txt
```

Behavior:
```
1. handle_input("file1.txt")
   - stdin → file1.txt
2. handle_input("file2.txt")
   - stdin → file2.txt (overrides previous)
3. cat reads from file2.txt only
```

Last redirection wins (bash behavior).

**Combination with Output Redirection:**

```bash
sort < unsorted.txt > sorted.txt
```

Execution:
```
1. handle_input("unsorted.txt")
   - stdin → unsorted.txt
2. handle_output("sorted.txt", 0)
   - stdout → sorted.txt
3. sort reads from unsorted.txt, writes to sorted.txt
```

**File Descriptor State:**

Before redirection:
```
FD 0 (stdin):  terminal input
FD 1 (stdout): terminal output
FD 2 (stderr): terminal error
```

After `cat < file.txt`:
```
FD 0 (stdin):  file.txt
FD 1 (stdout): terminal output
FD 2 (stderr): terminal error
```

**Why dup2 Instead of Direct Assignment?**

Can't do this:
```c
STDIN_FILENO = fd;  // ERROR: Can't assign to constant
```

Must use dup2:
```c
dup2(fd, STDIN_FILENO);  // Correct: Duplicates FD
```

dup2 atomically:
1. Closes STDIN_FILENO if it's open
2. Makes STDIN_FILENO point to the same file as fd
3. Returns STDIN_FILENO

---

### handle_output

**Function Signature:**
```c
static int handle_output(char *file, int append)
```

**Purpose:**  
Handle output redirection to a file (> or >>) by opening the file and redirecting stdout.

**Parameters:**
- `file`: Path to the output file
- `append`: 1 for append mode (>>), 0 for truncate mode (>)

**Return Value:**
- 0 on success
- -1 if file cannot be opened/created

**Detailed Description:**

This function implements both `>` (truncate) and `>>` (append) output redirection operators.

**Truncate Mode (>):**
- Creates file if it doesn't exist
- Empties file if it exists
- Writes from the beginning
- Example: `echo "hello" > file.txt`

**Append Mode (>>):**
- Creates file if it doesn't exist
- Preserves existing content
- Writes at the end
- Example: `echo "world" >> file.txt`

**Implementation:**

1. **Set Flags**:
   ```c
   if (append)
       flags = O_WRONLY | O_CREAT | O_APPEND;
   else
       flags = O_WRONLY | O_CREAT | O_TRUNC;
   ```
   
   Flag meanings:
   - `O_WRONLY`: Open for writing only
   - `O_CREAT`: Create file if it doesn't exist
   - `O_TRUNC`: Truncate (empty) file if it exists
   - `O_APPEND`: Write at end of file

2. **Open File**:
   ```c
   fd = open(file, flags, 0644);
   ```
   - 0644 = rw-r--r-- (owner can read/write, others can read)
   - Returns file descriptor on success
   - Returns -1 on failure

3. **Error Handling**:
   ```c
   if (fd == -1) {
       ft_putstr_fd("minishell: ", 2);
       ft_putstr_fd(file, 2);
       ft_putendl_fd(": Permission denied", 2);
       return (-1);
   }
   ```
   - Print error to stderr
   - Format: "minishell: <filename>: Permission denied"
   - Return -1

4. **Redirect stdout**:
   ```c
   dup2(fd, STDOUT_FILENO);
   ```
   - Makes stdout (fd 1) point to the file
   - Now any write to stdout goes to the file

5. **Close Original FD**:
   ```c
   close(fd);
   ```
   - Prevent FD leak
   - stdout still works

**Example Usage:**

**Example 1: Truncate mode**
```bash
echo "first" > file.txt
echo "second" > file.txt
cat file.txt
```
Output:
```
second
```
Explanation: Second echo overwrites first

**Example 2: Append mode**
```bash
echo "first" > file.txt
echo "second" >> file.txt
cat file.txt
```
Output:
```
first
second
```
Explanation: Second echo appends to file

**Example 3: Create new file**
```bash
ls -la > listing.txt
```
Result: Creates listing.txt with directory contents

**Example 4: Permission denied**
```bash
echo "test" > /root/file.txt
```
Output:
```
minishell: /root/file.txt: Permission denied
```

**Multiple Output Redirections:**

```bash
echo "hello" > file1.txt > file2.txt
```

Behavior:
```
1. handle_output("file1.txt", 0)
   - stdout → file1.txt
2. handle_output("file2.txt", 0)
   - stdout → file2.txt (overrides)
3. echo writes to file2.txt only
4. file1.txt is created but empty
```

**File Permissions:**

Created files have mode 0644:
```
Owner:  rw- (read + write)
Group:  r-- (read only)
Others: r-- (read only)
```

To change, modify the open() call:
```c
fd = open(file, flags, 0600);  // rw------- (owner only)
fd = open(file, flags, 0666);  // rw-rw-rw- (all can read/write)
```

**Truncate vs Append Comparison:**

| Aspect | Truncate (>) | Append (>>) |
|--------|-------------|-------------|
| File exists | Empties it | Preserves content |
| File doesn't exist | Creates it | Creates it |
| Write position | Beginning | End |
| Previous content | Lost | Kept |
| Use case | Replace | Add to |

**Error Cases:**

**Case 1: No write permission**
```bash
echo "test" > /etc/passwd
```
Error: Permission denied

**Case 2: Directory**
```bash
echo "test" > /tmp/
```
Error: Permission denied (can't write to directory)

**Case 3: Read-only filesystem**
```bash
echo "test" > /mnt/cdrom/file.txt
```
Error: Permission denied

**Combination with Input Redirection:**

```bash
cat < input.txt > output.txt
```

Execution:
```
1. handle_input("input.txt")
   - stdin → input.txt
2. handle_output("output.txt", 0)
   - stdout → output.txt
3. cat reads from input.txt, writes to output.txt
```

Result: Copies input.txt to output.txt

---

### setup_redirections

**Function Signature:**
```c
int setup_redirections(t_redir *redirs)
```

**Purpose:**  
Set up all redirections for a command by processing a linked list of redirection structures.

**Parameters:**
- `redirs`: Linked list of redirection structures
  - Each contains `type` (TOKEN_REDIR_*) and `file` path

**Return Value:**
- 0 if all redirections succeed
- -1 if any redirection fails

**Detailed Description:**

This function is the main entry point for redirection handling. It processes all redirections in order, which is important because later redirections can override earlier ones.

**Redirection Types:**

```c
typedef enum e_token_type {
    TOKEN_REDIR_IN,      // <
    TOKEN_REDIR_OUT,     // >
    TOKEN_REDIR_APPEND,  // >>
    TOKEN_REDIR_HEREDOC, // <<
    // ... other tokens
} t_token_type;
```

**Algorithm:**

```
while (redirs != NULL):
    switch (redirs->type):
        case TOKEN_REDIR_IN:
            if handle_input(redirs->file) fails:
                return -1
        
        case TOKEN_REDIR_OUT:
            if handle_output(redirs->file, 0) fails:
                return -1
        
        case TOKEN_REDIR_APPEND:
            if handle_output(redirs->file, 1) fails:
                return -1
        
        case TOKEN_REDIR_HEREDOC:
            if handle_heredoc(redirs->file) fails:
                return -1
    
    redirs = redirs->next

return 0
```

**Order Matters:**

The order of redirections is significant because later redirections override earlier ones.

**Example 1: Multiple input redirections**
```bash
cat < file1.txt < file2.txt
```

Redirection list:
```
redir1: type=TOKEN_REDIR_IN, file="file1.txt"
redir2: type=TOKEN_REDIR_IN, file="file2.txt"
```

Execution:
```
1. handle_input("file1.txt") → stdin = file1.txt
2. handle_input("file2.txt") → stdin = file2.txt (overrides)
Result: cat reads from file2.txt
```

**Example 2: Multiple output redirections**
```bash
echo "hello" > file1.txt > file2.txt
```

Redirection list:
```
redir1: type=TOKEN_REDIR_OUT, file="file1.txt"
redir2: type=TOKEN_REDIR_OUT, file="file2.txt"
```

Execution:
```
1. handle_output("file1.txt", 0) → stdout = file1.txt
2. handle_output("file2.txt", 0) → stdout = file2.txt (overrides)
Result: "hello" written to file2.txt, file1.txt is empty
```

**Example 3: Mixed redirections**
```bash
sort < unsorted.txt > sorted.txt 2> errors.txt
```

Note: This implementation doesn't handle stderr redirection (2>), but the principle is the same.

**Example 4: Heredoc with output**
```bash
cat << EOF > output.txt
line 1
line 2
EOF
```

Redirection list:
```
redir1: type=TOKEN_REDIR_HEREDOC, file="EOF"
redir2: type=TOKEN_REDIR_OUT, file="output.txt"
```

Execution:
```
1. handle_heredoc("EOF")
   - Read lines until "EOF"
   - stdin → pipe with heredoc content
2. handle_output("output.txt", 0)
   - stdout → output.txt
Result: Heredoc content written to output.txt
```

**Error Handling:**

If any redirection fails, the function immediately returns -1:

```bash
cat < nonexistent.txt > output.txt
```

Execution:
```
1. handle_input("nonexistent.txt")
   - File not found
   - Prints error
   - Returns -1
2. setup_redirections() returns -1
3. Command execution aborts
4. output.txt is NOT created
```

This is correct behavior: if input fails, don't execute the command.

**Redirection Structure:**

```c
typedef struct s_redir {
    t_token_type  type;   // Type of redirection
    char          *file;  // Filename or heredoc delimiter
    struct s_redir *next; // Next redirection
} t_redir;
```

**When is this function called?**

This function is called in child processes before executing commands:

```c
// In exec_pipe_cmd():
if (setup_redirections(cmd->redirs) == -1)
    exit(1);  // Abort child process

// In execute_external():
if (pid == 0) {
    if (setup_redirections(cmd->redirs) == -1)
        exit(1);
    execve(...);
}
```

**Why in child process?**

Redirections modify file descriptors (stdin, stdout, stderr). If we did this in the parent shell:
- The parent's FDs would be modified
- Subsequent commands would be affected
- The shell prompt might disappear!

By doing it in a child process:
- Only the child's FDs are modified
- Parent remains unaffected
- Each command has clean FDs

**Complex Example:**

```bash
cat < input.txt | grep "error" > output.txt 2>&1 | wc -l
```

For `grep "error"`:
```
Redirections:
1. Input from pipe (not a file redirection)
2. Output to output.txt (TOKEN_REDIR_OUT)

setup_redirections() handles:
- handle_output("output.txt", 0)

Pipe setup (separate):
- stdin from previous pipe
- stdout to next pipe (overridden by redirection)
```

**Return Value Usage:**

```c
int result = setup_redirections(cmd->redirs);
if (result == -1) {
    // Redirection failed
    // Don't execute command
    exit(1);  // In child process
}
// Otherwise, continue with command execution
```

**Summary:**

`setup_redirections()` is the orchestrator that:
1. Processes all redirections in order
2. Delegates to specific handlers
3. Stops on first error
4. Returns success/failure status
5. Must be called in child process to preserve parent's FDs

---

## Summary

The executor module consists of 12 functions across 3 files:

**executor.c** (4 functions):
- `find_executable`: Locate commands in PATH
- `execute_external`: Fork and execve external commands
- `execute_builtin_with_redir`: Fork and execute builtins with redirections
- `execute_command`: Main dispatcher for command execution

**executor_pipeline.c** (4 functions):
- `count_commands`: Count commands in pipeline
- `exec_pipe_cmd`: Execute command with pipe I/O setup
- `execute_pipeline`: Orchestrate pipeline execution
- `executor`: Handle logical operators (&& and ||)

**redirections.c** (4 functions):
- `handle_heredoc`: Process << heredoc input
- `handle_input`: Process < input redirection
- `handle_output`: Process > and >> output redirection
- `setup_redirections`: Orchestrate all redirections

These functions work together to implement a complete command execution system with:
- PATH-based command lookup
- Process management (fork/exec/wait)
- Pipe creation and management
- File descriptor manipulation
- Input/output redirection
- Heredoc support
- Logical operator short-circuiting

---

**End of Documentation**
