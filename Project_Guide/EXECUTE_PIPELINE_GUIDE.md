# Complete execute_pipeline Implementation Guide

**Production-Ready Code with Comprehensive Error Handling**

---

## Overview

This document provides a **complete, production-ready implementation** of the `execute_pipeline` function with comprehensive error handling for all system calls.

**File:** `EXECUTE_PIPELINE_COMPLETE.c`

**Features:**
- ✅ Complete error handling for `fork`, `pipe`, `dup2`, `execve`
- ✅ Proper file descriptor management
- ✅ Memory leak prevention
- ✅ Zombie process prevention
- ✅ Two versions: standard and Norminette-compliant
- ✅ Comprehensive documentation
- ✅ Usage examples
- ✅ Testing guide

---

## Implementation Versions

### Version 1: Standard Implementation

**Function:** `execute_pipeline()`

**Characteristics:**
- Clear, readable code
- Single main function with helper functions
- Comprehensive error handling
- May exceed 25-line limit for Norminette

**Best for:**
- Understanding the algorithm
- Non-42 projects
- Code reviews and learning

### Version 2: Norminette-Compliant

**Function:** `execute_pipeline_v2()`

**Characteristics:**
- Split into multiple small functions
- Each function ≤ 25 lines
- Same functionality as Version 1
- Complies with Norminette rules

**Best for:**
- 42 School submission
- Strict coding standards
- Norminette compliance

---

## Function Overview

### Main Functions

#### 1. `execute_pipeline()`
```c
int execute_pipeline(t_cmd *cmds, t_shell *shell);
```

**Purpose:** Execute a pipeline of commands

**Parameters:**
- `cmds`: Linked list of commands
- `shell`: Shell state with environment

**Return:** Exit status of last command

**Process:**
1. Count commands
2. Allocate pid array
3. For each command:
   - Create pipe (if not last)
   - Fork child
   - Setup file descriptors
   - Execute command
4. Wait for all children
5. Return exit status

---

#### 2. `execute_pipeline_v2()`
```c
int execute_pipeline_v2(t_cmd *cmds, t_shell *shell);
```

**Purpose:** Same as `execute_pipeline()` but Norminette-compliant

**Difference:** Logic split into smaller helper functions

---

### Helper Functions

#### 1. `print_error()`
```c
static void print_error(const char *function, const char *message);
```

**Purpose:** Print formatted error messages

**Example:**
```c
print_error("fork", strerror(errno));
// Output: minishell: fork: Resource temporarily unavailable
```

---

#### 2. `safe_close()`
```c
static void safe_close(int fd);
```

**Purpose:** Safely close file descriptor with error checking

**Features:**
- Checks if fd is valid (>= 0)
- Prints error if close fails
- Safe to call multiple times

---

#### 3. `cleanup_pipe()`
```c
static void cleanup_pipe(int pipefd[2]);
```

**Purpose:** Close both ends of a pipe

**Usage:**
```c
int pipefd[2];
pipe(pipefd);
// ... error occurred ...
cleanup_pipe(pipefd);  // Clean up
```

---

#### 4. `setup_child_pipe_fds()`
```c
static int setup_child_pipe_fds(int pipefd[2], int is_first, 
                                int is_last, int prev_read_fd);
```

**Purpose:** Set up file descriptors for a child in pipeline

**Parameters:**
- `pipefd`: Current pipe file descriptors
- `is_first`: 1 if first command
- `is_last`: 1 if last command
- `prev_read_fd`: Read end of previous pipe

**Logic:**
```
If not first command:
    dup2(prev_read_fd, STDIN_FILENO)  // Read from previous pipe
    close(prev_read_fd)

If not last command:
    close(pipefd[0])                   // Don't need read end
    dup2(pipefd[1], STDOUT_FILENO)    // Write to next pipe
    close(pipefd[1])
```

---

#### 5. `execute_single_command()`
```c
static void execute_single_command(t_cmd *cmd, t_shell *shell);
```

