```bash
# 25-12-2025
   - All done. Here's a summary of the changes made: Removed Semicolon (;) Handling:
   - Removed try_semicolon() call from lexer_operator.c
   - Removed check_semicolon() function from parser_syntax_utils.c
   - Removed check_semicolon() call from parser_syntax_check.c
   - Removed try_semicolon and check_semicolon declarations from minishell.h
   - Removed Backslash (\) Handling:
   - Simplified expander_quotes.c - removed backslash escape handling
   - Simplified read_logical_line_utils.c - removed trailing backslash continuation
   - Simplified join_continuation.c - removed backslash removal logic
   - Simplified lexer_unclose.c - removed escape logic in quote checking
   - Simplified expander_utils.c - removed \$ escape handling
   - Test Results:
   - echo hello; world → outputs hello; world (semicolon treated as regular character)
   - echo hello\world → outputs hello\world (backslash treated as regular character)
   - Compilation successful
   - Norminette passed on all modified files

# 24-12-2025
    - Check leaks with export/unset
    - Check leaks with redirections
    - Check leaks with pipes
    - Check leaks with cd
    - Check leaks with env
    - Check leaks with quotes
    - Check leaks with error command
    - modify history
    - remove an allowed function and write own functions
    
# 24-12-2025
ملخص التغييرات:
قبل:

t_shell g_shell;  // struct كامل - ⚠️ مخالف للقواعد
بعد:

volatile sig_atomic_t g_signal = 0;  // رقم signal فقط - ✅ صحيح
الملفات المعدّلة:
minishell.h - تغيير extern t_shell g_shell إلى extern volatile sig_atomic_t g_signal
main.c - t_shell shell أصبح local variable، وأضيف volatile sig_atomic_t g_signal = 0
signals.c - الآن يضع فقط g_signal = sig في الـ handler
shell_loop.c - يفحص g_signal ويضبط shell->exit_status = 130 عند SIGINT
read_logical_line.c - يستخدم g_signal بدلاً من g_shell.sigint_during_read
shell_utils.c - يمرر shell pointer للدوال التي تحتاج exit_status
parser_syntax_*.c - أزيل استخدام g_shell.exit_status، يُضبط في shell_utils بدلاً
expander_pipeline.c - أضيف parameter exit_status للـ expander()
executor_redir_heredoc.c* - يستخدم g_signal للـ signal handling
الآن المشروع يتوافق مع متطلب 42:
"This global variable cannot provide any other information or data access than the number of a received signal"

# 24-12-2025
    - Fix error messege for permission denied 
    - Fix error for No such file or directory
    - minishell_Evaluation_v04.zip

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

# 12-11-2025
    - update Parser folder and Makefile
    - Norminette Ok
    - bash-3.2$ ;
    - bash: syntax error near unexpected token `;'
    - bash-3.2$ \\ 
    - bash: \: command not found
```
