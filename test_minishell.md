
Here's a comprehensive test suite to verify your minishell is working correctly, especially after the bug fixes:
🧪 Complete Test Suite for Minishell
1. Basic Commands
Bash
# Test simple commands
```bash
minishell> echo hello world
hello world

minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell

minishell> ls
Makefile  README.md  minishell  src  obj

minishell> whoami
malmarzo
2. Environment Variables
Bash
```
# Test $? (exit status)
minishell> echo $?  ====> zsh: segmentation fault  ./minishell
0

minishell> ls /nonexistent
ls: /nonexistent: No such file or directory
minishell> echo $?  ====> zsh: segmentation fault  ./minishell
1

# Test variable expansion
minishell> echo $USER  ====> zsh: segmentation fault  ./minishell
malmarzo

minishell> echo $HOME
/Users/malmarzo

minishell> echo Hello $USER, your home is $HOME
Hello malmarzo, your home is /Users/malmarzo
3. SHLVL Testing
Bash
# In your terminal
$ echo $SHLVL   ====> zsh: segmentation fault  ./minishell
1

# Start minishell
$ ./minishell
minishell> echo $SHLVL  ====> zsh: segmentation fault  ./minishell
2

# Start another minishell inside
minishell> ./minishell
minishell> echo $SHLVL
3

# Check with env
minishell> env | grep SHLVL
SHLVL=3

# Exit and verify it goes back
minishell> exit
minishell> echo $SHLVL
2
4. Redirections (Bug #1 Fix - No Segfault)
Bash
# Test incomplete redirections (should NOT crash)
minishell> ls >
minishell: syntax error near unexpected token `newline'

minishell> cat <
minishell: syntax error near unexpected token `newline'

minishell> echo hello >>
minishell: syntax error near unexpected token `newline'

# Test valid redirections
minishell> echo "Hello World" > test.txt
minishell> cat test.txt
Hello World

minishell> echo "Line 2" >> test.txt
minishell> cat test.txt
Hello World
Line 2

minishell> cat < test.txt
Hello World
Line 2

minishell> cat < test.txt > output.txt
minishell> cat output.txt
Hello World
Line 2
5. Multiple Redirections (Bug #3 Fix - No Memory Leak)
Bash
# Create test files
minishell> echo "File 1" > file1.txt
minishell> echo "File 2" > file2.txt
minishell> echo "File 3" > file3.txt

# Test multiple input redirections (last one wins)
minishell> cat < file1.txt < file2.txt < file3.txt
File 3

# Test multiple output redirections (last one wins)
minishell> echo "Test" > out1.txt > out2.txt > out3.txt
minishell> cat out1.txt
cat: out1.txt: No such file or directory
minishell> cat out2.txt
cat: out2.txt: No such file or directory
minishell> cat out3.txt
Test

# Mixed redirections
minishell> cat < file1.txt > result.txt >> result2.txt
minishell> cat result2.txt
File 1
6. Variable Expansion (Bug #2 Fix - No Buffer Overflow)
Bash
# Test large variable expansion
minishell> export BIG_VAR="AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
minishell> echo $BIG_VAR
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

# Test multiple variable expansions
minishell> export A="Hello"
minishell> export B="World"
minishell> echo $A $B $A $B
Hello World Hello World

# Test variable in quotes
minishell> echo "$USER is in $HOME"
malmarzo is in /Users/malmarzo

minishell> echo '$USER is in $HOME'
$USER is in $HOME
7. Pipes
Bash
# Simple pipe
minishell> echo "hello world" | cat
hello world

# Multiple pipes
minishell> ls | grep minishell | cat
minishell

# Pipe with redirections
minishell> echo "test" | cat > piped.txt
minishell> cat piped.txt
test

# Long pipeline
minishell> cat file1.txt | grep "File" | cat | cat
File 1
8. Builtins
Bash
# cd
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell
minishell> cd src
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell/src
minishell> cd ..
minishell> pwd
/Users/malmarzo/Desktop/My_minishell/minishell

# export and unset
minishell> export MY_VAR="test value"
minishell> echo $MY_VAR
test value
minishell> unset MY_VAR
minishell> echo $MY_VAR

# env
minishell> env | grep USER
USER=malmarzo

# exit
minishell> exit
$
9. Quotes
Bash
# Single quotes (no expansion)
minishell> echo '$USER'
$USER

# Double quotes (with expansion)
minishell> echo "$USER"
malmarzo

# Mixed quotes
minishell> echo "Hello '$USER'"
Hello '$USER'

minishell> echo 'Hello "$USER"'
Hello "$USER"
10. Edge Cases
Bash
# Empty command
minishell> 
minishell> 

# Multiple spaces
minishell> echo    hello     world
hello world

# Command not found
minishell> nonexistent_command
minishell: nonexistent_command: command not found
minishell> echo $?
127

# Empty variable
minishell> echo $NONEXISTENT

minishell> echo "Value: $NONEXISTENT end"
Value:  end
11. Signals
Bash
# Ctrl+C (should not exit, just new prompt)
minishell> sleep 10
^C
minishell> echo $?
130

# Ctrl+D (should exit gracefully)
minishell> [Ctrl+D]
$

# Ctrl+\ (should be ignored in interactive mode)
minishell> ^\
minishell>
12. Heredoc
Bash
# Basic heredoc
minishell> cat << EOF
> line 1
> line 2
> line 3
> EOF
line 1
line 2
line 3

# Heredoc with variable expansion
minishell> cat << EOF
> Hello $USER
> Your home is $HOME
> EOF
Hello malmarzo
Your home is /Users/malmarzo
13. Stress Test (Memory Leaks)
Bash
# Run these multiple times to check for memory leaks
minishell> cat < file1.txt < file2.txt < file3.txt
minishell> cat < file1.txt < file2.txt < file3.txt
minishell> cat < file1.txt < file2.txt < file3.txt

# Test with valgrind (if on Linux)
$ valgrind --leak-check=full --show-leak-kinds=all ./minishell
minishell> cat < file1.txt < file2.txt < file3.txt
minishell> exit
# Should show no leaks from your code
14. Exit Status
Bash
# Test exit codes
minishell> ls
minishell> echo $?
0

minishell> ls /nonexistent
ls: /nonexistent: No such file or directory
minishell> echo $?
1

minishell> exit 42
$ echo $?
42
15. Complex Scenarios
Bash
# Combination of features
minishell> export NAME="John"
minishell> echo "Hello $NAME" | cat > greeting.txt
minishell> cat < greeting.txt | cat | cat
Hello John

# Multiple commands
minishell> echo "test1" > file.txt
minishell> cat file.txt
test1
minishell> echo "test2" >> file.txt
minishell> cat file.txt
test1
test2
📊 Expected Results Summary
Test
Expected Behavior
Incomplete redirections
Error message, no crash
Multiple redirections
Last redirection wins, no memory leak
Large variables
No buffer overflow, correct output
SHLVL
Increments with each nested shell
Pipes
Correct data flow between commands
Signals
Ctrl+C shows new prompt, Ctrl+D exits
Exit status
Correctly stored in $?
🐛 What to Look For
No segfaults - Shell should never crash
No memory leaks - Run with valgrind to verify
Correct output - Compare with bash behavior
Proper cleanup - All file descriptors closed, memory freed