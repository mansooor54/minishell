# Executor

## Files
- `src/executor/executor.c`
- `src/executor/executor_pipeline.c`
- `src/executor/executor_pipeline_run.c`
- `src/executor/executor_pipeline_loop.c`
- `src/executor/executor_pipeline_utils.c`
- `src/executor/executor_pipeline_helpers.c`
- `src/executor/executor_commands.c`
- `src/executor/executor_commands_utils.c`
- `src/executor/executor_external.c`
- `src/executor/executor_external_utils.c`
- `src/executor/executor_path.c`
- `src/executor/executor_path_search.c`
- `src/executor/executor_path_utils.c`
- `src/executor/executor_redirections.c`
- `src/executor/executor_redir_io.c`
- `src/executor/executor_redir_heredoc.c`
- `src/executor/executor_redir_heredoc_utils.c`
- `src/executor/executor_child_fds.c`
- `src/executor/executor_child_run.c`
- `src/executor/executor_utils.c`
- `src/executor/executor_error.c`

---

## What is the Executor?

The executor takes the parsed and expanded command structure and actually runs the commands. It handles:
- Running builtin commands (echo, cd, etc.)
- Forking for external commands
- Setting up pipes between commands
- Setting up file redirections
- Waiting for child processes

---

## executor_pipeline.c

### executor()
```c
void executor(t_pipeline *pipeline, t_shell *shell)
{
    while (pipeline)
    {
        shell->exit_status = execute_pipeline(pipeline->cmds, shell);
        pipeline = pipeline->next;
    }
}
```

**Purpose**: Main entry point - execute all pipelines.

**Note**: In mandatory, there's only one pipeline per command line.

---

## Pipeline Execution

### execute_pipeline() (simplified view)
```c
int execute_pipeline(t_cmd *cmds, t_shell *shell)
{
    int     cmd_count;
    pid_t   *pids;
    int     status;

    // Single builtin without pipes? Run in parent
    if (!cmds->next && cmds->args && cmds->args[0] && is_builtin(cmds->args[0]))
        return (execute_single_builtin_parent(cmds, shell));

    cmd_count = count_commands(cmds);
    if (!init_pipeline(cmd_count, &pids))
        return (1);

    status = execute_pipeline_loop(cmds, shell, pids, cmd_count);
    free(pids);
    return (status);
}
```

**Purpose**: Orchestrate pipeline execution.

**Special case**: Single builtin runs in parent process (for `cd`, `export` to affect shell state).

---

### execute_pipeline_loop()
```c
int execute_pipeline_loop(t_cmd *cmds, t_shell *shell, pid_t *pids, int cmd_count)
{
    t_pipe_ctx  ctx;
    t_cmd       *cmd;
    int         i;
    int         prev_rd;

    ctx.shell = shell;
    ctx.pids = pids;
    prev_rd = -1;
    ctx.prev_rd = &prev_rd;
    cmd = cmds;
    i = 0;
    while (cmd)
    {
        execute_one_command(cmd, i, &ctx);
        cmd = cmd->next;
        i++;
    }
    if (prev_rd != -1)
        close(prev_rd);
    return (wait_for_children(pids, cmd_count));
}
```

**Purpose**: Loop through commands, fork each one, set up pipes.

---

## Pipe Mechanism

### How Pipes Work
```
cmd1 | cmd2 | cmd3

┌─────────┐    pipe[0]    ┌─────────┐    pipe[1]    ┌─────────┐
│  cmd1   │──────────────►│  cmd2   │──────────────►│  cmd3   │
│         │   write→read  │         │   write→read  │         │
└─────────┘               └─────────┘               └─────────┘
```

Each pipe has:
- `pipefd[0]` = read end
- `pipefd[1]` = write end

---

### setup_child_fds()
```c
int setup_child_fds(int pipefd[2], int prev_read_fd, int has_next)
{
    // Connect stdin to previous pipe's read end
    if (prev_read_fd != -1)
    {
        if (dup2(prev_read_fd, STDIN_FILENO) == -1)
            return (-1);
        close(prev_read_fd);
    }

    // Connect stdout to current pipe's write end
    if (has_next)
    {
        close(pipefd[0]);  // Close read end (we write)
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
            return (-1);
        close(pipefd[1]);
    }
    return (0);
}
```

