# Main Entry Point

## Files
- `src/main/main.c`
- `src/main/minishell_logo.c`

---

## What Happens When You Start Minishell?

**Real-life analogy**: Starting minishell is like opening a restaurant for the day:

```
1. Turn on the lights (clear screen)
2. Put up the "OPEN" sign (show logo)
3. Set up the kitchen (init_shell)
4. Train staff on emergency procedures (setup_signals)
5. Load yesterday's orders book (history_init)
6. Start taking orders! (shell_loop)
7. End of day: save order book (history_save)
8. Clean up everything (free memory)
9. Lock the doors (return exit_status)
```

---

## The Global Variable

```c
volatile sig_atomic_t g_signal = 0;
```

**What is this?**
Think of it as an **emergency alarm system**. When someone presses Ctrl+C:
1. The signal handler (like a smoke detector) immediately sets `g_signal = 2`
2. The main program (like the firefighter) checks this value and takes action

**Why only one global?**
42 School rules! And it's good practice - globals are hard to track.

**Why these special keywords?**

| Keyword | What it means | Real-life analogy |
|---------|---------------|-------------------|
| `volatile` | "Don't cache this, always check fresh value" | Like checking the actual clock, not your memory of what time it was |
| `sig_atomic_t` | "Read/write in one operation, no interruption" | Like flipping a light switch - it's instant, can't be half-on |

---

## main.c - Step by Step

### The main() Function

```c
int main(int argc, char **argv, char **envp)
{
    t_shell shell;

    (void)argc;
    (void)argv;
    write(1, "\033[2J\033[H", 7);  // Clear screen
    print_logo();                   // Display welcome banner
    init_shell(&shell, envp);       // Initialize shell state
    setup_signals();                // Configure signal handlers
    history_init(&shell);           // Load command history
    shell_loop(&shell);             // Main REPL loop
    history_save(&shell);           // Save history on exit
    rl_clear_history();             // Clear readline's history
    free_history(shell.history);    // Free our history list
    free_env(shell.env);            // Free environment
    free(shell.history_path);       // Free history file path
    return (shell.exit_status);     // Return last exit code
}
```

### Step 1: Silence Compiler Warnings
```c
(void)argc;
(void)argv;
```

**What**: Tells compiler "yes, I know I'm not using these, it's intentional"

**Why**: minishell doesn't need command-line arguments like:
```bash
./minishell -c "echo hello"  # We don't support this
./minishell script.sh        # We don't support this either
```

---

### Step 2: Clear the Screen
```c
write(1, "\033[2J\033[H", 7);
```

**What this does**:
```
\033[     = Start escape sequence (ANSI)
2J        = Clear entire screen
\033[     = Start another escape sequence
H         = Move cursor to top-left (home position)
```

**Visual**:
```
Before:                      After:
┌────────────────────┐       ┌────────────────────┐
│ old stuff          │       │                    │
│ more old stuff     │   →   │ █                  │
│ even more stuff    │       │                    │
└────────────────────┘       └────────────────────┘
                              cursor here
```

---

### Step 3: Show the Logo
```c
print_logo();
```

**What happens**:
```
███╗   ███╗██╗███╗   ██╗██╗███████╗██╗  ██╗███████╗██╗     ██╗
████╗ ████║██║████╗  ██║██║██╔════╝██║  ██║██╔════╝██║     ██║
...
Welcome to Mansoor & Afaf MiniShell for 42 School
```

**ANSI Color Codes**:
```c
"\033[1;36m"  // Bold + Cyan (for the main logo)
"\033[1;31m"  // Bold + Red
"\033[1;32m"  // Bold + Green
"\033[0m"     // Reset to default (important! otherwise everything stays colored)
```

---

### Step 4: Initialize the Shell
```c
init_shell(&shell, envp);
```

**What `envp` looks like**:
```
envp[0] = "HOME=/Users/mansoor"
envp[1] = "PATH=/usr/bin:/bin:/usr/local/bin"
envp[2] = "USER=mansoor"
envp[3] = "SHELL=/bin/zsh"
envp[4] = "TERM=xterm-256color"
...
envp[N] = NULL  ← End marker
```

**What init_shell does**:
```
envp (array)                    shell.env (linked list)
┌─────────────────────┐         ┌─────────────────────┐
│ "HOME=/Users/m..."  │   →     │ key: "HOME"         │
├─────────────────────┤         │ value: "/Users/m..." │
│ "PATH=/usr/bin:..." │         │ next ─────────────────┼──→ ...
├─────────────────────┤         └─────────────────────┘
│ "USER=mansoor"      │
├─────────────────────┤
│ NULL                │
└─────────────────────┘

Also sets:
  shell.interactive = 1 (if connected to terminal)
  shell.exit_status = 0
  shell.should_exit = 0
```

---

### Step 5: Setup Signal Handlers
```c
setup_signals();
```

**What this configures**:

