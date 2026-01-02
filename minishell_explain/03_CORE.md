# Core Shell Loop

## Files
- `src/core/shell_loop.c`
- `src/core/read_logical_line.c`
- `src/core/join_continuation.c`
- `src/core/read_logical_line_utils.c`
- `src/core/shell_utils.c`

---

## shell_loop.c

### shell_loop()
```c
void shell_loop(t_shell *shell)
{
    char *line;

    while (!shell->should_exit)
    {
        setup_signals();
        check_signal(shell);
        if (shell->interactive)
            line = read_logical_line();
        else
            line = readline("");
        check_signal(shell);
        if (handle_eof(shell, line))
            break;
        if (line && *line && !is_all_space(line))
        {
            if (shell->interactive)
                history_add_line(line, shell);
            process_line(line, shell);
        }
        free(line);
    }
}
```

**Purpose**: The heart of the shell - the Read-Eval-Print Loop (REPL).

**Step-by-step**:

1. **`while (!shell->should_exit)`**
   - Keeps running until `exit` command sets this flag
   - Or until EOF (Ctrl+D) breaks out

2. **`setup_signals()`**
   - Reset signal handlers each iteration
   - Important after child processes might have changed them

3. **`check_signal(shell)`**
   - If Ctrl+C was pressed (`g_signal == SIGINT`)
   - Set exit_status to 130 (128 + 2)
   - Reset g_signal to 0

4. **Reading input**:
   - Interactive mode: `read_logical_line()` with colored prompt
   - Non-interactive: `readline("")` for scripts/pipes

5. **`handle_eof()`**
   - If line is NULL (Ctrl+D or EOF)
   - Print "exit" if interactive
   - Return 1 to break the loop

6. **Processing valid input**:
   - Skip if line is NULL, empty, or all whitespace
   - Add to history if interactive
   - `process_line()` does lexer → parser → expander → executor

7. **`free(line)`**
   - Always free the allocated line

---

### check_signal()
```c
static void check_signal(t_shell *shell)
{
    if (g_signal == SIGINT)
    {
        shell->exit_status = 130;
        g_signal = 0;
    }
}
```

**Purpose**: Handle deferred signal processing.

**Why deferred**: Signal handlers should be minimal - they just set `g_signal`. The main loop checks this flag and handles it properly.

**Exit code 130**: Standard convention for "terminated by SIGINT" (128 + signal number 2).

---

### handle_eof()
```c
static int handle_eof(t_shell *shell, char *line)
{
    if (!line)
    {
        if (shell->interactive)
            ft_putendl_fd("exit", 1);
        return (1);
    }
    return (0);
}
```

**Purpose**: Handle end-of-file (Ctrl+D).

**Why print "exit"**: When user presses Ctrl+D at empty prompt, bash prints "exit" before closing. We mimic this behavior for interactive sessions.

---

## read_logical_line.c

### read_logical_line()
```c
char *read_logical_line(void)
{
    char *line;

    line = read_one_line("\001\033[1;33m\002minishell> \001\033[0m\002");
    if (!line)
        return (NULL);
    while (needs_continuation(line))
    {
        line = process_continuation(line);
        if (!line)
            return (NULL);
    }
    return (line);
}
```

**Purpose**: Read a complete logical line, handling multi-line input for unclosed quotes.

**Prompt anatomy**:
- `\001` and `\002` are readline markers for non-printing characters
- `\033[1;33m` = yellow color
- `minishell> ` = the visible prompt
- `\033[0m` = reset color

**Why markers**: Without `\001`/`\002`, readline miscalculates cursor position when using colors, causing display glitches.

---

### read_one_line()
```c
static char *read_one_line(const char *prompt)
{
    char *line;

    g_signal = 0;
    line = readline(prompt);
    if (g_signal == SIGINT)
    {
        if (line)
            free(line);
        return (NULL);
    }
    return (line);
}
```

**Purpose**: Wrapper around readline that handles SIGINT.

