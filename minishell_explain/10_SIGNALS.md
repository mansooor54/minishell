# Signal Handling

## Files
- `src/signals/signals.c`

---

## What are Signals?

Signals are asynchronous notifications sent to a process. Common signals in shells:

| Signal | Key | Default Action | Shell Behavior |
|--------|-----|----------------|----------------|
| SIGINT | Ctrl+C | Terminate | New prompt |
| SIGQUIT | Ctrl+\ | Core dump | Ignored |
| SIGTERM | - | Terminate | Terminate |
| SIGTSTP | Ctrl+Z | Stop | (Not required) |

---

## The Global Variable

```c
volatile sig_atomic_t g_signal = 0;
```

**Why global**: Signal handlers can't access local variables. This is the ONLY way to communicate signal reception to the main program.

**Why `volatile`**: Compiler optimization might cache the value. `volatile` ensures every read fetches from memory.

**Why `sig_atomic_t`**: Guarantees atomic read/write operations. Prevents partial reads during signal handling.

---

## signals.c

### handle_sigint()
```c
void handle_sigint(int sig)
{
    g_signal = sig;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
```

**Purpose**: Handle Ctrl+C (SIGINT).

**Behavior**:
1. Store signal number in global
2. Print newline (move to next line)
3. `rl_on_new_line()` - tell readline we're on new line
4. `rl_replace_line("", 0)` - clear current input
5. `rl_redisplay()` - show fresh prompt

**Result**: Pressing Ctrl+C gives a clean new prompt.

---

### handle_sigquit()
```c
void handle_sigquit(int sig)
{
    (void)sig;
}
```

**Purpose**: Handle Ctrl+\ (SIGQUIT).

**Behavior**: Do nothing (ignore).

**Why**: Subject requires Ctrl+\ to do nothing at prompt.

---

### setup_signals()
```c
void setup_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handle_sigquit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGQUIT, &sa, NULL);
}
```

**Purpose**: Configure signal handlers.

**Why sigaction over signal()**:
- More portable behavior
- More control (flags, mask)
- Can block other signals during handler

---

## Signal States

### At Prompt (Interactive)
```
SIGINT (Ctrl+C):  Print newline, show new prompt
SIGQUIT (Ctrl+\): Do nothing
```

### During Command Execution
```
Parent (shell):
  SIGINT:  Ignored (wait for child)
  SIGQUIT: Ignored

Child (command):
  SIGINT:  Default (terminate)
  SIGQUIT: Default (terminate + core dump)
```

---

## Signal Handling in Child Processes

```c
// In execute_child_process()
signal(SIGINT, SIG_DFL);
signal(SIGQUIT, SIG_DFL);
```

**Why reset**: Child should respond normally to signals. If user presses Ctrl+C while `sleep 100` is running, it should interrupt the sleep.

---

## Signal Handling While Waiting

```c
// In handle_parent_process()
signal(SIGINT, SIG_IGN);   // Ignore while waiting
signal(SIGQUIT, SIG_IGN);

waitpid(pid, &status, 0);

signal(SIGINT, handle_sigint);    // Restore after
signal(SIGQUIT, handle_sigquit);
```

**Why ignore during wait**:
- Ctrl+C should kill the child, not the shell
- Shell needs to wait for child to finish
- Shell captures child's exit status

---

## Exit Status from Signals

```c
if (WIFSIGNALED(status))
{
    shell->exit_status = 128 + WTERMSIG(status);
    write(1, "\n", 1);
}
```

**Convention**: When process is killed by signal N, exit status is 128 + N.

| Signal | Number | Exit Status |
|--------|--------|-------------|
| SIGINT | 2 | 130 |
| SIGQUIT | 3 | 131 |
| SIGKILL | 9 | 137 |
| SIGTERM | 15 | 143 |

---

## Signal Flow Diagram

```
                    ┌─────────────────────────────────────┐
                    │         At Prompt (readline)         │
                    │                                      │
User presses        │  SIGINT ──► handle_sigint()         │
Ctrl+C              │              │                       │
                    │              ├─► g_signal = 2        │
                    │              ├─► print \n            │
                    │              └─► redisplay prompt    │
                    │                                      │
User presses        │  SIGQUIT ──► handle_sigquit()       │
Ctrl+\              │              │                       │
                    │              └─► (do nothing)        │
                    └─────────────────────────────────────┘


                    ┌─────────────────────────────────────┐
                    │     During Command Execution         │
                    │                                      │
                    │   Parent        Child                │
                    │   (shell)       (command)            │
                    │                                      │
User presses        │   Ignores       Terminates          │
Ctrl+C              │   SIGINT        with exit 130       │
                    │      │                │              │
                    │      │                ▼              │
                    │      │         exit(128+2)           │
                    │      │                │              │
                    │      ▼                │              │
                    │   waitpid() ◄─────────┘              │
                    │      │                               │
                    │      ▼                               │
                    │   shell->exit_status = 130           │
                    └─────────────────────────────────────┘
```

---

## Heredoc Signal Handling

```c
// Special handler for heredoc
void handle_sigint_heredoc(int sig)
{
    g_signal = sig;
    // Don't redisplay - just mark signal received
}
```

During heredoc input:
- Ctrl+C should cancel the heredoc
- Shell should return to prompt with exit status 130

---

## Key Concepts

### Why Minimal Signal Handlers?
Signal handlers run asynchronously, interrupting normal code. They should:
- Do as little as possible
- Avoid non-reentrant functions (most of libc)
- Use async-signal-safe functions only

`write()` is async-signal-safe. `printf()` is NOT.

### The readline() Problem
When signal interrupts `readline()`:
- `readline()` returns (possibly with partial input)
- We check `g_signal` after return
- If SIGINT, we discard input and handle it

### Deferred Signal Processing
```c
// In shell_loop()
check_signal(shell);  // Before reading
line = readline(...);
check_signal(shell);  // After reading
```

We check for signals at safe points in the main loop, not in the handler itself.

### SA_RESTART Flag
We don't use `SA_RESTART` because:
- We want `readline()` to return on signal
- We handle signals explicitly after system calls
- Gives us more control over behavior
