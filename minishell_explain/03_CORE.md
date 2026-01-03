# Core Shell Loop

## Files
- `src/core/shell_loop.c`
- `src/core/read_logical_line.c`
- `src/core/join_continuation.c`
- `src/core/read_logical_line_utils.c`
- `src/core/shell_utils.c`

---

## What is the Shell Loop?

**Real-life analogy**: The shell loop is like a **cashier at a restaurant**:

```
┌──────────────────────────────────────────────────────┐
│                    CASHIER LOOP                       │
│                                                       │
│   1. Say "What would you like?" (show prompt)        │
│   2. Customer says "burger please" (read input)      │
│   3. Check if valid order (syntax check)             │
│   4. Send to kitchen (execute)                       │
│   5. Give food to customer (show output)             │
│   6. Go back to step 1 (loop)                        │
│                                                       │
│   Stop when: Customer says "bye" or leaves (exit)    │
└──────────────────────────────────────────────────────┘
```

In shell terms:
```
┌──────────────────────────────────────────────────────┐
│                    SHELL LOOP                         │
│                                                       │
│   1. Show "minishell> " (prompt)                     │
│   2. User types "ls -la" (read input)                │
│   3. Check for syntax errors (validate)              │
│   4. Run the command (execute)                       │
│   5. Show files list (output)                        │
│   6. Go back to step 1 (loop)                        │
│                                                       │
│   Stop when: "exit" command or Ctrl+D                │
└──────────────────────────────────────────────────────┘
```

---

## shell_loop.c - The Main Loop

### shell_loop() - Complete Example

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

### Let's Walk Through an Example

**User types**: `echo hello`

```
Iteration 1 of shell_loop():

┌──────────────────────────────────────────────────────┐
│ Step 1: setup_signals()                               │
│         - Make sure Ctrl+C handler is ready          │
│                                                       │
│ Step 2: check_signal(shell)                          │
│         - g_signal == 0 (no Ctrl+C pressed)          │
│         - Do nothing, continue                        │
│                                                       │
│ Step 3: read_logical_line()                          │
│         - Display: "minishell> "                     │
│         - User types: echo hello                     │
│         - Press Enter                                │
│         - line = "echo hello"                        │
│                                                       │
│ Step 4: check_signal(shell)                          │
│         - Still g_signal == 0, continue              │
│                                                       │
│ Step 5: handle_eof()                                 │
│         - line is NOT NULL, return 0                 │
│         - Don't break                                │
│                                                       │
│ Step 6: Check if valid input                         │
│         - line exists ✓                              │
│         - line is not empty ✓                        │
│         - line is not just spaces ✓                  │
│                                                       │
│ Step 7: history_add_line()                           │
│         - Add "echo hello" to history                │
│         - Now ↑ arrow will show it                   │
│                                                       │
│ Step 8: process_line("echo hello", shell)            │
│         - Lexer: [WORD:"echo"] [WORD:"hello"]        │
│         - Parser: cmd{args:["echo","hello"]}         │
│         - Expander: nothing to expand                │
│         - Executor: runs builtin_echo()              │
│         - Output: "hello" printed to screen          │
│                                                       │
│ Step 9: free(line)                                   │
│         - Free "echo hello" string                   │
│                                                       │
│ Step 10: Loop back to Step 1...                      │
└──────────────────────────────────────────────────────┘
```

---

## What Happens with Ctrl+C?

**Example**: User is typing but presses Ctrl+C

```
minishell> echo hel^C     ← User presses Ctrl+C mid-typing

minishell>                 ← Fresh prompt appears!
```

**Behind the scenes**:

```
┌──────────────────────────────────────────────────────┐
│ 1. User starts typing "echo hel"                     │
│                                                       │
│ 2. User presses Ctrl+C                               │
│    - Kernel sends SIGINT signal                      │
│    - handle_sigint() runs immediately:               │
│      • g_signal = 2 (SIGINT)                         │
│      • write("\n") - new line                        │
│      • rl_on_new_line() - tell readline              │
│      • rl_replace_line("") - clear input             │
│      • rl_redisplay() - show fresh prompt            │
│                                                       │
│ 3. readline() returns (maybe with partial "echo hel")│
│                                                       │
│ 4. Back in shell_loop():                             │
│    check_signal(shell):                              │
│      - Sees g_signal == 2                            │
│      - Sets shell->exit_status = 130                 │
│      - Resets g_signal = 0                           │
│                                                       │
│ 5. Loop continues with fresh prompt                  │
└──────────────────────────────────────────────────────┘
```

**Why exit status 130?**
```
130 = 128 + 2 (SIGINT number)

This is Unix convention:
- Exit codes 0-127: normal exit codes
- Exit codes 128+N: killed by signal N
```

---

## What Happens with Ctrl+D (EOF)?