**Purpose:** Execute a single command (called in child process)

**Process:**
1. Setup redirections
2. Check if builtin
3. If builtin: execute and exit
4. If external: find executable
5. Execute with execve
6. Exit with appropriate code

**Exit Codes:**
- 0: Success
- 1: Redirection error
- 126: Permission denied
- 127: Command not found

---

#### 6. `fork_and_execute()`
```c
static pid_t fork_and_execute(t_cmd *cmd, t_shell *shell, 
                              int pipefd[2], int is_first, 
                              int is_last, int prev_read_fd);
```

**Purpose:** Fork and execute a command

**Return:** Child PID on success, -1 on error

**Process:**
1. Fork
2. In child: Setup fds and execute
3. In parent: Return child PID

---

#### 7. `wait_for_children()`
```c
static int wait_for_children(pid_t *pids, int count);
```

**Purpose:** Wait for all children and get exit status

**Return:** Exit status of last command

**Features:**
- Waits for all children
- Handles both normal exit and signals
- Returns exit code of last command
- Prevents zombie processes

---

#### 8. `count_commands()`
```c
static int count_commands(t_cmd *cmds);
```

**Purpose:** Count commands in linked list

**Usage:**
```c
int count = count_commands(cmds);
pid_t *pids = malloc(sizeof(pid_t) * count);
```

---

## Error Handling

### 1. Malloc Failure

**Check:**
```c
pids = malloc(sizeof(pid_t) * cmd_count);
if (!pids)
{
    print_error("malloc", "failed to allocate pid array");
    return (1);
}
```

**Action:**
- Print error message
- Return error code
- No cleanup needed (nothing allocated yet)

---

### 2. Pipe Failure

**Check:**
```c
if (pipe(pipefd) == -1)
{
    print_error("pipe", strerror(errno));
    safe_close(prev_read_fd);
    free(pids);
    return (1);
}
```

**Action:**
- Print error with errno
- Close previous pipe read end
- Free pid array
- Return error code

**Common Causes:**
- Too many open files
- System resource exhaustion

---

### 3. Fork Failure

**Check:**
```c
pid = fork();
if (pid == -1)
{
    print_error("fork", strerror(errno));
    return (-1);
}
```

**Action:**
- Print error with errno
- Clean up pipes
- Free memory
- Return error code

**Common Causes:**
- Process limit reached
- Out of memory
- System resource exhaustion

---

### 4. Dup2 Failure

**Check:**
```c
if (dup2(pipefd[1], STDOUT_FILENO) == -1)
{
    print_error("dup2", "failed to redirect stdout");
    return (-1);
}
```

**Action:**
- Print error message
- Exit child process with code 1
- Parent continues normally

**Common Causes:**
- Invalid file descriptor
- File descriptor limit reached

---

### 5. Execve Failure

**Check:**
```c
if (execve(path, cmd->args, shell->env) == -1)
{
    print_error("execve", strerror(errno));
    free(path);
    exit(126);
}
```

**Action:**
- Print error with errno
- Free allocated memory
- Exit with code 126 (permission denied)

**Common Causes:**
- File not executable
- Permission denied
- Invalid format

---

### 6. Command Not Found

**Check:**
```c
path = find_executable(cmd->args[0], shell->env);
if (!path)
{
    ft_putstr_fd("minishell: ", STDERR_FILENO);
    ft_putstr_fd(cmd->args[0], STDERR_FILENO);
    ft_putstr_fd(": command not found\n", STDERR_FILENO);
    exit(127);
}
```

**Action:**
- Print "command not found" message
- Exit with code 127 (standard for command not found)

---

### 7. Waitpid Failure

**Check:**
```c
if (waitpid(pids[i], &status, 0) == -1)
{
    print_error("waitpid", strerror(errno));
}
```

**Action:**
- Print error
- Continue waiting for other children
- Don't stop execution

