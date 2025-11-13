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
true
echo $?
false
echo $?
ls /this/path/does/not/exist
echo $?


Expected:
Correct exit statuses (0 for success, non-zero for errors).

# 2. History
# 2.1 Arrow key navigation
echo one
echo two
echo three
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