**Purpose**: Connect child's stdin/stdout to pipe ends.

**dup2()**: Duplicates file descriptor to specific number.
- `dup2(pipefd[1], STDOUT_FILENO)` makes stdout write to pipe

---

## External Command Execution

### execute_external()
```c
void execute_external(t_cmd *cmd, t_shell *shell)
{
    char    *path;
    pid_t   pid;

    if (!cmd || !cmd->args || !cmd->args[0])
        return;
    if (handle_path_resolution(cmd, shell, &path))
        return;
    pid = fork();
    if (pid == -1)
    {
        print_error("fork", strerror(errno));
        free(path);
        shell->exit_status = 127;
        return;
    }
    if (pid == 0)
        execute_child_process(cmd, shell, path);
    free(path);
    handle_parent_process(pid, shell);
}
```

**Purpose**: Fork and execute external program.

---

### execute_child_process()
```c
static void execute_child_process(t_cmd *cmd, t_shell *shell, char *path)
{
    char **envp;

    signal(SIGINT, SIG_DFL);   // Reset signal handlers
    signal(SIGQUIT, SIG_DFL);
    if (setup_redirections(cmd->redirs, shell) == -1)
        exit(1);
    envp = env_to_array(shell->env);
    execve(path, cmd->args, envp);
    // If we get here, execve failed
    free_array(envp);
    if (errno == EACCES)
        exit(126);
    else
        exit(127);
}
```

**Purpose**: Child process setup and exec.

**Steps**:
1. Reset signals (child should have default handlers)
2. Setup redirections
3. Convert env to array for execve
4. Execute program (never returns if successful)
5. Handle exec failure

---

### handle_parent_process()
```c
static void handle_parent_process(pid_t pid, t_shell *shell)
{
    int status;

    status = 0;
    signal(SIGINT, SIG_IGN);   // Ignore signals while waiting
    signal(SIGQUIT, SIG_IGN);
    if (waitpid(pid, &status, 0) == -1)
    {
        print_error("waitpid", strerror(errno));
        shell->exit_status = 1;
    }
    else if (WIFEXITED(status))
        shell->exit_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
    {
        shell->exit_status = 128 + WTERMSIG(status);
        write(1, "\n", 1);
    }
    signal(SIGINT, handle_sigint);    // Restore handlers
    signal(SIGQUIT, handle_sigquit);
}
```

**Purpose**: Parent waits for child and gets exit status.

**Exit status macros**:
- `WIFEXITED(status)` - true if child exited normally
- `WEXITSTATUS(status)` - get exit code
- `WIFSIGNALED(status)` - true if child killed by signal
- `WTERMSIG(status)` - get signal number

---

## Path Resolution

### resolve_command_path()
```c
char *resolve_command_path(char *cmd, t_shell *shell)
{
    if (has_slash(cmd))
        return (ft_strdup(cmd));  // Already a path
    return (find_executable(cmd, shell->env));
}
```

**Purpose**: Find full path to executable.

**Cases**:
- `./program` or `/usr/bin/ls` → use as-is
- `ls` → search in PATH

---

### find_executable()
```c
char *find_executable(char *cmd, t_env *env)
{
    char *path_var;
    char *result;

    path_var = get_env_value(env, "PATH");
    if (!path_var)
        return (NULL);
    result = search_in_path(path_var, cmd);
    return (result);
}
```

**Purpose**: Search PATH directories for command.

---

### search_in_path()
```c
char *search_in_path(const char *path, const char *cmd)
{
    size_t  start;
    size_t  end;
    char    *dir;
    char    *full;

    start = 0;
    while (path[start])
    {
        end = seg_end(path, start);
        dir = dup_segment_or_dot(path, start, end);
        if (!dir)
            return (NULL);
        full = join_cmd_path(dir, cmd);
        free(dir);
        if (full && is_exec_file(full))
            return (full);
        free(full);
        start = end;
        if (path[start] == ':')
            start++;
    }
    return (NULL);
}
```

**Purpose**: Try each PATH directory.

**PATH format**: `/usr/bin:/bin:/usr/local/bin`

---

## Redirections