**Common Causes:**
- Child already reaped
- Invalid PID

---

### 8. Close Failure

**Check:**
```c
if (close(fd) == -1)
    print_error("close", strerror(errno));
```

**Action:**
- Print error
- Continue execution
- Usually not critical

---

## File Descriptor Management

### Tracking File Descriptors

**Throughout Pipeline Execution:**

```
Initial State (Parent):
  FD 0: stdin
  FD 1: stdout
  FD 2: stderr

After pipe():
  FD 0: stdin
  FD 1: stdout
  FD 2: stderr
  FD 3: pipe read end
  FD 4: pipe write end

After fork (Child 1):
  FD 0: stdin
  FD 1: stdout → pipe write end (after dup2)
  FD 2: stderr
  FD 3: CLOSED
  FD 4: CLOSED

After fork (Parent):
  FD 0: stdin
  FD 1: stdout
  FD 2: stderr
  FD 3: pipe read end
  FD 4: CLOSED

After second fork (Child 2):
  FD 0: stdin → pipe read end (after dup2)
  FD 1: stdout
  FD 2: stderr
  FD 3: CLOSED

Final State (Parent):
  FD 0: stdin
  FD 1: stdout
  FD 2: stderr
  (All pipe fds closed)
```

### Rules for FD Management

**Rule 1: Close unused pipe ends**
- Each process should only keep the pipe end it needs
- Close read end if you only write
- Close write end if you only read

**Rule 2: Close after dup2**
- After dup2, the original fd is duplicated
- Close the original to avoid fd leaks

**Rule 3: Parent closes both ends**
- After forking, parent closes both pipe ends
- Children inherit open fds, so they have their own copies

**Rule 4: Close in order**
- Always dup2 first, then close
- Closing before dup2 will fail

---

## Usage Examples

### Example 1: Two-Command Pipeline

**Command:** `ls | grep .c`

**Setup:**
```c
// Create command structures
t_cmd cmd2 = {
    .args = (char *[]){"grep", ".c", NULL},
    .redirs = NULL,
    .next = NULL
};

t_cmd cmd1 = {
    .args = (char *[]){"ls", NULL},
    .redirs = NULL,
    .next = &cmd2
};

// Execute pipeline
int status = execute_pipeline(&cmd1, shell);
```

**Execution Flow:**
```
1. Count commands: 2
2. Allocate pids[2]
3. Create pipe (FD 3, 4)
4. Fork child 1 (ls):
   - Close FD 3 (read end)
   - dup2(FD 4, FD 1) - stdout → pipe
   - Close FD 4
   - execve("/bin/ls")
5. Parent closes FD 4 (write end)
6. Fork child 2 (grep):
   - dup2(FD 3, FD 0) - stdin → pipe
   - Close FD 3
   - execve("/usr/bin/grep")
7. Parent closes FD 3 (read end)
8. Wait for both children
9. Return exit status
```

---

### Example 2: Three-Command Pipeline

**Command:** `cat file.txt | grep pattern | wc -l`

**Setup:**
```c
t_cmd cmd3 = {
    .args = (char *[]){"wc", "-l", NULL},
    .redirs = NULL,
    .next = NULL
};

t_cmd cmd2 = {
    .args = (char *[]){"grep", "pattern", NULL},
    .redirs = NULL,
    .next = &cmd3
};

t_cmd cmd1 = {
    .args = (char *[]){"cat", "file.txt", NULL},
    .redirs = NULL,
    .next = &cmd2
};

int status = execute_pipeline(&cmd1, shell);
```