**Example**: User presses Ctrl+D at empty prompt

```
minishell> ^D              ← User presses Ctrl+D
exit                       ← Shell prints "exit"
$                          ← Back to parent shell
```

**Behind the scenes**:

```
┌──────────────────────────────────────────────────────┐
│ 1. readline() is waiting for input                   │
│                                                       │
│ 2. User presses Ctrl+D on empty line                 │
│    - This sends EOF (End Of File) signal             │
│    - readline() returns NULL                         │
│                                                       │
│ 3. handle_eof(shell, NULL):                          │
│    - line is NULL → true                             │
│    - if (interactive) print "exit"                   │
│    - return 1                                         │
│                                                       │
│ 4. shell_loop sees return 1 → break                  │
│                                                       │
│ 5. main() continues to cleanup and exit              │
└──────────────────────────────────────────────────────┘
```

---

## read_logical_line.c - Reading with Prompt

### The Prompt Anatomy

```c
line = read_one_line("\001\033[1;33m\002minishell> \001\033[0m\002");
```

**Breaking down the prompt string**:
```
\001           = Tell readline: "next chars are non-printing"
\033[1;33m     = ANSI: set color to bold yellow
\002           = Tell readline: "end of non-printing chars"
minishell>     = The actual text user sees
                 (space after >)
\001           = Start non-printing again
\033[0m        = ANSI: reset color to default
\002           = End non-printing
```

**Why \001 and \002?**
```
Without them:                    With them:
┌────────────────────┐           ┌────────────────────┐
│ minishell> hello█  │           │ minishell> hello█  │
│     ^              │           │               ^    │
│ cursor here wrong! │           │ cursor here right! │
└────────────────────┘           └────────────────────┘

readline counts all characters for cursor position.
Color codes are invisible but counted without markers.
```

---

### Multi-line Input (Unclosed Quotes)

**Example**: User types string with unclosed quote

```
minishell> echo "hello
> world
> done"
hello
world
done
minishell>
```

**How it works**:

```
┌──────────────────────────────────────────────────────┐
│ Step 1: read_one_line("minishell> ")                 │
│         User types: echo "hello                      │
│         line = "echo \"hello"                        │
│                                                       │
│ Step 2: needs_continuation(line)?                    │
│         - Check if quotes are balanced               │
│         - Found " but no closing "                   │
│         - Return TRUE                                │
│                                                       │
│ Step 3: process_continuation(line)                   │
│         - read_one_line("> ")                        │
│         - User types: world                          │
│         - more = "world"                             │
│         - join_continuation(line, more)              │
│         - line = "echo \"hello\nworld"               │
│                                                       │
│ Step 4: needs_continuation(line)?                    │
│         - Still no closing "                         │
│         - Return TRUE                                │
│                                                       │
│ Step 5: process_continuation(line)                   │
│         - read_one_line("> ")                        │
│         - User types: done"                          │
│         - more = "done\""                            │
│         - join_continuation(line, more)              │
│         - line = "echo \"hello\nworld\ndone\""       │
│                                                       │
│ Step 6: needs_continuation(line)?                    │
│         - Quotes are now balanced!                   │
│         - Return FALSE                               │
│                                                       │
│ Step 7: Return complete line                         │
└──────────────────────────────────────────────────────┘
```

---

## Interactive vs Non-Interactive Mode

### What's the difference?

| Aspect | Interactive | Non-Interactive |
|--------|-------------|-----------------|
| Input from | Keyboard (terminal) | Pipe or file |
| Prompt | Shows "minishell> " | No prompt |
| History | Saved | Not saved |
| Detection | `isatty(STDIN) == 1` | `isatty(STDIN) == 0` |

### Examples

**Interactive (terminal)**:
```bash
$ ./minishell
minishell> ls        ← You type this
file1.txt file2.txt  ← Output
minishell>           ← Prompt again
```

**Non-Interactive (pipe)**:
```bash
$ echo "ls" | ./minishell
file1.txt file2.txt  ← Output, no prompts!
```

**Non-Interactive (here-string)**:
```bash
$ ./minishell <<< "ls"
file1.txt file2.txt
```

### Detection Code

```c
void init_shell(t_shell *shell, char **envp)
{
    // ...
    shell->interactive = isatty(STDIN_FILENO);
    // isatty returns 1 if stdin is a terminal
    // isatty returns 0 if stdin is a pipe or file
}
```

---

## process_line() - The Processing Pipeline

