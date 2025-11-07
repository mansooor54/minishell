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

1. Complex Variable Expansion
Bash
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
2. Exit Status Chain
Bash
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
3. SHLVL Nesting
Bash
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
```
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
6. Builtin Commands
Bash
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