**Execution Flow:**
```
1. Count commands: 3
2. Allocate pids[3]

3. Create pipe1 (FD 3, 4)
4. Fork child 1 (cat):
   - stdout → pipe1 write end (FD 4)
5. Parent closes pipe1 write end (FD 4)

6. Create pipe2 (FD 4, 5)
7. Fork child 2 (grep):
   - stdin → pipe1 read end (FD 3)
   - stdout → pipe2 write end (FD 5)
8. Parent closes pipe1 read end (FD 3)
9. Parent closes pipe2 write end (FD 5)

10. Fork child 3 (wc):
    - stdin → pipe2 read end (FD 4)
11. Parent closes pipe2 read end (FD 4)

12. Wait for all three children
13. Return exit status of wc
```

---

### Example 3: Pipeline with Redirections

**Command:** `cat < input.txt | grep pattern > output.txt`

**Setup:**
```c
t_redir redir2 = {
    .type = REDIR_OUT,
    .file = "output.txt",
    .next = NULL
};

t_redir redir1 = {
    .type = REDIR_IN,
    .file = "input.txt",
    .next = NULL
};

t_cmd cmd2 = {
    .args = (char *[]){"grep", "pattern", NULL},
    .redirs = &redir2,
    .next = NULL
};

t_cmd cmd1 = {
    .args = (char *[]){"cat", NULL},
    .redirs = &redir1,
    .next = &cmd2
};

int status = execute_pipeline(&cmd1, shell);
```

**Execution Flow:**
```
1. Fork child 1 (cat):
   - Setup pipe: stdout → pipe write end
   - Setup redirections: stdin → input.txt
   - execve cat
   
2. Fork child 2 (grep):
   - Setup pipe: stdin → pipe read end
   - Setup redirections: stdout → output.txt
   - execve grep

Result: Data flows from input.txt → cat → pipe → grep → output.txt
```

---

## Testing Guide

### Test Case 1: Basic Pipe

**Input:**
```bash
ls | cat
```

**Expected:**
- List of files displayed
- Exit status: 0

**Verify:**
```bash
# Check output
ls | cat

# Check with valgrind
valgrind --leak-check=full ./minishell
> ls | cat
```

**Check:**
- ✅ Output matches `ls` output
- ✅ No memory leaks
- ✅ No fd leaks
- ✅ Exit status correct

---

### Test Case 2: Multiple Pipes

**Input:**
```bash
ls -la | grep .c | wc -l
```

**Expected:**
- Count of .c files
- Exit status: 0

**Verify:**
```bash
# Compare with bash
bash -c "ls -la | grep .c | wc -l"
./minishell
> ls -la | grep .c | wc -l

# Should produce same result
```

**Check:**
- ✅ Count matches bash
- ✅ All three commands execute
- ✅ Pipes connect correctly

---

### Test Case 3: Pipe with Redirections

**Input:**
```bash
cat < input.txt | grep pattern > output.txt
```

**Expected:**
- Filtered content in output.txt
- Exit status: 0

**Verify:**
```bash
# Create test file
echo -e "line1\npattern\nline3\npattern2" > input.txt

# Run command
./minishell
> cat < input.txt | grep pattern > output.txt

# Check output
cat output.txt
# Should show: pattern, pattern2
```

**Check:**
- ✅ Input file read correctly
- ✅ Pipe works
- ✅ Output file created
- ✅ Content filtered correctly

---

### Test Case 4: Command Not Found

**Input:**
```bash
invalidcmd | cat
```

**Expected:**
- Error: "invalidcmd: command not found"
- cat still runs (reads from empty pipe)
- Exit status: 127

**Verify:**
```bash
./minishell
> invalidcmd | cat
# Should print error but not crash
```

**Check:**
- ✅ Error message printed
- ✅ Second command still executes
- ✅ Exit status 127
- ✅ No crash

---

### Test Case 5: Permission Denied

**Input:**
```bash
/root/private_file | cat
```

**Expected:**
- Error: "Permission denied"
- Exit status: 126

**Check:**
- ✅ Error message printed
- ✅ Appropriate exit code
- ✅ No crash

---

### Test Case 6: Empty Command

**Input:**
```bash
| cat
```

**Expected:**
- Syntax error or skip empty command
- No crash