```
                    "ls -la | grep .c"
                           │
                           ▼
                    ┌─────────────┐
                    │   LEXER     │
                    │             │
                    │ Split into  │
                    │ tokens      │
                    └──────┬──────┘
                           │
             [WORD] [WORD] [PIPE] [WORD] [WORD]
               ls    -la    |      grep   .c
                           │
                           ▼
                    ┌─────────────┐
                    │  VALIDATE   │
                    │   SYNTAX    │
                    │             │
                    │ Check for   │
                    │ errors      │
                    └──────┬──────┘
                           │
                    OK? ───┼─── Error?
                           │         │
                           │         ▼
                           │    Print error
                           │    exit_status = 2
                           │    return
                           │
                           ▼
                    ┌─────────────┐
                    │   PARSER    │
                    │             │
                    │ Build       │
                    │ command     │
                    │ structures  │
                    └──────┬──────┘
                           │
                    t_pipeline
                    ├── cmd1: args=["ls","-la"]
                    └── cmd2: args=["grep",".c"]
                           │
                           ▼
                    ┌─────────────┐
                    │  EXPANDER   │
                    │             │
                    │ Replace $   │
                    │ Remove ""   │
                    └──────┬──────┘
                           │
                    (no changes here, no $ or quotes)
                           │
                           ▼
                    ┌─────────────┐
                    │  EXECUTOR   │
                    │             │
                    │ Fork, pipe  │
                    │ execve      │
                    └──────┬──────┘
                           │
                           ▼
                    Output: .c files listed
```

---

## Shell Loop Flow Diagram - Complete

```
                         ┌─────────────────────┐
                         │    shell_loop()     │
                         └──────────┬──────────┘
                                    │
                                    ▼
                         ┌─────────────────────┐
            ┌───────────►│  should_exit?       │───yes───► return
            │            └──────────┬──────────┘
            │                       │ no
            │                       ▼
            │            ┌─────────────────────┐
            │            │   setup_signals()   │
            │            │                     │
            │            │ Reset Ctrl+C/Ctrl+\ │
            │            │ handlers            │
            │            └──────────┬──────────┘
            │                       │
            │                       ▼
            │            ┌─────────────────────┐
            │            │   check_signal()    │
            │            │                     │
            │            │ if g_signal == 2:   │
            │            │   exit_status = 130 │
            │            │   g_signal = 0      │
            │            └──────────┬──────────┘
            │                       │
            │                       ▼
            │            ┌─────────────────────┐
            │            │  read_logical_line()│
            │            │  or readline("")    │
            │            │                     │
            │            │ Show prompt, wait   │
            │            │ for user input      │
            │            └──────────┬──────────┘
            │                       │
            │                       ▼
            │            ┌─────────────────────┐
            │            │  line == NULL?      │───yes───► print "exit"
            │            │  (Ctrl+D / EOF)     │           break loop
            │            └──────────┬──────────┘
            │                       │ no
            │                       ▼
            │            ┌─────────────────────┐
            │            │  Empty or spaces?   │───yes───► free(line)
            │            └──────────┬──────────┘           loop back
            │                       │ no
            │                       ▼
            │            ┌─────────────────────┐
            │            │  history_add_line() │
            │            │  (if interactive)   │
            │            └──────────┬──────────┘
            │                       │
            │                       ▼
            │            ┌─────────────────────┐
            │            │   process_line()    │
            │            │                     │
            │            │ lexer → validate →  │
            │            │ parser → expander → │
            │            │ executor            │
            │            └──────────┬──────────┘
            │                       │
            │                       ▼
            │            ┌─────────────────────┐
            │            │     free(line)      │
            │            └──────────┬──────────┘
            │                       │
            └───────────────────────┘
```

---

## Common Questions

### Q: Why call setup_signals() every iteration?

**A**: Child processes might change signal handlers. After forking and waiting, we restore our handlers to be safe.

### Q: Why check_signal() twice (before and after readline)?

**A**:
- **Before**: Handle any Ctrl+C from previous command
- **After**: Handle Ctrl+C pressed during input

### Q: What if user types only spaces?

**A**:
```
minishell>          ← User types only spaces, presses Enter
minishell>          ← No error, no action, just new prompt
```

The `is_all_space()` check skips processing empty/whitespace-only lines.

### Q: Why add to history BEFORE processing?

**A**: Even if command fails, we want to remember it:
```
minishell> lss       ← Typo!
minishell: lss: command not found
minishell>           ← Press ↑
minishell> lss       ← Can fix the typo!
```

---

## Key Concepts Summary

| Function | Purpose | When it runs |
|----------|---------|--------------|
| `shell_loop()` | Main REPL loop | Continuously |
| `setup_signals()` | Reset signal handlers | Each iteration |
| `check_signal()` | Handle deferred Ctrl+C | Before/after input |
| `read_logical_line()` | Read with prompt & multi-line | Interactive mode |
| `readline("")` | Read without prompt | Non-interactive |
| `handle_eof()` | Handle Ctrl+D | When line is NULL |
| `process_line()` | Full command processing | Valid input only |
| `history_add_line()` | Remember command | Interactive mode |
