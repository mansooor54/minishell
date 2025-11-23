```bash
# 16-11-2025
    - handle is_numeric_overflow like bash:
        minishell> exit 99999999999999999999
        exit
        minishell: exit: 99999999999999999999: numeric argument required

    - and too many argument :
        minishell> exit 9 9 8 7
        exit
        minishell: exit: too many arguments

# 15-11-2025
    - Norminette Ok
    - Semicolone works

# 14-11-2025
    - fix signal handling in heredoc
    - fix memory leaks in heredoc
    - Norminette Ok
    - bash-3.2$ cat << EOF
    - remove semicolon operator test
```
# 13-11-2025

    ** This function:
    ** ^C is printed by the terminal itself (ECHOCTL), not by your handle_sigint.
        To make minishell behave like bash (no ^C shown), you must disable ECHOCTL 
        on the terminal.
    Reads current terminal settings.
    Turns off the ECHOCTL flag so the kernel stops printing ^C.
    Applies the new settings.
    No ternary operator is used.
    Press Ctrl-C at the prompt.
    You should now see only a new line and a fresh
    minishell> prompt, with no ^C, just like bash.


```bash
# 12-11-2025
    - update Parser folder and Makefile
    - Norminette Ok
    - bash-3.2$ ;
    - bash: syntax error near unexpected token `;'
    - bash-3.2$ \\ 
    - bash: \: command not found
```