**Check:**
- ✅ Handled gracefully
- ✅ No segfault

---

### Test Case 7: Memory Leak Test

**Commands:**
```bash
valgrind --leak-check=full --show-leak-kinds=all ./minishell
> ls | cat
> ls | grep .c | wc -l
> cat < file.txt | grep pattern > out.txt
> exit
```

**Expected:**
```
HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
  total heap usage: X allocs, X frees, Y bytes allocated

All heap blocks were freed -- no leaks are possible
```

**Check:**
- ✅ 0 bytes leaked
- ✅ All fds closed
- ✅ All memory freed

---

### Test Case 8: Signal Handling

**Input:**
```bash
sleep 10 | cat
[Press Ctrl+C]
```

**Expected:**
- Both processes terminate
- No zombies
- Clean exit

**Verify:**
```bash
# In one terminal
./minishell
> sleep 10 | cat

# Press Ctrl+C

# In another terminal
ps aux | grep sleep
# Should show no sleep processes

ps aux | grep defunct
# Should show no zombie processes
```

**Check:**
- ✅ Processes terminate
- ✅ No zombies
- ✅ Shell continues

---

### Test Case 9: Large Pipeline

**Input:**
```bash
cat file.txt | grep a | grep b | grep c | wc -l
```

**Expected:**
- Correct count
- All pipes work

**Check:**
- ✅ All commands execute
- ✅ Pipes connect correctly
- ✅ No fd leaks

---

### Test Case 10: Stress Test

**Commands:**
```bash
# Run 100 times
for i in {1..100}; do
    echo "ls | grep .c | wc -l" | ./minishell
done
```

**Expected:**
- All executions succeed
- No memory leaks
- No fd leaks

**Check:**
- ✅ Consistent results
- ✅ No resource leaks
- ✅ No crashes

---

## Common Mistakes and Solutions

### Mistake 1: Not Closing Pipe FDs

**Wrong:**
```c
fork();
if (pid == 0) {
    dup2(pipefd[1], STDOUT_FILENO);
    // ❌ Forgot to close pipefd[1]!
    execve(...);
}
```

**Correct:**
```c
fork();
if (pid == 0) {
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);  // ✅ Close after dup2
    execve(...);
}
```

**Problem:** FD leak, pipe never gets EOF

---

### Mistake 2: Closing Before dup2

**Wrong:**
```c
close(pipefd[1]);
dup2(pipefd[1], STDOUT_FILENO);  // ❌ Copying closed FD!
```

**Correct:**
```c
dup2(pipefd[1], STDOUT_FILENO);  // ✅ dup2 first
close(pipefd[1]);                 // ✅ Then close
```

**Problem:** dup2 fails because source FD is closed

---

### Mistake 3: Parent Not Closing Pipe Ends

**Wrong:**
```c
pipe(pipefd);
fork();
// ❌ Parent keeps both ends open
fork();
```

**Correct:**
```c
pipe(pipefd);
fork();
close(pipefd[1]);  // ✅ Close write end after first fork
fork();
close(pipefd[0]);  // ✅ Close read end after second fork
```

**Problem:** Pipe never gets EOF, grep waits forever

---

### Mistake 4: Not Checking Return Values

**Wrong:**
```c
pipe(pipefd);  // ❌ No error check
fork();        // ❌ No error check
dup2(...);     // ❌ No error check
```

**Correct:**
```c
if (pipe(pipefd) == -1)     // ✅ Check pipe
    handle_error();
if ((pid = fork()) == -1)   // ✅ Check fork
    handle_error();
if (dup2(...) == -1)        // ✅ Check dup2
    handle_error();
```

**Problem:** Silent failures, undefined behavior

---

### Mistake 5: Memory Leaks

**Wrong:**
```c
pids = malloc(sizeof(pid_t) * count);
// ... error occurs ...
return (1);  // ❌ Forgot to free pids!
```

