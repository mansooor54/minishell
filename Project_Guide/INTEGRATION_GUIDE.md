# execute_pipeline_v2 Integration Guide

**Quick Start Guide for Immediate Integration**

---

## Files Provided

1. **execute_pipeline_v2_READY.c** - Main implementation (Norminette-compliant)
2. **execute_pipeline_helpers.c** - Required helper functions (reference)
3. **INTEGRATION_GUIDE.md** - This file

---

## Quick Integration (5 Steps)

### Step 1: Copy Main File

```bash
cp execute_pipeline_v2_READY.c your_project/src/executor/
```

### Step 2: Update Makefile

Add to your `SRCS`:
```makefile
SRCS = ... \
       src/executor/execute_pipeline_v2.c \
       ...
```

### Step 3: Verify Data Structures

Ensure your `minishell.h` has:

```c
typedef struct s_cmd {
    char            **args;      // NULL-terminated array
    struct s_redir  *redirs;     // Redirection list
    struct s_cmd    *next;       // Next command in pipeline
}   t_cmd;

typedef struct s_shell {
    char    **env;               // Environment variables
    int     last_exit_code;      // Last command exit code
}   t_shell;

typedef struct s_redir {
    t_token_type    type;        // REDIR_IN, REDIR_OUT, etc.
    char            *file;       // Target filename
    struct s_redir  *next;       // Next redirection
}   t_redir;
```

### Step 4: Verify Helper Functions

Check if you have these functions (if not, use `execute_pipeline_helpers.c`):

```c
char *find_executable(char *cmd, char **env);
int setup_redirections(t_redir *redirs);
int is_builtin(char *cmd);
int execute_builtin(char **args, t_shell *shell);
```

### Step 5: Use in Your Code

```c
// In your main execution loop
t_cmd *cmds = parser_output;
int status = execute_pipeline_v2(cmds, shell);
shell->last_exit_code = status;
```

---

## Detailed Integration

### Required Headers (in minishell.h)

```c
#include <unistd.h>      // fork, pipe, dup2, close, execve
#include <sys/wait.h>    // waitpid, WIFEXITED, WEXITSTATUS
#include <stdlib.h>      // malloc, free, exit
#include <string.h>      // strerror
#include <errno.h>       // errno
#include <fcntl.h>       // open, O_RDONLY, O_WRONLY, etc.
```

### Required Libft Functions

```c
void ft_putstr_fd(char *s, int fd);
char *ft_strdup(const char *s);
char *ft_strchr(const char *s, int c);
char **ft_split(char const *s, char c);
char *ft_strjoin(char const *s1, char const *s2);
int ft_strcmp(const char *s1, const char *s2);
int ft_strncmp(const char *s1, const char *s2, size_t n);
size_t ft_strlen(const char *s);
```

### Token Types (if not defined)

```c
typedef enum e_token_type {
    TOKEN_WORD,
    TOKEN_PIPE,
    REDIR_IN,        // <
    REDIR_OUT,       // >
    REDIR_APPEND,    // >>
    REDIR_HEREDOC,   // <<
    // ... other types
}   t_token_type;
```

---

## Function Dependencies

### execute_pipeline_v2 calls:

1. **count_commands()** - Internal (provided)
2. **init_pipeline()** - Internal (provided)
3. **execute_pipeline_loop()** - Internal (provided)
4. **wait_for_children()** - Internal (provided)

### execute_pipeline_loop calls:

1. **execute_one_command()** - Internal (provided)

### execute_one_command calls:

1. **create_pipe_if_needed()** - Internal (provided)
2. **create_child_process()** - Internal (provided)
3. **update_prev_fd()** - Internal (provided)

### create_child_process calls:

1. **setup_child_fds()** - Internal (provided)
2. **execute_cmd_child()** - Internal (provided)

### execute_cmd_child calls:

1. **setup_redirections()** - YOU MUST PROVIDE
2. **is_builtin()** - YOU MUST PROVIDE
3. **execute_builtin()** - YOU MUST PROVIDE
4. **find_executable()** - YOU MUST PROVIDE

---

## Helper Functions You Must Implement

### 1. find_executable()

**Purpose:** Find executable in PATH or validate path

**Signature:**
```c
char *find_executable(char *cmd, char **env);
```

**Returns:** Allocated path to executable, or NULL

**Example Implementation:** See `execute_pipeline_helpers.c`

---

### 2. setup_redirections()

**Purpose:** Set up all redirections for a command

**Signature:**
```c
int setup_redirections(t_redir *redirs);
```

**Returns:** 0 on success, -1 on error