**Why check g_signal**: If user presses Ctrl+C during input:
- Signal handler sets g_signal
- readline returns (possibly with partial input)
- We discard any partial input and return NULL

---

### process_continuation()
```c
static char *process_continuation(char *line)
{
    char *more;

    more = read_one_line("> ");
    if (g_signal == SIGINT)
    {
        free(line);
        if (more)
            free(more);
        return (NULL);
    }
    if (!more)
        return (handle_continuation_error(line));
    line = join_continuation(line, more);
    free(more);
    return (line);
}
```

**Purpose**: Handle unclosed quotes by reading more input.

**Scenario**:
```
minishell> echo "hello
> world"
```
When user types `echo "hello` and presses Enter, the quote isn't closed. Shell prompts with `> ` for continuation.

---

## join_continuation.c

### join_continuation()
```c
char *join_continuation(char *line, char *next)
{
    char *r;

    if (!line && !next)
        return (NULL);
    if (!line)
        return (ft_strdup(next));
    if (!next)
        return (line);
    while (*next == ' ' || *next == '\t' || *next == '\n')
        next++;
    r = str_join(line, next);
    free(line);
    return (r);
}
```

**Purpose**: Join a continuation line to the original line.

**Why skip whitespace**: Leading whitespace in continuation is typically not meaningful.

**Memory management**: Frees the old `line` and returns new combined string.

---

### str_join() (helper)
```c
static char *str_join(const char *a, const char *b)
{
    size_t la, lb;
    char *r;

    la = get_safe_len(a);
    lb = get_safe_len(b);
    r = malloc(la + lb + 1);
    if (!r)
        return (NULL);
    if (a)
        copy_str(r, a, la);
    if (b)
        copy_str(r + la, b, lb);
    r[la + lb] = '\0';
    return (r);
}
```

**Purpose**: Safe string concatenation with NULL handling.

---

## Shell Loop Flow Diagram

```
                    ┌─────────────────┐
                    │  shell_loop()   │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
          ┌────────►│ should_exit?    │
          │         └────────┬────────┘
          │                  │ no
          │                  ▼
          │         ┌─────────────────┐
          │         │ setup_signals() │
          │         └────────┬────────┘
          │                  │
          │                  ▼
          │         ┌─────────────────┐
          │         │ check_signal()  │
          │         └────────┬────────┘
          │                  │
          │                  ▼
          │         ┌─────────────────┐
          │         │ readline()      │
          │         │ (with prompt)   │
          │         └────────┬────────┘
          │                  │
          │                  ▼
          │         ┌─────────────────┐
          │         │ line == NULL?   │──yes──► Print "exit", break
          │         └────────┬────────┘
          │                  │ no
          │                  ▼
          │         ┌─────────────────┐
          │         │ empty or space? │──yes──► free(line), loop
          │         └────────┬────────┘
          │                  │ no
          │                  ▼
          │         ┌─────────────────┐
          │         │ history_add()   │
          │         └────────┬────────┘
          │                  │
          │                  ▼
          │         ┌─────────────────┐
          │         │ process_line()  │ ──► lexer → parser → executor
          │         └────────┬────────┘
          │                  │
          │                  ▼
          │         ┌─────────────────┐
          │         │   free(line)    │
          │         └────────┬────────┘
          │                  │
          └──────────────────┘
```

---

## Key Concepts

### Interactive vs Non-Interactive Mode
```c
if (shell->interactive)
    line = read_logical_line();  // With prompt
else
    line = readline("");          // No prompt
```

- **Interactive**: Shell is connected to a terminal (TTY)
- **Non-interactive**: Input from pipe or file (`echo "ls" | ./minishell`)

Detected using `isatty(STDIN_FILENO)` in `init_shell()`.

### Why process_line() is separate
The `process_line()` function (in shell_utils.c) encapsulates:
1. Lexer call
2. Syntax validation
3. Parser call
4. Expander call
5. Executor call
6. Memory cleanup

This separation keeps `shell_loop()` clean and focused on the REPL logic.