**Correct:**
```c
pids = malloc(sizeof(pid_t) * count);
// ... error occurs ...
free(pids);  // ✅ Free before return
return (1);
```

**Problem:** Memory leak on error path

---

### Mistake 6: Not Waiting for Children

**Wrong:**
```c
fork();
fork();
return (0);  // ❌ Didn't wait for children!
```

**Correct:**
```c
fork();
fork();
waitpid(pid1, &status, 0);  // ✅ Wait for both
waitpid(pid2, &status, 0);
return (status);
```

**Problem:** Zombie processes

---

### Mistake 7: Closing Wrong End

**Wrong:**
```c
// In first child (should write to pipe)
close(pipefd[1]);  // ❌ Closing write end!
dup2(pipefd[0], STDOUT_FILENO);  // ❌ Using read end!
```

**Correct:**
```c
// In first child (should write to pipe)
close(pipefd[0]);  // ✅ Close read end
dup2(pipefd[1], STDOUT_FILENO);  // ✅ Use write end
```

**Problem:** Pipe doesn't work, commands can't communicate

---

## Integration with Minishell

### Required Data Structures

```c
// Command structure
typedef struct s_cmd {
    char            **args;     // NULL-terminated argument array
    struct s_redir  *redirs;    // Linked list of redirections
    struct s_cmd    *next;      // Next command in pipeline
}   t_cmd;

// Shell state structure
typedef struct s_shell {
    char    **env;              // Environment variables
    int     last_exit_code;     // Last command exit code
    // ... other fields ...
}   t_shell;

// Redirection structure
typedef struct s_redir {
    t_token_type    type;       // REDIR_IN, REDIR_OUT, etc.
    char            *file;      // Target filename
    struct s_redir  *next;      // Next redirection
}   t_redir;
```

### Required Helper Functions

```c
// Find executable in PATH
char *find_executable(char *cmd, char **env);

// Setup redirections for a command
int setup_redirections(t_redir *redirs);

// Check if command is a builtin
int is_builtin(char *cmd);

// Execute builtin command
int execute_builtin(char **args, t_shell *shell);
```

### Integration Example

```c
// In main execution loop
int execute_command(t_pipeline *pipeline, t_shell *shell)
{
    int exit_status;
    
    if (!pipeline || !pipeline->cmds)
        return (0);
    
    // Check if single command
    if (!pipeline->cmds->next)
    {
        // Single command, may need special handling for builtins
        exit_status = execute_single_cmd(pipeline->cmds, shell);
    }
    else
    {
        // Pipeline, use execute_pipeline
        exit_status = execute_pipeline(pipeline->cmds, shell);
    }
    
    // Update last exit code
    shell->last_exit_code = exit_status;
    
    return (exit_status);
}
```

---

## Performance Considerations

### Memory Usage

**Per Pipeline:**
- `sizeof(pid_t) * cmd_count` for pid array
- Minimal additional memory

**Optimization:**
- Could use static array for small pipelines
- Trade-off: less flexible, but no malloc

### File Descriptor Usage

**Per Pipeline:**
- 2 FDs per pipe
- For N commands: (N-1) pipes = 2(N-1) FDs
- All closed after execution

**Limit:**
- System limit usually 1024 FDs per process
- Can handle ~500 command pipeline theoretically
- Practical limit much lower

### Process Creation

**Cost:**
- fork() is expensive
- Each command creates new process
- Unavoidable for proper isolation

**Optimization:**
- Builtins can run in parent (when not in pipeline)
- Reduces fork count for common commands

---

## Conclusion

This implementation provides:

✅ **Complete error handling** for all system calls  
✅ **Proper resource management** (memory, FDs, processes)  
✅ **Two versions** (standard and Norminette-compliant)  
✅ **Comprehensive documentation** and examples  
✅ **Testing guide** with multiple test cases  
✅ **Production-ready** code

**Use this as your reference implementation for the minishell project!**

---

**End of Guide**