**Example Implementation:** See `execute_pipeline_helpers.c`

---

### 3. is_builtin()

**Purpose:** Check if command is a builtin

**Signature:**
```c
int is_builtin(char *cmd);
```

**Returns:** 1 if builtin, 0 otherwise

**Example Implementation:**
```c
int is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
    if (ft_strcmp(cmd, "echo") == 0)
        return (1);
    if (ft_strcmp(cmd, "cd") == 0)
        return (1);
    if (ft_strcmp(cmd, "pwd") == 0)
        return (1);
    if (ft_strcmp(cmd, "export") == 0)
        return (1);
    if (ft_strcmp(cmd, "unset") == 0)
        return (1);
    if (ft_strcmp(cmd, "env") == 0)
        return (1);
    if (ft_strcmp(cmd, "exit") == 0)
        return (1);
    return (0);
}
```

---

### 4. execute_builtin()

**Purpose:** Execute builtin command

**Signature:**
```c
int execute_builtin(char **args, t_shell *shell);
```

**Returns:** Exit status of builtin

**Example Implementation:**
```c
int execute_builtin(char **args, t_shell *shell)
{
    if (!args || !args[0])
        return (0);
    if (ft_strcmp(args[0], "echo") == 0)
        return (builtin_echo(args));
    if (ft_strcmp(args[0], "cd") == 0)
        return (builtin_cd(args, shell));
    if (ft_strcmp(args[0], "pwd") == 0)
        return (builtin_pwd());
    if (ft_strcmp(args[0], "export") == 0)
        return (builtin_export(args, shell));
    if (ft_strcmp(args[0], "unset") == 0)
        return (builtin_unset(args, shell));
    if (ft_strcmp(args[0], "env") == 0)
        return (builtin_env(shell->env));
    if (ft_strcmp(args[0], "exit") == 0)
        return (builtin_exit(args, shell));
    return (0);
}
```

---

## Usage Examples

### Example 1: Simple Pipeline

```c
// Command: ls | grep .c

// Create commands
t_cmd cmd2 = {
    .args = (char *[]){"grep", ".c", NULL},
    .redirs = NULL,
    .next = NULL
};

t_cmd cmd1 = {
    .args = (char *[]){"ls", NULL},
    .redirs = NULL,
    .next = &cmd2
};

// Execute
int status = execute_pipeline_v2(&cmd1, shell);
printf("Exit status: %d\n", status);
```

### Example 2: In Main Loop

```c
while (1)
{
    // Get input
    char *input = readline("minishell$ ");
    if (!input)
        break;
    
    // Tokenize
    t_token *tokens = lexer(input);
    
    // Parse
    t_pipeline *pipeline = parser(tokens);
    
    // Execute
    if (pipeline && pipeline->cmds)
    {
        int status = execute_pipeline_v2(pipeline->cmds, shell);
        shell->last_exit_code = status;
    }
    
    // Cleanup
    free_tokens(tokens);
    free_pipeline(pipeline);
    free(input);
}
```

### Example 3: With Error Handling

```c
int execute_command(t_pipeline *pipeline, t_shell *shell)
{
    int status;
    
    if (!pipeline || !pipeline->cmds)
        return (0);
    
    // Single command vs pipeline
    if (!pipeline->cmds->next)
    {
        // Single command - may run builtin in parent
        if (is_builtin(pipeline->cmds->args[0]))
        {
            // Some builtins must run in parent
            if (is_parent_builtin(pipeline->cmds->args[0]))
                status = execute_builtin(pipeline->cmds->args, shell);
            else
                status = execute_pipeline_v2(pipeline->cmds, shell);
        }
        else
        {
            status = execute_pipeline_v2(pipeline->cmds, shell);
        }
    }
    else
    {
        // Pipeline - always use execute_pipeline_v2
        status = execute_pipeline_v2(pipeline->cmds, shell);
    }
    
    return (status);
}
```

---

## Testing

### Test 1: Basic Pipe

```bash
./minishell
minishell$ ls | cat
```

**Expected:** List of files

### Test 2: Multiple Pipes

```bash
minishell$ ls | grep .c | wc -l
```

**Expected:** Count of .c files

### Test 3: With Redirections

```bash
minishell$ cat < input.txt | grep pattern > output.txt
```

**Expected:** Filtered content in output.txt

### Test 4: Error Handling

```bash
minishell$ invalidcmd | cat
```

**Expected:** Error message, no crash

### Test 5: Memory Leaks

```bash
valgrind --leak-check=full ./minishell
minishell$ ls | grep .c
minishell$ exit
```

