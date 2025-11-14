# Minishell - 42 School Project

A simple shell implementation in C, as beautiful as a shell.

## Project Description

This project implements a basic Unix shell that can:
- Display a prompt and execute commands
- Maintain command history
- Handle pipes and redirections
- Expand environment variables
- Implement built-in commands
- Handle signals (Ctrl-C, Ctrl-D, Ctrl-\)

## Project Structure

```
.
├── main.c                      # Main entry point (in root)
├── minishell.h                 # Main header file (in root)
├── Makefile                    # Build configuration
├── src/                        # Source files directory
│   ├── lexer/                  # Tokenization
│   ├── parser/                 # Command parsing
│   ├── expander/               # Variable expansion
│   ├── executor/               # Command execution
│   ├── builtins/               # Built-in commands
│   ├── signals/                # Signal handling
│   └── utils/                  # Utility functions
└── libft/                      # Custom C library
```

### compine all files in one:
cat src/lexer/*.c src/parser/*.c src/executor/*.c src/builtins/*.c src/core/*.c src/environment/*.c src/expander/*.c src/history/*.c src/signals/*.c src/utils/*.c main.c minishell_logo.c > minishell_all_code.c

### Remove repeat include:
grep -h '^#include' src/**/*.c | sort -u > tmp_includes.h\ngrep -hv '^#include' libft/*.c >> tmp_includes.h\nmv tmp_includes.h merged_libft_small.c\n

## Compilation

```bash
make        # Compile the project
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Recompile from scratch
```

## Features

### Mandatory Part

#### Core Functionality
- Interactive prompt display
- Working command history (using readline)
- Executable search based on PATH or absolute/relative paths
- Quote handling:
  - Single quotes (`'`) prevent interpretation of meta-characters
  - Double quotes (`"`) prevent interpretation except for `$`

#### Redirections
- `<` : Input redirection
- `>` : Output redirection
- `<<` : Heredoc (read until delimiter)
- `>>` : Append output redirection

#### Pipes
- `|` : Pipe character connecting command outputs to inputs

#### Environment Variables
- `$VARIABLE` : Expands to variable value
- `$?` : Expands to exit status of last command

#### Signal Handling
- `Ctrl-C` : Display new prompt on new line
- `Ctrl-D` : Exit the shell
- `Ctrl-\` : Do nothing

#### Built-in Commands
- `echo` with `-n` option
- `cd` with relative or absolute path
- `pwd` (no options)
- `export` (no options)
- `unset` (no options)
- `env` (no options or arguments)
- `exit` (no options)

### Bonus Part (if mandatory is perfect)
- `&&` and `||` with parenthesis for priorities
- Wildcards `*` for current working directory

## Implementation Details

### Global Variable
The project uses only **one global variable** (`g_shell`) as required by the subject.
This global is necessary for signal handlers to access shell state.

### Memory Management
- All heap-allocated memory is properly freed
- No memory leaks in user code (readline may have its own leaks)
- Proper error handling throughout

### External Functions Used
As specified in the subject:
- readline, rl_clear_history, rl_on_new_line, rl_replace_line, rl_redisplay, add_history
- printf, malloc, free, write, access, open, read, close
- fork, wait, waitpid, wait3, wait4, signal, sigaction, sigemptyset, sigaddset, kill, exit
- getcwd, chdir, stat, lstat, fstat, unlink, execve
- dup, dup2, pipe, opendir, readdir, closedir
- strerror, perror, isatty, ttyname, ttyslot, ioctl, getenv
- tcsetattr, tcgetattr, tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs

# Testing Results - All Passed!
```bash
✓ Basic commands (ls, pwd, echo)
✓ Built-in commands (cd, export, env, exit)
✓ Pipes (ls | grep Make)
✓ Redirections (echo test > file.txt)
✓ Environment variables ($TEST expansion)
✓ Directory navigation (cd -, cd /tmp)
```

## Usage Examples

```bash
$ ./minishell
minishell> echo "Hello World"
Hello World
minishell> ls -la | grep minishell
minishell> cat < input.txt > output.txt
minishell> export MY_VAR=value
minishell> echo $MY_VAR
value
minishell> echo $?
0
minishell> cd /tmp
minishell> pwd
/tmp
minishell> exit
```

## Function Documentation

Each function in the project includes detailed comments explaining:
- **Purpose**: What the function does
- **Parameters**: Input parameters and their meanings
- **Return value**: What the function returns
- **Description**: Detailed behavior and implementation notes

See individual source files for comprehensive function documentation.

## Compliance with 42 Subject

This implementation follows all requirements specified in the minishell subject (version 7.1):
- ✅ Written in C
- ✅ Follows Norm (if applicable)
- ✅ No unexpected crashes (segfault, bus error, double free, etc.)
- ✅ Proper memory management
- ✅ Makefile with required rules and flags (-Wall -Wextra -Werror)
- ✅ Uses only allowed external functions
- ✅ Includes libft
- ✅ One global variable for signal handling
- ✅ All mandatory features implemented

## Authors

42 School Student Project

## Version
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
✅ Final Comprehensive Test Suite
Now that the basic tests work, let's test more advanced scenarios:

```bash
Minishell Test Suite (42 Subject Compliance)
1. Prompt and Basic Command Execution
1.1 Prompt and simple commands
./minishell
<press Enter>
echo hello


Expected:

Prompt always appears.
Empty line reprints prompt.
echo hello → hello.

# 1.2 Exit status
true                            # minishell>
echo $?                         # minishell> 0
false                           # minishell> 
echo $?                         # minishell> 1
ls /this/path/does/not/exist    # ls: /this/path/does/not/exist: No such file or directory
echo $?                         # minishell> 1

Expected:
Correct exit statuses (0 for success, non-zero for errors).

# 2. History
# 2.1 Arrow key navigation
echo one                        # minishell> one
echo two                        # minishell> tow
echo three                      # minishell> one
<Up arrow multiple times>


Expected:
three → two → one

# 3. PATH Resolution
# 3.1 PATH search
ls
which ls
echo $PATH


Expected:
ls works through PATH.

# 3.2 Relative path
cd /tmp
echo 'echo HI' > myscript.sh
chmod +x myscript.sh
./myscript.sh

# 3.3 Absolute path
/bin/echo ABSOLUTE

# 3.4 Command not found
thiscommanddoesnotexist


Expected:
minishell: ... command not found

# 4. Quotes and Special Characters
# 4.1 Single quotes
echo '$HOME'


Expected:
Literal $HOME.

# 4.2 Double quotes
echo "$HOME"

#4.3 Mixed quotes
echo "HOME=" $HOME
echo "HOME='$HOME'"
echo '$HOME'"$HOME"

# 4.4 Unclosed quotes
echo "hello

"

Expected:
Syntax error OR waiting for closing quote (your choice, must not execute partial command).

# 4.5 Semicolon and backslash
\ls
;
echo hi\there

# Semicolon
minishell> ;
minishell: syntax error near unexpected token `;'

minishell> echo hi ;
minishell: syntax error near unexpected token `;'

minishell> echo hi ;; echo bye
minishell: syntax error near unexpected token `;'

minishell> echo hi ; echo bye
hi
bye

minishell> echo hi ; && echo bye
minishell: syntax error near unexpected token `&&'


Expected:
Consistent behavior (not required by subject).

# 5. Redirections
# 5.1 Output >
echo "hello" > out1
cat out1

# 5.2 Append >>
echo "line1" > out2
echo "line2" >> out2
cat out2

# 5.3 Input <
printf "line A\nline B\n" > in1
cat < in1

# 5.4 Combined < and >
cat < in1 > out3
cat out3

# 5.5 Here-document <<
# Unquoted delimiter
cat << EOF
line $HOME
EOF

# Quoted delimiter
cat << "EOF"
line $HOME
EOF


Expected:
Quoted delimiter → no variable expansion.

# 5.6 Redirection syntax errors
>
>>
<<<
<<
echo hi >

# 6. Pipes
# 6.1 Simple pipe
echo "hello" | cat

# 6.2 Multi-pipe
echo "test" | cat | cat | cat

# 6.3 Pipe heavy example
ls -1 | grep '\.c' | wc -l

# 6.4 Redirect + pipe
echo "abc" | cat > piped_out
cat piped_out

# 7. Environment Variables
# 7.1 Basic expansion
echo $HOME
echo "$HOME"
echo '$HOME'

# 7.2 Undefined variable
echo $DOES_NOT_EXIST

# 7.3 $?
false
echo $?
true
echo $?
ls /no/such/file
echo $?

# 7.4 Combining text
echo "status=$?"

# 8. Signals (Interactive Only)
# 8.1 Ctrl-C at prompt

Press Ctrl-C
Expected:
New prompt + exit code 130.

# 8.2 Ctrl-C during a command
sleep 10
press Ctrl-C

# 8.3 Ctrl-D
press Ctrl-D
Expected:
Exit minishell.

8.4 Ctrl-\
press Ctrl-\
Expected:
Do nothing.

# 9. Builtins
# 9.1 echo
echo hello world
echo -n hello
echo -n -n -n abc
echo "-n" hello

# 9.2 cd
pwd
cd ..
pwd
cd /
pwd
cd /no/such/dir
echo $?

# 9.3 pwd
pwd
/bin/pwd

# 9.4 export
export TESTVAR=hello
echo $TESTVAR
export TESTVAR=world
echo $TESTVAR
export ABC
env | grep ABC
export

# 9.5 unset
export TEMPVAR=123
echo $TEMPVAR
unset TEMPVAR
echo $TEMPVAR

# 9.6 env
env
env | grep HOME
env BADARG

# 9.7 exit
# Numeric:
exit 42

# Overflow:
exit 99999999999999999999 //diffrent result in bash

# Too many args:
exit 1 2

# 10. Non-required Features (Ensure no crash)
# 10.1 && and ||
true && echo yes
false || echo yes

# 10.2 Semicolon //diffrent result in bash
echo hi ; echo bye
;

# 11. How to Use This Test Plan

Run each test in bash.

Run the same in minishell.

Compare:

Output

Exit status (echo $?)

File contents

Ensure no crashes or memory leaks (your code only).
```

✅ Final Comprehensive Test Suite
Now that the basic tests work, let's test more advanced scenarios:

```bash
# Test qoute
echo "A\"B"            # A"B
echo "x\"\"y"          # x""y
echo "keep \\ back"    # keep \ back
echo "\$HOME"          # $HOME
echo "$HOME"           # expands
echo '$HOME'           # $HOME
echo 'A'\''B'          # A'B
echo "Test Hello"\n "hello"   # Test Hellon hello



#1. Complex Variable Expansion
minishell> export VAR1="Hello"
minishell> export VAR2="World"
minishell> echo $VAR1 $VAR2
Hello World

minishell> echo "$VAR1 $VAR2"
Hello World

minishell> echo '$VAR1 $VAR2'
$VAR1 $VAR2

minishell> echo $VAR1$VAR2
HelloWorld
#2. Exit Status Chain
minishell> ls
# ... files listed ...
minishell> echo $?
0

minishell> ls /nonexistent
ls: /nonexistent: No such file or directory
minishell> echo $?
1

minishell> echo $?
0

minishell> false
minishell> echo $?
1

minishell> true
minishell> echo $?
0

#3. SHLVL Nesting

# Terminal
$ echo $SHLVL
1

# First minishell
$ ./minishell
minishell> echo $SHLVL
2
minishell> env | grep SHLVL
SHLVL=2

# Second minishell (nested)
minishell> ./minishell
minishell> echo $SHLVL
3
minishell> env | grep SHLVL
SHLVL=3

# Exit back
minishell> exit
minishell> echo $SHLVL
2
minishell> exit
$ echo $SHLVL
1
4. Redirections Stress Test
Bash
# Create test files
minishell> echo "Test 1" > test1.txt
minishell> echo "Test 2" > test2.txt
minishell> echo "Test 3" > test3.txt

# Multiple input redirections
minishell> cat < test1.txt < test2.txt < test3.txt
Test 3

# Multiple output redirections
minishell> echo "Final" > out1.txt > out2.txt > out3.txt
minishell> cat out3.txt
Final
minishell> cat out1.txt 2>/dev/null || echo "out1.txt doesn't exist (correct)"
out1.txt doesn't exist (correct)

# Append redirections
minishell> echo "Line 1" > append.txt
minishell> echo "Line 2" >> append.txt
minishell> echo "Line 3" >> append.txt
minishell> cat append.txt
Line 1
Line 2
Line 3
5. Pipes with Redirections
Bash
minishell> echo "hello world" | cat > piped.txt
minishell> cat piped.txt
hello world

minishell> cat test1.txt | cat | cat | cat
Test 1

minishell> echo "test" | cat | cat > multi_pipe.txt
minishell> cat multi_pipe.txt
test
# 6. Builtin Commands
# pwd
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell

# cd
minishell> cd src
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell/src
minishell> cd ..
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell

# export
minishell> export TEST="value with spaces"
minishell> echo $TEST
value with spaces

# unset
minishell> unset TEST
minishell> echo $TEST

minishell> echo $?
0

# env
minishell> env | grep USER
USER=malmarzo

# echo with -n
minishell> echo -n "no newline"
no newlineminishell> 
minishell> echo "with newline"
with newline
7. Edge Cases
Bash
# Empty variable
minishell> echo $NONEXISTENT_VAR
<empty line>

# Dollar at end
minishell> echo test$
test$

# Multiple dollars
minishell> echo $$
<process id>

# Variable in middle of word
minishell> export NAME="John"
minishell> echo Hello$NAME!
HelloJohn!

# Empty command
minishell> 
minishell> 

# Only spaces
minishell>      
minishell> 
8. Quotes Edge Cases
Bash
minishell> echo "test"
test

minishell> echo 'test'
test

minishell> echo "test'test"
test'test

minishell> echo 'test"test'
test"test

minishell> echo "$USER's home is $HOME"
malmarzo's home is /Users/malmarzo

minishell> echo '$USER'"'"'s home is $HOME'
$USER's home is $HOME
9. Signal Handling
Bash
# Ctrl+C during command
minishell> sleep 5
^C
minishell> echo $?
130

# Ctrl+C at prompt
minishell> ^C
minishell> 

# Ctrl+D to exit
minishell> [Press Ctrl+D]
$
10. Memory Leak Test
If you're on Linux with valgrind:
Bash
$ valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell
minishell> echo $USER
malmarzo
minishell> echo $?
0
minishell> cat < test1.txt < test2.txt < test3.txt
Test 3
minishell> echo "test" | cat | cat
test
minishell> exit

# Check valgrind output - should show:
# "All heap blocks were freed -- no leaks are possible"
# Or minimal leaks from readline library (which is acceptable)
11. Performance Test
Bash
# Test with many redirections
minishell> cat < test1.txt < test2.txt < test3.txt < test1.txt < test2.txt
Test 2

# Long pipeline
minishell> echo "test" | cat | cat | cat | cat | cat | cat | cat | cat
test

# Many variables
minishell> export A=1 B=2 C=3 D=4 E=5
minishell> echo $A $B $C $D $E
1 2 3 4 5
12. Error Handling
Bash
# Command not found
minishell> nonexistent_command
minishell: nonexistent_command: command not found
minishell> echo $?
127

# Permission denied (if you have a file without execute permission)
minishell> ./no_execute_permission_file
minishell: ./no_execute_permission_file: Permission denied
minishell> echo $?
126

# Incomplete redirections (should NOT crash)
minishell> echo test >
minishell: syntax error near unexpected token `newline'
minishell> echo $?
2

minishell> cat <
minishell: syntax error near unexpected token `newline'
🎯 Summary of What's Fixed
✅ Bug #1: Parser segfault on incomplete redirections - FIXED

✅ Bug #2: Buffer overflow in variable expansion - FIXED

✅ Bug #3: Memory leak in redirection parsing - FIXED

✅ SHLVL: Correctly increments with nested shells - WORKING

✅ Variable expansion: $? and $VAR work correctly - WORKING

✅ Redirections: Multiple redirections handled properly - WORKING
📝 Next Steps (Optional Improvements)
If you want to make your minishell even better, consider:
Heredoc with variable expansion (if delimiter is unquoted)
Wildcards (* expansion)
Logical operators (&& and ||)
Subshells ((command))
Better error messages (more descriptive)
```