### setup_redirections()
```c
int setup_redirections(t_redir *redirs, t_shell *shell)
{
    while (redirs)
    {
        if (redirs->type == TOKEN_REDIR_IN)
        {
            if (handle_input(redirs->file) == -1)
                return (-1);
        }
        else if (redirs->type == TOKEN_REDIR_OUT)
        {
            if (handle_output(redirs->file, 0) == -1)
                return (-1);
        }
        else if (redirs->type == TOKEN_REDIR_APPEND)
        {
            if (handle_output(redirs->file, 1) == -1)
                return (-1);
        }
        else if (redirs->type == TOKEN_REDIR_HEREDOC)
        {
            if (handle_heredoc(redirs->file, shell) == -1)
                return (-1);
        }
        redirs = redirs->next;
    }
    return (0);
}
```

**Purpose**: Process all redirections for a command.

---

### handle_input()
```c
int handle_input(char *file)
{
    int fd;

    fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(file, 2);
        ft_putstr_fd(": ", 2);
        ft_putendl_fd(strerror(errno), 2);
        return (-1);
    }
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        close(fd);
        return (-1);
    }
    close(fd);
    return (0);
}
```

**Purpose**: Redirect stdin from file.

---

### handle_output()
```c
int handle_output(char *file, int append)
{
    int fd;
    int flags;

    flags = O_WRONLY | O_CREAT;
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    fd = open(file, flags, 0644);
    if (fd == -1)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(file, 2);
        ft_putstr_fd(": ", 2);
        ft_putendl_fd(strerror(errno), 2);
        return (-1);
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        close(fd);
        return (-1);
    }
    close(fd);
    return (0);
}
```

**Purpose**: Redirect stdout to file.

**Flags**:
- `O_TRUNC` → overwrite (for `>`)
- `O_APPEND` → append (for `>>`)
- `0644` → file permissions (rw-r--r--)

---

### handle_heredoc()
```c
int handle_heredoc(char *delimiter, t_shell *shell)
{
    int     pipefd[2];
    char    *line;
    char    *clean;
    int     quoted;

    clean = clean_delimiter(delimiter, &quoted);
    if (pipe(pipefd) == -1)
        return (-1);

    while (1)
    {
        line = readline("> ");
        if (!line || check_heredoc_end(line, clean))
        {
            free(line);
            break;
        }
        line = get_expanded_line(line, quoted, shell->env, shell->exit_status);
        write(pipefd[1], line, ft_strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
    }
    free(clean);
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    return (0);
}
```

**Purpose**: Read heredoc content and pipe to stdin.

**Heredoc example**:
```bash
cat << EOF
Hello $USER
World
EOF
```

---

## Execution Flow Diagram

```
     executor()
          │
          ▼
    ┌─────────────┐
    │ execute_    │
    │ pipeline()  │
    └──────┬──────┘
           │
           ├── Single builtin? ──► execute_single_builtin_parent()
           │
           ▼ (multiple commands or external)
    ┌─────────────────────────────────────────┐
    │       execute_pipeline_loop()            │
    │                                          │
    │   For each command:                      │
    │   ┌─────────────────────────────────┐   │
    │   │     Create pipe (if not last)   │   │
    │   │              │                   │   │
    │   │              ▼                   │   │
    │   │          fork()                  │   │
    │   │         /      \                 │   │
    │   │     Child      Parent            │   │
    │   │        │          │              │   │
    │   │        ▼          │              │   │
    │   │  setup_child_fds  │              │   │
    │   │        │          │              │   │
    │   │        ▼          │              │   │
    │   │  setup_redirections              │   │
    │   │        │          │              │   │
    │   │        ▼          │              │   │
    │   │  execve() or      │              │   │
    │   │  execute_builtin  │              │   │
    │   └─────────────────────────────────┘   │
    │                                          │
    │   wait_for_children()                    │
    └─────────────────────────────────────────┘
```

---

## Key Concepts

### Why Fork?
The shell cannot directly run external programs - it would replace itself. `fork()` creates a child process that can be replaced with `execve()`.

### Why Builtins in Parent?
Commands like `cd` and `export` must modify the shell's state. If run in a child, changes would be lost when child exits.

### Pipe Closing Rules
- Close unused pipe ends to avoid deadlocks
- Parent closes both ends after forking
- Child closes the end it doesn't use
- Reader closes write end, writer closes read end

### File Descriptor Inheritance
After `fork()`, child inherits parent's open file descriptors. This is how pipes work - parent creates pipe, forks, child inherits pipe ends.