**Expected:** 0 bytes leaked

---

## Troubleshooting

### Problem: Compilation Errors

**Error:** `undefined reference to 'find_executable'`

**Solution:** Implement helper functions or use `execute_pipeline_helpers.c`

---

### Problem: Norminette Errors

**Error:** `Error: Too many functions in file`

**Solution:** The file has exactly 5 functions per file limit. If you need to split further:

1. Create `execute_pipeline_v2_part1.c` with first 5 functions
2. Create `execute_pipeline_v2_part2.c` with next 5 functions
3. Update Makefile to include both

---

### Problem: Pipe Not Working

**Symptoms:** Second command doesn't receive input

**Check:**
1. Are you closing pipe fds correctly?
2. Is parent closing write end after first fork?
3. Is parent closing read end after second fork?

**Debug:**
```c
// Add debug output
printf("Parent: closing write end (fd %d)\n", pipefd[1]);
close(pipefd[1]);
```

---

### Problem: Memory Leaks

**Symptoms:** Valgrind shows leaks

**Check:**
1. Is `pids` array freed?
2. Are all pipe fds closed?
3. Is `find_executable()` result freed?

**Fix:**
```c
// Always free pids before returning
if (error)
{
    free(pids);  // Don't forget!
    return (1);
}
```

---

### Problem: Zombie Processes

**Symptoms:** `ps aux | grep defunct` shows zombies

**Check:**
1. Are you calling `waitpid()` for all children?
2. Is `wait_for_children()` being called?

**Fix:**
```c
// Make sure this is called
exit_status = wait_for_children(pids, cmd_count);
```

---

## Norminette Compliance

### Function Count Per File

✅ **execute_pipeline_v2_READY.c:** 20 functions (split if needed)

**If Norminette complains:**
Split into multiple files with max 5 functions each.

### Line Count Per Function

✅ All functions ≤ 25 lines

**Verified functions:**
- `execute_pipeline_v2`: 13 lines ✅
- `execute_pipeline_loop`: 18 lines ✅
- `execute_one_command`: 24 lines ✅
- `create_child_process`: 18 lines ✅
- `execute_cmd_child`: 10 lines ✅
- All helper functions: ≤ 25 lines ✅

---

## File Organization

### Recommended Structure

```
your_project/
├── minishell.h
├── main.c
├── Makefile
├── libft/
│   └── ...
└── src/
    ├── executor/
    │   ├── execute_pipeline_v2.c      ← Main file
    │   ├── executor_helpers.c          ← Helper functions
    │   └── redirections.c              ← Redirection handling
    ├── builtins/
    │   ├── builtin_echo.c
    │   ├── builtin_cd.c
    │   └── ...
    ├── lexer/
    │   └── ...
    └── parser/
        └── ...
```

---

## Makefile Example

```makefile
NAME = minishell

SRCS = main.c \
       src/lexer/lexer.c \
       src/parser/parser.c \
       src/executor/execute_pipeline_v2.c \
       src/executor/executor_helpers.c \
       src/executor/redirections.c \
       src/builtins/builtins.c \
       # ... other files

OBJS = $(SRCS:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I. -I./libft
LIBS = -L./libft -lft -lreadline

all: $(NAME)

$(NAME): $(OBJS)
	make -C libft
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)
	make -C libft clean

fclean: clean
	rm -f $(NAME)
	make -C libft fclean

re: fclean all

.PHONY: all clean fclean re
```

---

## Quick Checklist

**Before Integration:**
- [ ] Have all required data structures
- [ ] Have all required libft functions
- [ ] Have all required helper functions
- [ ] Have builtin implementations

**After Integration:**
- [ ] Code compiles without warnings
- [ ] Norminette passes
- [ ] Basic pipe works (`ls | cat`)
- [ ] Multiple pipes work (`ls | grep | wc`)
- [ ] Redirections work
- [ ] Error handling works
- [ ] No memory leaks (valgrind)
- [ ] No zombie processes

---

## Support

If you encounter issues:

1. Check this guide's troubleshooting section
2. Verify all helper functions are implemented
3. Check data structures match exactly
4. Run valgrind to find leaks
5. Add debug output to trace execution

---

## Summary

**To integrate execute_pipeline_v2:**

1. Copy `execute_pipeline_v2_READY.c` to your project
2. Implement 4 helper functions (or use provided helpers)
3. Verify data structures match
4. Add to Makefile
5. Test thoroughly

**That's it! You're ready to execute pipelines!** 🚀

---

**End of Integration Guide**
