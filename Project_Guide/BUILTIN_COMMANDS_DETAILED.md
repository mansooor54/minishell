# Built-in Commands - Comprehensive Documentation

This document provides detailed explanations for all built-in commands in the minishell project.

---

## Table of Contents

1. [Overview](#overview)
2. [builtins.c](#builtinsc)
   - [is_builtin](#is_builtin)
   - [execute_builtin](#execute_builtin)
3. [builtin_echo.c](#builtin_echoc)
   - [builtin_echo](#builtin_echo)
4. [builtin_cd.c](#builtin_cdc)
   - [builtin_cd](#builtin_cd)
5. [builtin_pwd.c](#builtin_pwdc)
   - [builtin_pwd](#builtin_pwd)
6. [builtin_export.c](#builtin_exportc)
   - [parse_export_arg](#parse_export_arg)
   - [update_or_add_env](#update_or_add_env)
   - [builtin_export](#builtin_export)
7. [builtin_unset.c](#builtin_unsetc)
   - [builtin_unset](#builtin_unset)
8. [builtin_env.c](#builtin_envc)
   - [builtin_env](#builtin_env)
9. [builtin_exit.c](#builtin_exitc)
   - [is_numeric](#is_numeric)
   - [builtin_exit](#builtin_exit)

---

## Overview

Built-in commands are commands that are implemented directly in the shell rather than as external programs. They are necessary for commands that need to modify the shell's internal state (like `cd`, `export`, `exit`) or for performance reasons.

### Why Built-ins Are Necessary

**1. State Modification:**
- `cd`: Must change the parent shell's working directory
- `export`: Must modify the parent shell's environment
- `unset`: Must remove variables from the parent shell
- `exit`: Must terminate the parent shell

If these were external programs, they would run in a child process and couldn't affect the parent shell.

**2. Performance:**
- `echo`, `pwd`, `env`: Could be external, but built-ins are faster (no fork/exec overhead)

**3. 42 Subject Requirement:**
All seven built-ins are mandatory:
- `echo` with `-n` option
- `cd` with relative or absolute path
- `pwd` with no options
- `export` with no options
- `unset` with no options
- `env` with no options or arguments
- `exit` with no options

---

## builtins.c

### is_builtin

**Function Signature:**
```c
int is_builtin(char *cmd)
```

**Purpose:**  
Check if a command name corresponds to a built-in command.

**Parameters:**
- `cmd`: Command name to check (e.g., "echo", "cd", "pwd")

**Return Value:**
- 1 if the command is a built-in
- 0 if the command is not a built-in (external command)

**Detailed Description:**

This function performs a simple string comparison to determine if a command is one of the seven built-in commands. It's used by the executor to decide whether to call `execute_builtin()` or `execute_external()`.

**Implementation:**

The function uses a series of `if` statements to check the command name:

```c
if (ft_strcmp(cmd, "echo") == 0)    return (1);
if (ft_strcmp(cmd, "cd") == 0)      return (1);
if (ft_strcmp(cmd, "pwd") == 0)     return (1);
if (ft_strcmp(cmd, "export") == 0)  return (1);
if (ft_strcmp(cmd, "unset") == 0)   return (1);
if (ft_strcmp(cmd, "env") == 0)     return (1);
if (ft_strcmp(cmd, "exit") == 0)    return (1);
return (0);  // Not a builtin
```

**Design Considerations:**

**Why not use a lookup table?**
- Simple implementation for only 7 commands
- Easy to read and maintain
- Performance difference is negligible

**Why not use `||` operators?**
```c
// Alternative (but less clear):
return (strcmp(cmd, "echo") == 0 || strcmp(cmd, "cd") == 0 || ...);
```
- Current implementation is more readable
- Easier to add/remove builtins
- Follows 42 Norminette style

**Usage in Executor:**

```c
// In execute_command():
if (is_builtin(cmd->args[0])) {
    // Execute as builtin
    if (cmd->redirs)
        execute_builtin_with_redir(cmd, shell);
    else
        shell->exit_status = execute_builtin(cmd, shell);
} else {
    // Execute as external command
    execute_external(cmd, shell);
}
```

**Examples:**

```c
is_builtin("echo");     // Returns: 1
is_builtin("cd");       // Returns: 1
is_builtin("ls");       // Returns: 0 (external command)
is_builtin("grep");     // Returns: 0 (external command)
is_builtin("ECHO");     // Returns: 0 (case-sensitive)
is_builtin("Echo");     // Returns: 0 (case-sensitive)
```

**Case Sensitivity:**

Built-in names are case-sensitive (bash behavior):
```bash
echo "hello"   # Works (builtin)
ECHO "hello"   # Error: command not found
Echo "hello"   # Error: command not found
```

**Performance:**

For 7 commands, worst case is 7 comparisons. This is O(n) where n=7, which is acceptable. For shells with many builtins, a hash table would be more efficient.

---

### execute_builtin

**Function Signature:**
```c
int execute_builtin(t_cmd *cmd, t_shell *shell)
```

**Purpose:**  
Dispatch a built-in command to its specific implementation function.

**Parameters:**
- `cmd`: Command structure containing:
  - `args`: NULL-terminated array of arguments (args[0] is command name)
  - `redirs`: Redirections (not used in this function)
- `shell`: Shell state structure containing environment and exit status

**Return Value:**  
Exit status of the executed built-in command (0 for success, non-zero for error)

**Detailed Description:**

This function is the dispatcher for built-in commands. It determines which built-in was requested and calls the appropriate implementation function with the correct parameters.

**Implementation:**

```c
if (strcmp(cmd->args[0], "echo") == 0)
    return (builtin_echo(cmd->args));
if (strcmp(cmd->args[0], "cd") == 0)
    return (builtin_cd(cmd->args, shell->env));
if (strcmp(cmd->args[0], "pwd") == 0)
    return (builtin_pwd());
if (strcmp(cmd->args[0], "export") == 0)
    return (builtin_export(cmd->args, &shell->env));
if (strcmp(cmd->args[0], "unset") == 0)
    return (builtin_unset(cmd->args, &shell->env));
if (strcmp(cmd->args[0], "env") == 0)
    return (builtin_env(shell->env));
if (strcmp(cmd->args[0], "exit") == 0)
    return (builtin_exit(cmd->args, shell));
return (0);  // Should never reach here
```

**Parameter Patterns:**

Different builtins require different parameters:

| Builtin | Parameters | Why |
|---------|-----------|-----|
| `echo` | `args` only | Doesn't need environment or shell state |
| `cd` | `args`, `env` | Needs HOME from environment |
| `pwd` | None | Only needs to call getcwd() |
| `export` | `args`, `&env` | Modifies environment (needs pointer to pointer) |
| `unset` | `args`, `&env` | Modifies environment (needs pointer to pointer) |
| `env` | `env` | Reads environment (doesn't modify) |
| `exit` | `args`, `shell` | Needs to set should_exit flag |

**Why Pass `&env` vs `env`?**

```c
// Read-only access:
builtin_env(shell->env);        // Pass pointer to list
builtin_cd(cmd->args, shell->env);  // Pass pointer to list

// Modify list:
builtin_export(cmd->args, &shell->env);  // Pass pointer to pointer
builtin_unset(cmd->args, &shell->env);   // Pass pointer to pointer
```

- `env`: Can traverse and read, but can't change the head pointer
- `&env`: Can modify the list structure (add/remove nodes, change head)

**Execution Context:**

This function is called in two contexts:

**1. Direct execution (no redirections):**
```c
// In execute_command():
shell->exit_status = execute_builtin(cmd, shell);
```
- Runs in parent process
- Can modify shell state (cd, export, exit)
- Exit status directly updates shell

**2. Forked execution (with redirections):**
```c
// In execute_builtin_with_redir():
pid = fork();
if (pid == 0) {
    setup_redirections(cmd->redirs);
    exit(execute_builtin(cmd, shell));
}
```
- Runs in child process
- Redirections don't affect parent
- Exit status passed via exit()

**Examples:**

```c
// Example 1: echo hello
cmd->args = {"echo", "hello", NULL};
execute_builtin(cmd, shell);
// Calls: builtin_echo({"echo", "hello", NULL})
// Output: "hello\n"
// Returns: 0

// Example 2: cd /tmp
cmd->args = {"cd", "/tmp", NULL};
execute_builtin(cmd, shell);
// Calls: builtin_cd({"cd", "/tmp", NULL}, shell->env)
// Effect: Changes directory to /tmp
// Returns: 0 (success) or 1 (failure)

// Example 3: export VAR=value
cmd->args = {"export", "VAR=value", NULL};
execute_builtin(cmd, shell);
// Calls: builtin_export({"export", "VAR=value", NULL}, &shell->env)
// Effect: Adds VAR=value to environment
// Returns: 0

// Example 4: exit 42
cmd->args = {"exit", "42", NULL};
execute_builtin(cmd, shell);
// Calls: builtin_exit({"exit", "42", NULL}, shell)
// Effect: Sets shell->should_exit = 1
// Returns: 42
```

**Error Handling:**

Each builtin handles its own errors and returns appropriate exit codes:
- 0: Success
- 1: General error
- 2: Misuse of shell builtin
- 255: Exit with invalid argument

**Why Not Use Function Pointers?**

Alternative implementation with function pointer array:
```c
typedef int (*builtin_func)(char **, t_shell *);

struct builtin_entry {
    char *name;
    builtin_func func;
};

struct builtin_entry builtins[] = {
    {"echo", builtin_echo_wrapper},
    {"cd", builtin_cd_wrapper},
    // ...
};
```

Current implementation is simpler and more straightforward for only 7 builtins.

---

## builtin_echo.c

### builtin_echo

**Function Signature:**
```c
int builtin_echo(char **args)
```

**Purpose:**  
Print arguments to standard output, with optional `-n` flag to suppress the trailing newline.

**Parameters:**
- `args`: NULL-terminated array of arguments
  - `args[0]`: "echo" (command name)
  - `args[1]`: First argument (possibly "-n" flag)
  - `args[2...]`: Remaining arguments to print

**Return Value:**  
Always returns 0 (success)

**Detailed Description:**

The `echo` builtin prints its arguments separated by spaces. It supports the `-n` option which suppresses the trailing newline.

**Implementation Algorithm:**

```
1. Initialize:
   - i = 1 (start at first argument after "echo")
   - newline = 1 (default: print newline at end)

2. Check for -n flag:
   - If args[1] == "-n":
     - Set newline = 0
     - Set i = 2 (skip the -n flag)

3. Print arguments:
   - While args[i] exists:
     - Print args[i]
     - If args[i+1] exists:
       - Print a space
     - Increment i

4. Print newline:
   - If newline == 1:
     - Print "\n"

5. Return 0
```

**Examples:**

**Example 1: Basic echo**
```bash
echo hello world
```
Execution:
```
args = {"echo", "hello", "world", NULL}
i = 1, newline = 1
Print "hello"
Print " "
Print "world"
Print "\n"
Output: "hello world\n"
```

**Example 2: Echo with -n flag**
```bash
echo -n hello world
```
Execution:
```
args = {"echo", "-n", "hello", "world", NULL}
Check args[1] == "-n": YES
newline = 0, i = 2
Print "hello"
Print " "
Print "world"
No newline printed
Output: "hello world" (no newline)
```

**Example 3: Echo with no arguments**
```bash
echo
```
Execution:
```
args = {"echo", NULL}
i = 1, newline = 1
Loop doesn't execute (args[1] is NULL)
Print "\n"
Output: "\n"
```

**Example 4: Echo with only -n**
```bash
echo -n
```
Execution:
```
args = {"echo", "-n", NULL}
Check args[1] == "-n": YES
newline = 0, i = 2
Loop doesn't execute (args[2] is NULL)
No newline printed
Output: "" (empty, no newline)
```

**Example 5: Echo with -n as argument (not flag)**
```bash
echo hello -n world
```
Execution:
```
args = {"echo", "hello", "-n", "world", NULL}
Check args[1] == "-n": NO (args[1] is "hello")
i = 1, newline = 1
Print "hello"
Print " "
Print "-n"
Print " "
Print "world"
Print "\n"
Output: "hello -n world\n"
```

**Comparison with Bash:**

**Similarities:**
- Prints arguments separated by spaces
- Supports `-n` to suppress newline
- `-n` must be the first argument to be recognized as a flag

**Differences:**
- Bash `echo` supports multiple flags: `-n`, `-e`, `-E`
- Bash `echo` with `-e` interprets escape sequences (\n, \t, etc.)
- This implementation only supports `-n`

**Bash behavior:**
```bash
echo -n -n hello    # Bash: prints "-n hello" (only first -n is flag)
echo -n hello -n    # Bash: prints "hello -n" (-n in middle is literal)
```

**This implementation:**
```bash
echo -n -n hello    # Prints "-n hello" (only first -n is flag)
echo -n hello -n    # Prints "hello -n" (same as bash)
```

**Edge Cases:**

**Case 1: Empty string argument**
```bash
echo "" hello ""
```
Output: ` hello ` (spaces preserved)

**Case 2: Special characters**
```bash
echo "hello\nworld"
```
Output: `hello\nworld\n` (no escape interpretation)

**Case 3: Multiple spaces**
```bash
echo hello    world
```
After parsing, args = {"echo", "hello", "world", NULL}
Output: `hello world\n` (multiple spaces collapsed by parser)

**Case 4: Quotes (handled by expander/parser)**
```bash
echo "hello world"
```
After parsing, args = {"echo", "hello world", NULL}
Output: `hello world\n`

**Why Always Return 0?**

The `echo` builtin cannot fail in normal circumstances:
- No file I/O (writes to stdout)
- No system calls that can fail
- Even if stdout is closed, write() fails silently

Bash `echo` also always returns 0.

**Implementation Notes:**

**Space Handling:**
```c
if (args[i + 1])
    ft_putstr_fd(" ", 1);
```
- Only print space if there's another argument
- Prevents trailing space before newline

**Newline Handling:**
```c
if (newline)
    ft_putstr_fd("\n", 1);
```
- Newline is printed after all arguments
- Controlled by the `-n` flag

---

## builtin_cd.c

### builtin_cd

**Function Signature:**
```c
int builtin_cd(char **args, t_env *env)
```

**Purpose:**  
Change the current working directory of the shell.

**Parameters:**
- `args`: NULL-terminated array of arguments
  - `args[0]`: "cd" (command name)
  - `args[1]`: Target directory path (optional)
- `env`: Environment variable list (to get HOME)

**Return Value:**
- 0 on success (directory changed)
- 1 on failure (directory doesn't exist or HOME not set)

**Detailed Description:**

The `cd` (change directory) builtin changes the shell's current working directory. It supports:
1. Absolute paths: `/usr/local/bin`
2. Relative paths: `../parent`, `subdir`
3. Home directory: `cd` with no arguments goes to HOME

**Implementation Algorithm:**

```
1. Check if args[1] exists:
   
   a. If NO (no argument):
      - Get HOME from environment
      - If HOME not set:
        - Print error: "HOME not set"
        - Return 1
      - Set path = HOME
   
   b. If YES (argument provided):
      - Set path = args[1]

2. Call chdir(path):
   - If chdir returns -1 (failure):
     - Print error: "No such file or directory"
     - Return 1
   
3. Return 0 (success)
```

**Examples:**

**Example 1: cd to home directory**
```bash
cd
```
Execution:
```
args = {"cd", NULL}
args[1] is NULL
Get HOME from env → "/home/user"
chdir("/home/user")
Success → return 0
```

**Example 2: cd to absolute path**
```bash
cd /tmp
```
Execution:
```
args = {"cd", "/tmp", NULL}
path = "/tmp"
chdir("/tmp")
Success → return 0
```

**Example 3: cd to relative path**
```bash
cd ../parent
```
Execution:
```
args = {"cd", "../parent", NULL}
path = "../parent"
chdir("../parent")
Success → return 0
```

**Example 4: cd to non-existent directory**
```bash
cd /nonexistent
```
Execution:
```
args = {"cd", "/nonexistent", NULL}
path = "/nonexistent"
chdir("/nonexistent") → returns -1
Print error: "minishell: cd: /nonexistent: No such file or directory"
Return 1
```

**Example 5: cd with HOME not set**
```bash
unset HOME
cd
```
Execution:
```
args = {"cd", NULL}
get_env_value(env, "HOME") → NULL
Print error: "minishell: cd: HOME not set"
Return 1
```

**Why cd Must Be a Builtin:**

If `cd` were an external command:

```c
// Hypothetical external cd:
pid = fork();
if (pid == 0) {
    chdir("/tmp");  // Changes child's directory
    exit(0);
}
wait(&status);
// Parent's directory unchanged!
```

The child process would change its own directory, but the parent shell's directory would remain unchanged. This is why `cd` must be a builtin that runs in the parent process.

**Special Paths:**

**1. Home directory (`~`)**
```bash
cd ~
```
Note: The `~` expansion is handled by the expander, not by `cd`. By the time `cd` receives the argument, `~` has already been expanded to HOME.

**2. Previous directory (`-`)**
```bash
cd -
```
Note: This implementation doesn't support `cd -`. To implement it:
- Store previous directory in environment variable `OLDPWD`
- Before changing directory, save current directory to `OLDPWD`
- If args[1] is "-", use `OLDPWD` as path

**3. Current directory (`.`)**
```bash
cd .
```
Valid but pointless (stays in current directory).

**4. Parent directory (`..`)**
```bash
cd ..
```
Goes to parent directory.

**Error Cases:**

**Case 1: Directory doesn't exist**
```bash
cd /nonexistent
```
Error: `minishell: cd: /nonexistent: No such file or directory`
Exit status: 1

**Case 2: Not a directory**
```bash
cd /etc/passwd
```
Error: `minishell: cd: /etc/passwd: Not a directory`
Note: chdir() returns -1, but error message says "No such file or directory"
This is a simplification; bash distinguishes between these errors.

**Case 3: Permission denied**
```bash
cd /root
```
Error: `minishell: cd: /root: Permission denied`
Note: chdir() returns -1, but error message says "No such file or directory"
This is a simplification.

**Case 4: Too many arguments**
```bash
cd /tmp /usr
```
This implementation doesn't check for multiple arguments. It would use `/tmp` and ignore `/usr`. Bash would print: `bash: cd: too many arguments`

**Comparison with Bash:**

| Feature | This Implementation | Bash |
|---------|-------------------|------|
| `cd` (no args) | Goes to HOME | Goes to HOME |
| `cd ~` | Depends on expander | Goes to HOME |
| `cd -` | Not supported | Goes to OLDPWD |
| `cd ..` | Supported | Supported |
| Multiple args | Uses first, ignores rest | Error: too many arguments |
| Error messages | Generic | Specific (permission, not a dir, etc.) |
| OLDPWD | Not updated | Updated automatically |

**Environment Variables:**

**PWD (Present Working Directory):**
This implementation doesn't update PWD after cd. To be more bash-like:
```c
int builtin_cd(char **args, t_env *env) {
    char *path;
    char cwd[4096];
    
    // ... existing code to determine path ...
    
    if (chdir(path) == -1) {
        // ... error handling ...
        return (1);
    }
    
    // Update PWD
    if (getcwd(cwd, sizeof(cwd))) {
        update_or_add_env(&env, "PWD", cwd);
    }
    
    return (0);
}
```

**OLDPWD (Previous Working Directory):**
Not implemented. To add:
```c
// Before chdir:
if (getcwd(cwd, sizeof(cwd))) {
    update_or_add_env(&env, "OLDPWD", cwd);
}
```

**Why Pass `env` Instead of `&env`?**

```c
int builtin_cd(char **args, t_env *env)
```

`cd` only **reads** from the environment (to get HOME). It doesn't modify the environment list structure, so it doesn't need `&env`.

If we were updating PWD/OLDPWD, we would need `&env`:
```c
int builtin_cd(char **args, t_env **env)
```

---

## builtin_pwd.c

### builtin_pwd

**Function Signature:**
```c
int builtin_pwd(void)
```

**Purpose:**  
Print the current working directory to standard output.

**Parameters:**  
None

**Return Value:**
- 0 on success
- 1 if getcwd() fails

**Detailed Description:**

The `pwd` (print working directory) builtin displays the absolute path of the current working directory. It's one of the simplest builtins.

**Implementation:**

```c
char cwd[4096];

if (getcwd(cwd, sizeof(cwd)) == NULL) {
    ft_putendl_fd("minishell: pwd: error getting current directory", 2);
    return (1);
}
ft_putendl_fd(cwd, 1);
return (0);
```

**Algorithm:**

```
1. Declare buffer: char cwd[4096]
2. Call getcwd(cwd, 4096)
3. If getcwd returns NULL:
   - Print error to stderr
   - Return 1
4. Print cwd to stdout
5. Return 0
```

**getcwd() System Call:**

```c
char *getcwd(char *buf, size_t size);
```

- Gets the absolute pathname of the current working directory
- Stores it in `buf` (up to `size` bytes)
- Returns `buf` on success, NULL on failure

**Why 4096 bytes?**

- PATH_MAX on most systems is 4096
- Sufficient for most directory paths
- If path exceeds 4096, getcwd() fails

**Examples:**

**Example 1: Normal usage**
```bash
pwd
```
Execution:
```
getcwd(cwd, 4096) → "/home/user/projects"
Print "/home/user/projects\n"
Return 0
```

**Example 2: After cd**
```bash
cd /tmp
pwd
```
Execution:
```
getcwd(cwd, 4096) → "/tmp"
Print "/tmp\n"
Return 0
```

**Example 3: In home directory**
```bash
cd
pwd
```
Execution:
```
getcwd(cwd, 4096) → "/home/user"
Print "/home/user\n"
Return 0
```

**Error Cases:**

**Case 1: Directory deleted**
```bash
mkdir /tmp/testdir
cd /tmp/testdir
rm -rf /tmp/testdir  # Delete current directory
pwd
```
Execution:
```
getcwd(cwd, 4096) → NULL (directory no longer exists)
Print "minishell: pwd: error getting current directory\n"
Return 1
```

**Case 2: No read permission on parent**
```bash
mkdir /tmp/parent
mkdir /tmp/parent/child
chmod 000 /tmp/parent
cd /tmp/parent/child
pwd
```
Execution:
```
getcwd(cwd, 4096) → NULL (can't read parent directory)
Print "minishell: pwd: error getting current directory\n"
Return 1
```

**Case 3: Path too long**
Extremely rare, but if the path exceeds 4096 characters:
```
getcwd(cwd, 4096) → NULL (buffer too small)
Print error
Return 1
```

**Why pwd Should Be a Builtin:**

Technically, `pwd` could be an external command (and it exists as `/bin/pwd`). However:

1. **Performance**: Builtin is faster (no fork/exec)
2. **Reliability**: Always available even if `/bin/pwd` is missing
3. **Consistency**: Matches bash behavior

**Comparison with Bash:**

| Feature | This Implementation | Bash |
|---------|-------------------|------|
| Basic pwd | Supported | Supported |
| pwd -L | Not supported | Logical pwd (follows symlinks) |
| pwd -P | Not supported | Physical pwd (resolves symlinks) |
| Error handling | Generic message | Specific error codes |

**Bash pwd options:**

```bash
pwd -L  # Logical: use PWD from environment (follows symlinks)
pwd -P  # Physical: resolve all symlinks
```

This implementation doesn't support options (as per 42 subject: "pwd with no options").

**Symlink Behavior:**

```bash
mkdir /tmp/real
ln -s /tmp/real /tmp/link
cd /tmp/link
pwd
```

**This implementation:**
```
Output: /tmp/link (logical path)
```

**Bash `pwd` (default):**
```
Output: /tmp/link (logical, uses PWD environment variable)
```

**Bash `pwd -P`:**
```
Output: /tmp/real (physical, resolves symlinks)
```

**Why No Parameters?**

```c
int builtin_pwd(void)
```

`pwd` doesn't need:
- Arguments (no options supported)
- Environment (doesn't read env variables)
- Shell state (doesn't modify anything)

It's completely self-contained.

**Alternative Implementation:**

Could use PWD environment variable:
```c
int builtin_pwd(t_env *env) {
    char *pwd;
    
    pwd = get_env_value(env, "PWD");
    if (pwd) {
        ft_putendl_fd(pwd, 1);
        return (0);
    }
    
    // Fallback to getcwd if PWD not set
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        ft_putendl_fd("minishell: pwd: error getting current directory", 2);
        return (1);
    }
    ft_putendl_fd(cwd, 1);
    return (0);
}
```

This would match bash behavior more closely, but the current implementation is simpler and sufficient.

---

## builtin_export.c

### parse_export_arg

**Function Signature:**
```c
static void parse_export_arg(char *arg, char **key, char **value)
```

**Purpose:**  
Parse a KEY=VALUE string into separate key and value components.

**Parameters:**
- `arg`: Input string in format "KEY=VALUE" or "KEY"
- `key`: Output pointer to store the key (allocated)
- `value`: Output pointer to store the value (allocated, or NULL if no '=')

**Return Value:**  
void (results returned via `key` and `value` pointers)

**Detailed Description:**

This helper function splits an export argument into key and value parts. It handles two formats:
1. `KEY=VALUE`: Sets variable with value
2. `KEY`: Declares variable without value (value is NULL)

**Implementation:**

```c
char *eq;

eq = ft_strchr(arg, '=');  // Find '=' character
if (eq) {
    // Format: KEY=VALUE
    *key = ft_substr(arg, 0, eq - arg);  // Extract key
    *value = ft_strdup(eq + 1);           // Extract value
} else {
    // Format: KEY (no value)
    *key = ft_strdup(arg);
    *value = NULL;
}
```

**Algorithm:**

```
1. Find '=' in arg using ft_strchr()
2. If '=' found:
   - key = substring from start to '='
   - value = substring from after '=' to end
3. If '=' not found:
   - key = duplicate of entire arg
   - value = NULL
```

**Examples:**

**Example 1: KEY=VALUE**
```c
char *key, *value;
parse_export_arg("PATH=/usr/bin", &key, &value);
// key = "PATH"
// value = "/usr/bin"
```

**Example 2: KEY (no value)**
```c
char *key, *value;
parse_export_arg("MY_VAR", &key, &value);
// key = "MY_VAR"
// value = NULL
```

**Example 3: KEY=**
```c
char *key, *value;
parse_export_arg("EMPTY=", &key, &value);
// key = "EMPTY"
// value = "" (empty string, not NULL)
```

**Example 4: KEY=VALUE=MORE**
```c
char *key, *value;
parse_export_arg("VAR=value=more", &key, &value);
// key = "VAR"
// value = "value=more" (everything after first '=')
```

**Example 5: =VALUE (invalid)**
```c
char *key, *value;
parse_export_arg("=value", &key, &value);
// key = "" (empty string)
// value = "value"
// Note: This is invalid but not checked here
```

**Memory Management:**

Both `key` and `value` are allocated and must be freed by the caller:

```c
char *key, *value;
parse_export_arg("VAR=value", &key, &value);
// Use key and value...
free(key);
free(value);
```

**Edge Cases:**

**Case 1: Empty string**
```c
parse_export_arg("", &key, &value);
// key = ""
// value = NULL
```

**Case 2: Only '='**
```c
parse_export_arg("=", &key, &value);
// key = ""
// value = ""
```

**Case 3: Multiple '=' signs**
```c
parse_export_arg("A=B=C=D", &key, &value);
// key = "A"
// value = "B=C=D"
// Only first '=' is used as delimiter
```

**Case 4: Spaces (handled by parser)**
```c
// After shell parsing:
parse_export_arg("VAR = value", &key, &value);
// key = "VAR "
// value = " value"
// Note: Spaces are preserved
```

**Why Static?**

```c
static void parse_export_arg(...)
```

- Only used within builtin_export.c
- Not needed by other files
- Reduces namespace pollution
- Follows encapsulation principles

---

### update_or_add_env

**Function Signature:**
```c
static void update_or_add_env(t_env **env, char *key, char *value)
```

**Purpose:**  
Update an existing environment variable or add a new one.

**Parameters:**
- `env`: Pointer to pointer to environment list head
- `key`: Variable name to update/add
- `value`: New value (can be NULL to declare without value)

**Return Value:**  
void (modifies environment list)

**Detailed Description:**

This function implements the core logic of `export`: it searches for an existing variable and updates it, or creates a new variable if it doesn't exist.

**Implementation:**

```c
t_env *current;
t_env *new_node;

current = *env;
while (current) {
    if (ft_strcmp(current->key, key) == 0) {
        // Found existing variable
        if (value) {
            free(current->value);
            current->value = ft_strdup(value);
        }
        return;
    }
    current = current->next;
}
// Not found, create new
new_node = create_env_node(key, value);
add_env_node(env, new_node);
```

**Algorithm:**

```
1. Search for existing variable:
   - Traverse environment list
   - Compare each key with target key
   - If match found:
     a. If value is not NULL:
        - Free old value
        - Duplicate and store new value
     b. Return (done)

2. If not found:
   - Create new environment node
   - Add node to environment list
```

**Examples:**

**Example 1: Update existing variable**
```c
// Environment: PATH=/usr/bin
update_or_add_env(&env, "PATH", "/usr/local/bin");
// Environment: PATH=/usr/local/bin (updated)
```

**Example 2: Add new variable**
```c
// Environment: PATH=/usr/bin
update_or_add_env(&env, "MY_VAR", "value");
// Environment: PATH=/usr/bin, MY_VAR=value (added)
```

**Example 3: Update with NULL value**
```c
// Environment: VAR=oldvalue
update_or_add_env(&env, "VAR", NULL);
// Environment: VAR=oldvalue (unchanged)
// Note: NULL value doesn't update existing variable
```

**Example 4: Add with NULL value**
```c
// Environment: PATH=/usr/bin
update_or_add_env(&env, "NEW_VAR", NULL);
// Environment: PATH=/usr/bin, NEW_VAR (no value)
```

**Behavior with NULL Value:**

When `value` is NULL:

**For existing variable:**
```c
if (value) {
    free(current->value);
    current->value = ft_strdup(value);
}
// If value is NULL, do nothing (keep old value)
```

**For new variable:**
```c
new_node = create_env_node(key, value);
// Creates node with value = NULL
```

This matches bash behavior:
```bash
export VAR=value  # Sets VAR to "value"
export VAR        # Declares VAR but doesn't change its value
```

**Why Pass `**env`?**

```c
void update_or_add_env(t_env **env, ...)
```

We need `**env` (pointer to pointer) because:
1. We might modify the head of the list (when adding first node)
2. `add_env_node()` might change the head pointer

Example:
```c
// Empty environment
t_env *env = NULL;

// Add first variable
update_or_add_env(&env, "VAR", "value");
// env now points to new node
// If we only had *env, we couldn't update the pointer
```

**Memory Management:**

**Updating existing variable:**
```c
free(current->value);           // Free old value
current->value = ft_strdup(value);  // Allocate new value
```

**Adding new variable:**
```c
new_node = create_env_node(key, value);
// create_env_node() allocates:
// - The node itself
// - Duplicate of key
// - Duplicate of value (if not NULL)
```

**Case Sensitivity:**

Variable names are case-sensitive:
```c
update_or_add_env(&env, "VAR", "value1");
update_or_add_env(&env, "var", "value2");
// Creates two separate variables: VAR and var
```

This matches bash behavior.

**Order of Variables:**

New variables are added to the end of the list:
```c
// Environment: A=1, B=2
update_or_add_env(&env, "C", "3");
// Environment: A=1, B=2, C=3 (C added at end)
```

---

### builtin_export

**Function Signature:**
```c
int builtin_export(char **args, t_env **env)
```

**Purpose:**  
Set or display environment variables.

**Parameters:**
- `args`: NULL-terminated array of arguments
  - `args[0]`: "export" (command name)
  - `args[1...]`: Variables to export in KEY=VALUE format
- `env`: Pointer to pointer to environment list

**Return Value:**  
Always returns 0 (success)

**Detailed Description:**

The `export` builtin sets environment variables. It supports two modes:
1. **With arguments**: Set variables
2. **Without arguments**: Display all variables (calls `builtin_env`)

**Implementation:**

```c
int i;
char *key;
char *value;

if (!args[1])
    return (builtin_env(*env));  // No args: display all

i = 1;
while (args[i]) {
    parse_export_arg(args[i], &key, &value);
    update_or_add_env(env, key, value);
    free(key);
    free(value);
    i++;
}
return (0);
```

**Algorithm:**

```
1. Check if args[1] exists:
   - If NO: Call builtin_env() and return
   
2. For each argument (starting at args[1]):
   a. Parse argument into key and value
   b. Update or add environment variable
   c. Free key and value
   d. Move to next argument

3. Return 0
```

**Examples:**

**Example 1: Export single variable**
```bash
export VAR=value
```
Execution:
```
args = {"export", "VAR=value", NULL}
parse_export_arg("VAR=value") → key="VAR", value="value"
update_or_add_env(env, "VAR", "value")
Return 0
```

**Example 2: Export multiple variables**
```bash
export VAR1=value1 VAR2=value2 VAR3=value3
```
Execution:
```
args = {"export", "VAR1=value1", "VAR2=value2", "VAR3=value3", NULL}
Loop:
  i=1: VAR1=value1
  i=2: VAR2=value2
  i=3: VAR3=value3
Return 0
```

**Example 3: Export without value**
```bash
export MY_VAR
```
Execution:
```
args = {"export", "MY_VAR", NULL}
parse_export_arg("MY_VAR") → key="MY_VAR", value=NULL
update_or_add_env(env, "MY_VAR", NULL)
Return 0
```

**Example 4: Export with empty value**
```bash
export EMPTY=
```
Execution:
```
args = {"export", "EMPTY=", NULL}
parse_export_arg("EMPTY=") → key="EMPTY", value=""
update_or_add_env(env, "EMPTY", "")
Return 0
```

**Example 5: Export without arguments (display)**
```bash
export
```
Execution:
```
args = {"export", NULL}
args[1] is NULL
Call builtin_env(*env)
Return 0
```

**Bash Comparison:**

| Feature | This Implementation | Bash |
|---------|-------------------|------|
| `export VAR=value` | Supported | Supported |
| `export VAR` | Supported | Supported |
| `export` (no args) | Shows all vars | Shows all with "declare -x" |
| `export -p` | Not supported | Print in reusable format |
| `export -n` | Not supported | Remove export attribute |
| Invalid names | Not checked | Error: not a valid identifier |
| `export VAR+=value` | Not supported | Append to variable |

**Bash export without arguments:**
```bash
$ export
declare -x HOME="/home/user"
declare -x PATH="/usr/bin:/bin"
declare -x USER="user"
```

**This implementation:**
```bash
$ export
HOME=/home/user
PATH=/usr/bin:/bin
USER=user
```

**Variable Name Validation:**

This implementation doesn't validate variable names. Bash requires:
- Must start with letter or underscore
- Can contain letters, digits, underscores
- Cannot contain spaces, special characters

Examples of invalid names (accepted by this implementation, rejected by bash):
```bash
export 123=value      # Bash: not a valid identifier
export VAR-NAME=value # Bash: not a valid identifier
export VAR NAME=value # Bash: not a valid identifier
```

To add validation:
```c
static int is_valid_identifier(char *str) {
    int i;
    
    if (!str || !*str)
        return (0);
    if (!ft_isalpha(str[0]) && str[0] != '_')
        return (0);
    i = 1;
    while (str[i] && str[i] != '=') {
        if (!ft_isalnum(str[i]) && str[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

int builtin_export(char **args, t_env **env) {
    // ... existing code ...
    
    if (!is_valid_identifier(args[i])) {
        ft_putstr_fd("minishell: export: `", 2);
        ft_putstr_fd(args[i], 2);
        ft_putendl_fd("': not a valid identifier", 2);
        continue;  // Skip this argument
    }
    
    // ... rest of code ...
}
```

**Why Export Must Be a Builtin:**

If `export` were an external command:

```c
pid = fork();
if (pid == 0) {
    // Child process
    setenv("VAR", "value", 1);  // Sets in child's environment
    exit(0);
}
wait(&status);
// Parent's environment unchanged!
```

The child would modify its own environment, but the parent shell's environment would remain unchanged. This is why `export` must be a builtin.

**Memory Management:**

```c
parse_export_arg(args[i], &key, &value);
update_or_add_env(env, key, value);
free(key);    // Free temporary key
free(value);  // Free temporary value
```

- `parse_export_arg()` allocates key and value
- `update_or_add_env()` duplicates them internally
- We must free the temporary allocations

**Return Value:**

Always returns 0, even for invalid variable names. Bash would return non-zero for errors.

---

## builtin_unset.c

### builtin_unset

**Function Signature:**
```c
int builtin_unset(char **args, t_env **env)
```

**Purpose:**  
Remove environment variables by name.

**Parameters:**
- `args`: NULL-terminated array of arguments
  - `args[0]`: "unset" (command name)
  - `args[1...]`: Variable names to remove
- `env`: Pointer to pointer to environment list

**Return Value:**  
Always returns 0 (success)

**Detailed Description:**

The `unset` builtin removes environment variables from the shell's environment. It can remove multiple variables in a single command.

**Implementation:**

```c
int i;

if (!args[1])
    return (0);  // No arguments: do nothing

i = 1;
while (args[i]) {
    remove_env_node(env, args[i]);
    i++;
}
return (0);
```

**Algorithm:**

```
1. Check if args[1] exists:
   - If NO: Return 0 (nothing to do)

2. For each argument (starting at args[1]):
   a. Call remove_env_node(env, args[i])
   b. Move to next argument

3. Return 0
```

**Examples:**

**Example 1: Unset single variable**
```bash
export VAR=value
unset VAR
```
Execution:
```
args = {"unset", "VAR", NULL}
remove_env_node(env, "VAR")
VAR is removed from environment
Return 0
```

**Example 2: Unset multiple variables**
```bash
export VAR1=value1 VAR2=value2 VAR3=value3
unset VAR1 VAR2 VAR3
```
Execution:
```
args = {"unset", "VAR1", "VAR2", "VAR3", NULL}
Loop:
  i=1: remove_env_node(env, "VAR1")
  i=2: remove_env_node(env, "VAR2")
  i=3: remove_env_node(env, "VAR3")
All three variables removed
Return 0
```

**Example 3: Unset non-existent variable**
```bash
unset NONEXISTENT
```
Execution:
```
args = {"unset", "NONEXISTENT", NULL}
remove_env_node(env, "NONEXISTENT")
Nothing happens (variable doesn't exist)
Return 0
```

**Example 4: Unset without arguments**
```bash
unset
```
Execution:
```
args = {"unset", NULL}
args[1] is NULL
Return 0 immediately
```

**Comparison with Bash:**

| Feature | This Implementation | Bash |
|---------|-------------------|------|
| `unset VAR` | Supported | Supported |
| `unset VAR1 VAR2` | Supported | Supported |
| `unset` (no args) | Does nothing | Does nothing |
| `unset -v VAR` | Not supported | Unset variable |
| `unset -f FUNC` | Not supported | Unset function |
| Invalid names | Silently ignored | Error message |
| Readonly vars | Not checked | Error: readonly variable |

**Bash unset options:**
```bash
unset -v VAR   # Unset variable (default)
unset -f FUNC  # Unset function
```

This implementation doesn't support options (as per 42 subject: "unset with no options").

**Special Variables:**

Some variables should not be unset:
- `PATH`: Without PATH, shell can't find commands
- `HOME`: Without HOME, `cd` without arguments fails
- `USER`: Many programs rely on USER

This implementation allows unsetting any variable. Bash also allows it, but warns for some:
```bash
$ unset PATH
bash: hash: hash table empty
```

**Why Unset Must Be a Builtin:**

If `unset` were an external command:

```c
pid = fork();
if (pid == 0) {
    // Child process
    unsetenv("VAR");  // Removes from child's environment
    exit(0);
}
wait(&status);
// Parent's environment unchanged!
```

The child would modify its own environment, but the parent shell's environment would remain unchanged. This is why `unset` must be a builtin.

**remove_env_node() Behavior:**

```c
void remove_env_node(t_env **env, char *key)
```

This function (defined in environment_utils.c):
1. Searches for node with matching key
2. If found:
   - Removes node from linked list
   - Frees node's key and value
   - Frees node itself
3. If not found:
   - Does nothing (no error)

**Memory Management:**

```c
// Before unset:
env = {
    key: "VAR" (allocated)
    value: "value" (allocated)
    next: ...
}

// After unset VAR:
remove_env_node(env, "VAR");
// Frees: key, value, and node
// env no longer contains VAR
```

**Why Pass `**env`?**

```c
int builtin_unset(char **args, t_env **env)
```

We need `**env` because:
1. Removing a node might change the head pointer
2. `remove_env_node()` needs to modify the list structure

Example:
```c
// Environment: VAR=value (only one variable)
t_env *env = /* points to VAR node */;

unset("VAR");
// env now points to NULL (list is empty)
// If we only had *env, we couldn't update the pointer
```

**Return Value:**

Always returns 0, even if:
- Variable doesn't exist
- No arguments provided
- Invalid variable name

Bash also returns 0 in most cases, but returns 1 for:
- Readonly variables
- Invalid options

**Case Sensitivity:**

Variable names are case-sensitive:
```bash
export VAR=value
export var=value2
unset VAR
# Removes VAR, but var still exists
```

**Order of Removal:**

Variables are removed in the order specified:
```bash
unset VAR1 VAR2 VAR3
# Removes VAR1, then VAR2, then VAR3
```

Order doesn't matter for independent variables, but could matter if there were dependencies (which there aren't in this implementation).

---

## builtin_env.c

### builtin_env

**Function Signature:**
```c
int builtin_env(t_env *env)
```

**Purpose:**  
Print all environment variables to standard output.

**Parameters:**
- `env`: Environment variable list

**Return Value:**  
Always returns 0 (success)

**Detailed Description:**

The `env` builtin displays all environment variables in KEY=VALUE format. It's similar to the external `/usr/bin/env` command but implemented as a builtin for performance and consistency.

**Implementation:**

```c
while (env) {
    if (env->value) {
        ft_putstr_fd(env->key, 1);
        ft_putstr_fd("=", 1);
        ft_putendl_fd(env->value, 1);
    }
    env = env->next;
}
return (0);
```

**Algorithm:**

```
1. Traverse environment list:
   - For each node:
     a. If value is not NULL:
        - Print key
        - Print "="
        - Print value
        - Print newline
     b. Move to next node

2. Return 0
```

**Examples:**

**Example 1: Display environment**
```bash
env
```
Output:
```
HOME=/home/user
PATH=/usr/bin:/bin
USER=user
SHELL=/bin/minishell
...
```

**Example 2: After export**
```bash
export MY_VAR=my_value
env
```
Output includes:
```
...
MY_VAR=my_value
...
```

**Example 3: After unset**
```bash
unset PATH
env
```
Output doesn't include PATH.

**Why Check `if (env->value)`?**

```c
if (env->value) {
    // Print KEY=VALUE
}
```

This handles variables declared without a value:
```bash
export VAR  # Declares VAR but doesn't set a value
```

In the environment list:
```c
{
    key: "VAR"
    value: NULL  // No value
    next: ...
}
```

When printing:
- If `value` is NULL, skip this variable
- Only print variables that have values

This matches bash behavior:
```bash
$ export VAR
$ env | grep VAR
(no output - VAR not shown)
$ export VAR=value
$ env | grep VAR
VAR=value
```

**Comparison with Bash:**

| Feature | This Implementation | Bash |
|---------|-------------------|------|
| `env` (no args) | Shows all vars | Shows all vars |
| `env COMMAND` | Not supported | Run command with env |
| `env VAR=value COMMAND` | Not supported | Set var and run command |
| `env -i COMMAND` | Not supported | Run with empty environment |
| `env -u VAR COMMAND` | Not supported | Unset var and run command |
| Variables without values | Not shown | Not shown |

**Bash env with arguments:**
```bash
env VAR=value command  # Set VAR for this command only
env -i command         # Run command with empty environment
env -u VAR command     # Run command without VAR
```

This implementation doesn't support arguments (as per 42 subject: "env with no options or arguments").

**Output Format:**

```
KEY1=VALUE1
KEY2=VALUE2
KEY3=VALUE3
```

- One variable per line
- Format: KEY=VALUE
- No quotes around values
- No sorting (printed in list order)

**Bash env output:**
```bash
$ env
SHELL=/bin/bash
HOME=/home/user
USER=user
PATH=/usr/bin:/bin
...
```

**This implementation output:**
```bash
$ env
HOME=/home/user
PATH=/usr/bin:/bin
USER=user
SHELL=/bin/minishell
...
```

Order may differ (depends on how variables were added).

**Why env Could Be External:**

Unlike `cd`, `export`, `unset`, and `exit`, the `env` builtin doesn't need to modify the shell's state. It could be an external command.

However, it's a builtin because:
1. **Performance**: No fork/exec overhead
2. **Consistency**: Always available
3. **Tradition**: Most shells implement it as a builtin

**Empty Environment:**

```bash
# After unsetting all variables:
env
```
Output: (nothing)

**Special Characters in Values:**

```bash
export VAR="value with spaces"
env
```
Output:
```
VAR=value with spaces
```

No quotes in output. If value contains newlines:
```bash
export VAR=$'line1\nline2'
env
```
Output:
```
VAR=line1
line2
```

This could break parsing. Bash handles this correctly, but this implementation doesn't check for special characters.

**Why Pass `env` Instead of `&env`?**

```c
int builtin_env(t_env *env)
```

`env` only **reads** from the environment. It doesn't modify the list structure, so it doesn't need `&env`.

**Return Value:**

Always returns 0. The `env` builtin cannot fail in normal circumstances.

---

## builtin_exit.c

### is_numeric

**Function Signature:**
```c
static int is_numeric(char *str)
```

**Purpose:**  
Check if a string represents a valid numeric value.

**Parameters:**
- `str`: String to check

**Return Value:**
- 1 if the string is numeric
- 0 if the string contains non-numeric characters

**Detailed Description:**

This helper function validates that a string contains only digits, with an optional leading `+` or `-` sign.

**Implementation:**

```c
int i;

i = 0;
if (str[i] == '-' || str[i] == '+')
    i++;  // Skip optional sign
while (str[i]) {
    if (str[i] < '0' || str[i] > '9')
        return (0);  // Non-digit found
    i++;
}
return (1);  // All characters are digits
```

**Algorithm:**

```
1. Start at index 0
2. If first character is '+' or '-':
   - Skip it (increment i)
3. For each remaining character:
   - If not a digit ('0'-'9'):
     - Return 0 (not numeric)
4. Return 1 (all digits)
```

**Examples:**

**Example 1: Positive integer**
```c
is_numeric("123");     // Returns: 1
is_numeric("0");       // Returns: 1
is_numeric("999999");  // Returns: 1
```

**Example 2: Negative integer**
```c
is_numeric("-123");    // Returns: 1
is_numeric("-0");      // Returns: 1
is_numeric("-999");    // Returns: 1
```

**Example 3: With plus sign**
```c
is_numeric("+123");    // Returns: 1
is_numeric("+0");      // Returns: 1
```

**Example 4: Invalid formats**
```c
is_numeric("abc");     // Returns: 0 (letters)
is_numeric("12a34");   // Returns: 0 (letter in middle)
is_numeric("12.34");   // Returns: 0 (decimal point)
is_numeric("12 34");   // Returns: 0 (space)
is_numeric("");        // Returns: 1 (empty string - edge case!)
is_numeric("-");       // Returns: 1 (only sign - edge case!)
is_numeric("+");       // Returns: 1 (only sign - edge case!)
```

**Edge Cases:**

**Case 1: Empty string**
```c
is_numeric("");
```
- Loop doesn't execute (str[0] is '\0')
- Returns 1 (incorrectly considers empty string numeric)

**Fix:**
```c
if (!str || !*str)
    return (0);
```

**Case 2: Only sign**
```c
is_numeric("-");
is_numeric("+");
```
- Skips sign, then loop doesn't execute
- Returns 1 (incorrectly considers sign-only as numeric)

**Fix:**
```c
if (str[i] == '-' || str[i] == '+') {
    i++;
    if (!str[i])  // No digits after sign
        return (0);
}
```

**Case 3: Multiple signs**
```c
is_numeric("--123");
is_numeric("+-123");
```
- Only checks first character for sign
- Second '-' or '+' is treated as non-digit
- Returns 0 (correct behavior)

**Case 4: Leading zeros**
```c
is_numeric("0123");
```
- Returns 1 (valid)
- ft_atoi() will parse as 123

**Case 5: Overflow**
```c
is_numeric("99999999999999999999");
```
- Returns 1 (valid format)
- But ft_atoi() may overflow
- This function doesn't check for overflow

**Comparison with Bash:**

Bash `exit` accepts:
- Integers: `123`, `-456`
- Octal: `0123` (treated as 83 in decimal)
- Hex: `0x1F` (treated as 31 in decimal)

This implementation only accepts decimal integers.

**Why Static?**

```c
static int is_numeric(char *str)
```

- Only used within builtin_exit.c
- Not needed by other files
- Reduces namespace pollution

---

### builtin_exit

**Function Signature:**
```c
int builtin_exit(char **args, t_shell *shell)
```

**Purpose:**  
Exit the shell with an optional exit code.

**Parameters:**
- `args`: NULL-terminated array of arguments
  - `args[0]`: "exit" (command name)
  - `args[1]`: Optional exit code (must be numeric)
- `shell`: Shell state structure

**Return Value:**
- Exit code specified by user (0-255)
- 255 if argument is non-numeric
- 1 if too many arguments (but doesn't exit)
- 0 if no arguments

**Detailed Description:**

The `exit` builtin terminates the shell. It supports an optional numeric argument to specify the exit code.

**Implementation:**

```c
int exit_code;

ft_putendl_fd("exit", 1);  // Always print "exit"

if (args[1]) {
    // Argument provided
    if (!is_numeric(args[1])) {
        // Non-numeric argument
        ft_putstr_fd("minishell: exit: ", 2);
        ft_putstr_fd(args[1], 2);
        ft_putendl_fd(": numeric argument required", 2);
        shell->should_exit = 1;
        return (255);
    }
    if (args[2]) {
        // Too many arguments
        ft_putendl_fd("minishell: exit: too many arguments", 2);
        return (1);  // Don't exit!
    }
    exit_code = ft_atoi(args[1]);
    shell->should_exit = 1;
    return (exit_code);
}

// No arguments
shell->should_exit = 1;
return (0);
```

**Algorithm:**

```
1. Print "exit" to stdout

2. Check if args[1] exists:
   
   a. If NO (no argument):
      - Set should_exit = 1
      - Return 0
   
   b. If YES (argument provided):
      - Check if numeric:
        - If NO:
          - Print error
          - Set should_exit = 1
          - Return 255
      
      - Check if args[2] exists (too many args):
        - If YES:
          - Print error
          - Return 1 (DON'T set should_exit)
      
      - Parse exit code from args[1]
      - Set should_exit = 1
      - Return exit code
```

**Examples:**

**Example 1: Exit without argument**
```bash
exit
```
Execution:
```
Print "exit"
args[1] is NULL
shell->should_exit = 1
Return 0
Shell exits with status 0
```

**Example 2: Exit with numeric argument**
```bash
exit 42
```
Execution:
```
Print "exit"
is_numeric("42") → 1 (valid)
No args[2]
exit_code = ft_atoi("42") → 42
shell->should_exit = 1
Return 42
Shell exits with status 42
```

**Example 3: Exit with non-numeric argument**
```bash
exit abc
```
Execution:
```
Print "exit"
is_numeric("abc") → 0 (invalid)
Print "minishell: exit: abc: numeric argument required"
shell->should_exit = 1
Return 255
Shell exits with status 255
```

**Example 4: Exit with too many arguments**
```bash
exit 1 2
```
Execution:
```
Print "exit"
is_numeric("1") → 1 (valid)
args[2] exists ("2")
Print "minishell: exit: too many arguments"
Return 1
shell->should_exit NOT set
Shell DOES NOT exit!
```

**Example 5: Exit with negative number**
```bash
exit -1
```
Execution:
```
Print "exit"
is_numeric("-1") → 1 (valid)
exit_code = ft_atoi("-1") → -1
shell->should_exit = 1
Return -1
Shell exits with status 255 (256 - 1)
```

**Exit Code Range:**

Exit codes are 0-255 (8-bit unsigned):
```bash
exit 0     # Exit status: 0
exit 1     # Exit status: 1
exit 255   # Exit status: 255
exit 256   # Exit status: 0 (256 % 256)
exit 257   # Exit status: 1 (257 % 256)
exit -1    # Exit status: 255 (256 - 1)
exit -2    # Exit status: 254 (256 - 2)
```

The shell automatically wraps exit codes to 0-255 range.

**Why Print "exit"?**

```c
ft_putendl_fd("exit", 1);
```

This matches bash behavior:
```bash
$ exit
exit
$ # (shell exits)
```

The "exit" message is printed to stdout before exiting.

**should_exit Flag:**

```c
shell->should_exit = 1;
```

This flag tells the main loop to terminate:

```c
// In shell_loop():
while (!shell->should_exit) {
    // Read and process commands
}
```

When `should_exit` is set to 1, the loop exits and the program terminates.

**Too Many Arguments Behavior:**

```bash
exit 1 2
```

Bash behavior:
- Prints error: "exit: too many arguments"
- Does NOT exit
- Returns 1

This implementation matches bash behavior:
```c
if (args[2]) {
    ft_putendl_fd("minishell: exit: too many arguments", 2);
    return (1);  // Don't set should_exit
}
```

**Why Exit Must Be a Builtin:**

If `exit` were an external command:

```c
pid = fork();
if (pid == 0) {
    // Child process
    exit(42);  // Child exits
}
wait(&status);
// Parent shell continues running!
```

The child would exit, but the parent shell would continue. This is why `exit` must be a builtin.

**Comparison with Bash:**

| Feature | This Implementation | Bash |
|---------|-------------------|------|
| `exit` | Exits with 0 | Exits with last status |
| `exit N` | Exits with N | Exits with N |
| `exit abc` | Error, exits with 255 | Error, exits with 255 |
| `exit 1 2` | Error, doesn't exit | Error, doesn't exit |
| `exit -1` | Exits with 255 | Exits with 255 |
| `exit 256` | Exits with 0 | Exits with 0 |

**Difference:**
```bash
# Bash:
$ false  # Sets exit status to 1
$ exit   # Exits with 1 (last status)

# This implementation:
$ false  # Sets exit status to 1
$ exit   # Exits with 0 (always)
```

To match bash:
```c
if (!args[1]) {
    shell->should_exit = 1;
    return (shell->exit_status);  // Return last status
}
```

**Exit in Subshell:**

```bash
(exit 42)
echo $?
```

Output: `42`

The subshell exits with 42, but the parent shell continues.

**Exit in Pipeline:**

```bash
exit | cat
```

The `exit` runs in a subshell (due to pipe), so it doesn't exit the main shell.

**Exit Status Special Values:**

- 0: Success
- 1: General error
- 2: Misuse of shell builtin
- 126: Command found but not executable
- 127: Command not found
- 128+N: Terminated by signal N
- 130: Terminated by Ctrl-C (SIGINT = 2, 128+2 = 130)
- 255: Exit status out of range

---

## Summary

The built-in commands module consists of 11 functions across 8 files:

**builtins.c** (2 functions):
- `is_builtin`: Check if command is a builtin
- `execute_builtin`: Dispatch to appropriate builtin function

**builtin_echo.c** (1 function):
- `builtin_echo`: Print arguments with optional -n flag

**builtin_cd.c** (1 function):
- `builtin_cd`: Change current directory

**builtin_pwd.c** (1 function):
- `builtin_pwd`: Print current directory

**builtin_export.c** (3 functions):
- `parse_export_arg`: Parse KEY=VALUE format
- `update_or_add_env`: Update or add environment variable
- `builtin_export`: Set environment variables

**builtin_unset.c** (1 function):
- `builtin_unset`: Remove environment variables

**builtin_env.c** (1 function):
- `builtin_env`: Display all environment variables

**builtin_exit.c** (2 functions):
- `is_numeric`: Validate numeric string
- `builtin_exit`: Exit shell with optional code

These functions implement the seven mandatory built-in commands required by the 42 School minishell project:
1. `echo` with `-n` option
2. `cd` with relative or absolute path
3. `pwd` with no options
4. `export` with no options
5. `unset` with no options
6. `env` with no options or arguments
7. `exit` with no options

---

**End of Documentation**
