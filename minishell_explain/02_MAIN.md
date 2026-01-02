# Main Entry Point

## Files
- `src/main/main.c`
- `src/main/minishell_logo.c`

---

## main.c

### Global Variable
```c
volatile sig_atomic_t g_signal = 0;
```

**What**: The only global variable allowed by 42 subject.

**Why**: Signal handlers cannot access local variables. This stores the signal number (e.g., SIGINT=2) so the main program can check what signal was received.

**Why `volatile sig_atomic_t`**:
- `volatile`: Tells compiler the variable can change unexpectedly (by signal handler)
- `sig_atomic_t`: Guarantees atomic read/write operations, preventing race conditions

---

### main()
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

**Purpose**: Entry point that initializes the shell and starts the main loop.

**Step-by-step**:

1. **`(void)argc; (void)argv;`**
   - Silences "unused parameter" warnings
   - minishell doesn't use command-line arguments

2. **`write(1, "\033[2J\033[H", 7);`**
   - ANSI escape sequence to clear terminal screen
   - `\033[2J` = clear entire screen
   - `\033[H` = move cursor to home position (0,0)

3. **`print_logo()`**
   - Displays the colorful ASCII art banner
   - Pure cosmetic, welcoming the user

4. **`init_shell(&shell, envp)`**
   - Creates t_shell structure from system's environment
   - Sets interactive mode based on TTY detection
   - Initializes exit_status to 0

5. **`setup_signals()`**
   - Configures SIGINT (Ctrl+C) and SIGQUIT (Ctrl+\) handlers
   - Shell must handle these specially

6. **`history_init(&shell)`**
   - Loads command history from file (~/.minishell_history)
   - Enables arrow-key navigation of previous commands

7. **`shell_loop(&shell)`**
   - THE MAIN LOOP - reads input, parses, executes
   - Runs until `should_exit` flag is set or EOF (Ctrl+D)

8. **Cleanup sequence**:
   - `history_save()` - persist history to disk
   - `rl_clear_history()` - cleanup readline's internal list
   - `free_history()` - free our own history linked list
   - `free_env()` - free environment linked list
   - `free(shell.history_path)` - free the path string

9. **`return (shell.exit_status)`**
   - Returns the exit code of the last command
   - This becomes minishell's exit code

---

## minishell_logo.c

### print_logo()
```c
void print_logo(void)
{
    printf("\033[1;36m");  // Set cyan color
    printf("███╗   ███╗██╗...");  // ASCII art
    // ... more lines ...
    printf("Welcome to Mansoor & Afaf MiniShell for 42 School\n");
}
```

**Purpose**: Displays a colorful welcome banner when minishell starts.

**ANSI Color Codes Used**:
| Code | Color |
|------|-------|
| `\033[1;36m` | Bold Cyan |
| `\033[1;31m` | Bold Red |
| `\033[1;32m` | Bold Green |
| `\033[1;37m` | Bold White |
| `\033[1;30m` | Bold Black |
| `\033[1;33m` | Bold Yellow |
| `\033[0m` | Reset to default |

**Why**: Creates a professional, welcoming appearance when users start the shell. This is purely cosmetic and doesn't affect functionality.

---

## Initialization Sequence Diagram

```
main() starts
    │
    ├─► Clear screen (ANSI escape)
    │
    ├─► print_logo() ─► Display ASCII banner
    │
    ├─► init_shell() ─► Create t_shell
    │       │
    │       ├─► init_env() ─► Parse envp into linked list
    │       ├─► Set interactive flag (isatty check)
    │       └─► Initialize exit_status = 0
    │
    ├─► setup_signals() ─► Configure handlers
    │       │
    │       ├─► SIGINT → handle_sigint
    │       └─► SIGQUIT → handle_sigquit
    │
    ├─► history_init() ─► Load ~/.minishell_history
    │
    ├─► shell_loop() ─► MAIN REPL (see 03_CORE.md)
    │       │
    │       └─► (runs until exit or EOF)
    │
    ├─► history_save() ─► Write history to file
    │
    ├─► Cleanup (free all memory)
    │
    └─► return exit_status
```

---

## Key Concepts

### Why envp Parameter?
```c
int main(int argc, char **argv, char **envp)
```

The third parameter `envp` contains the environment variables inherited from the parent process. Each entry is a "KEY=VALUE" string:
```
envp[0] = "HOME=/Users/john"
envp[1] = "PATH=/usr/bin:/bin"
envp[2] = "SHELL=/bin/bash"
...
```

We parse this into a linked list (`t_env`) for easier manipulation.

### Why Return exit_status?
When you run minishell from another shell:
```bash
./minishell
exit 42
echo $?  # Shows 42
```

The return value becomes the exit code visible to the parent process. This is how shell scripts and parent processes know if minishell succeeded or failed.