| Signal | Key | What happens |
|--------|-----|--------------|
| SIGINT | Ctrl+C | Shows new prompt (doesn't quit shell) |
| SIGQUIT | Ctrl+\ | Ignored (does nothing) |

**Example behavior**:
```
minishell> sleep 100
^C                      ← User presses Ctrl+C
                        ← New line
minishell>              ← Fresh prompt, shell keeps running!
```

---

### Step 6: Load History
```c
history_init(&shell);
```

**What happens**:
```
Reads file: ~/.minishell_history

If file exists:
  ┌─────────────────────┐
  │ ls -la              │  ← Previous command
  │ cd /tmp             │  ← Previous command
  │ echo hello          │  ← Previous command
  └─────────────────────┘
        ↓
  Loads into readline so ↑↓ arrows work!
```

---

### Step 7: The Main Loop
```c
shell_loop(&shell);
```

**This is where 99% of the work happens!**

```
┌──────────────────────────────────────────────┐
│                SHELL LOOP                     │
│                                              │
│   ┌──────────────────────────────────────┐   │
│   │  1. Show prompt: minishell>          │   │
│   │  2. User types: ls -la              │   │
│   │  3. Lexer: "ls" "-la" (tokens)      │   │
│   │  4. Parser: cmd{args: [ls, -la]}    │   │
│   │  5. Expander: (no $ to expand)      │   │
│   │  6. Executor: fork + execve         │   │
│   │  7. Display output                   │   │
│   │  8. Go back to step 1               │   │
│   └──────────────────────────────────────┘   │
│                                              │
│   Loop ends when: exit command or Ctrl+D     │
└──────────────────────────────────────────────┘
```

---

### Step 8-9: Cleanup and Exit
```c
history_save(&shell);           // Save commands to file
rl_clear_history();             // Tell readline to forget everything
free_history(shell.history);    // Free our own history list
free_env(shell.env);            // Free environment linked list
free(shell.history_path);       // Free the path string
return (shell.exit_status);     // Return last command's exit code
```

**Why cleanup matters**:
```
Before cleanup:          After cleanup:
┌─────────────────┐      ┌─────────────────┐
│ Memory: 50 MB   │      │ Memory: 0 MB    │
│ (all our data)  │  →   │ (returned to    │
│                 │      │  operating      │
│                 │      │  system)        │
└─────────────────┘      └─────────────────┘
```

Valgrind will complain if we don't free everything!

---

## Complete Flow Diagram

```
     ./minishell
          │
          ▼
    ┌─────────────┐
    │   main()    │
    └──────┬──────┘
           │
    ┌──────▼──────┐
    │ Clear Screen │  ← write("\033[2J\033[H")
    └──────┬──────┘
           │
    ┌──────▼──────┐
    │ print_logo() │  ← ASCII art banner
    └──────┬──────┘
           │
    ┌──────▼──────┐
    │ init_shell() │  ← Convert envp to linked list
    │              │     Set shell.interactive
    │              │     Set exit_status = 0
    └──────┬──────┘
           │
    ┌──────▼────────┐
    │ setup_signals()│ ← SIGINT → handle_sigint
    │               │    SIGQUIT → handle_sigquit
    └──────┬────────┘
           │
    ┌──────▼────────┐
    │ history_init()│  ← Load ~/.minishell_history
    └──────┬────────┘
           │
    ┌──────▼────────┐
    │  shell_loop() │  ← THE MAIN REPL LOOP
    │               │    (Read-Eval-Print-Loop)
    │  ┌─────────┐  │
    │  │ prompt  │  │
    │  │ lexer   │◄─┼── User types commands
    │  │ parser  │  │
    │  │ expand  │  │
    │  │ execute │  │
    │  └─────────┘  │
    │       ↻       │   ← Loops until exit/Ctrl+D
    └──────┬────────┘
           │
    ┌──────▼────────┐
    │ history_save()│  ← Save commands to file
    └──────┬────────┘
           │
    ┌──────▼────────┐
    │   Cleanup     │  ← Free all memory
    └──────┬────────┘
           │
    ┌──────▼────────┐
    │    return     │  ← Exit with last command's code
    │  exit_status  │
    └───────────────┘
```

---

## The envp Parameter - Deep Dive

### What is it?
The third parameter to main contains ALL environment variables from parent shell:

```c
int main(int argc, char **argv, char **envp)
```

### Example of what's inside:
```
If you run from zsh with some exports:
$ export MY_VAR=hello
$ ./minishell

Then envp contains:
envp[0]  = "HOME=/Users/mansoor"
envp[1]  = "PATH=/usr/bin:/bin:/usr/local/bin"
envp[2]  = "USER=mansoor"
envp[3]  = "SHELL=/bin/zsh"
envp[4]  = "MY_VAR=hello"        ← Your custom export!
...
envp[42] = "TERM=xterm-256color"
envp[43] = NULL                   ← Always ends with NULL
```

### Why convert to linked list?
```
ARRAY (harder to modify):           LINKED LIST (easy to modify):
┌────────────────────┐              ┌────────┐   ┌────────┐
│ "HOME=/Users/..."  │              │ HOME   │──►│ PATH   │──►...
├────────────────────┤              │ /User..│   │ /usr.. │
│ "PATH=/usr/..."    │      →       └────────┘   └────────┘
├────────────────────┤
│ NULL               │              To add: just malloc and link
└────────────────────┘              To remove: just unlink and free
To add: reallocate whole array!
To remove: shift everything!
```

---

## Why Return exit_status?

When you run minishell from another shell:

```bash
$ ./minishell
minishell> exit 42
exit
$ echo $?
42          ← The 42 came from return (shell.exit_status)!
```

This is how shell scripts can check if minishell succeeded:
```bash
if ./minishell -c "test_command"; then
    echo "Success!"
else
    echo "Failed with code $?"
fi
```

---

## Common Questions

### Q: Why do we need both `rl_clear_history()` and `free_history()`?

**A**: They clean up different things:
- `rl_clear_history()` - Cleans up **readline library's** internal storage
- `free_history()` - Cleans up **our own** history linked list

### Q: What if envp is empty?

**A**: The shell still works! Commands like `ls` just won't be found by PATH search:
```
minishell> ls
minishell: ls: command not found
minishell> /bin/ls    ← Full path still works!
```

### Q: Why clear the screen at startup?

**A**: It's optional/cosmetic! It gives a clean slate feeling when starting minishell. Some people comment this out.
