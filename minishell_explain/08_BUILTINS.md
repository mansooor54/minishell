# Builtin Commands

## Files
- `src/builtins/builtins.c`
- `src/builtins/builtin_echo.c`
- `src/builtins/builtin_cd.c`
- `src/builtins/builtin_cd_utils.c`
- `src/builtins/builtin_pwd.c`
- `src/builtins/builtin_export.c`
- `src/builtins/builtin_export_utils.c`
- `src/builtins/builtin_unset.c`
- `src/builtins/builtin_env.c`
- `src/builtins/builtin_exit.c`

---

## What are Builtins?

Builtins are commands implemented directly in the shell, not as external programs. They MUST be builtins because they need to:
- Modify shell state (`cd`, `export`, `unset`, `exit`)
- Access shell internals (`env`)
- Or simply are required to be builtins (`echo`, `pwd`)

---

## builtins.c

### is_builtin()
```c
int is_builtin(char *cmd)
{
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

**Purpose**: Check if command is a builtin.

---

### execute_builtin()
```c
int execute_builtin(t_cmd *cmd, t_shell *shell)
{
    if (!cmd || !cmd->args || !cmd->args[0])
        return (0);
    if (ft_strcmp(cmd->args[0], "echo") == 0)
        return (builtin_echo(cmd->args));
    if (ft_strcmp(cmd->args[0], "cd") == 0)
        return (builtin_cd(cmd->args, &shell->env));
    if (ft_strcmp(cmd->args[0], "pwd") == 0)
        return (builtin_pwd());
    if (ft_strcmp(cmd->args[0], "export") == 0)
        return (builtin_export(cmd->args, &shell->env));
    if (ft_strcmp(cmd->args[0], "unset") == 0)
        return (builtin_unset(cmd->args, &shell->env));
    if (ft_strcmp(cmd->args[0], "env") == 0)
        return (builtin_env(shell->env));
    if (ft_strcmp(cmd->args[0], "exit") == 0)
        return (builtin_exit(cmd->args, shell));
    return (0);
}
```

**Purpose**: Dispatch to appropriate builtin function.

---

## echo

### builtin_echo()
```c
int builtin_echo(char **args)
{
    int i;
    int newline;

    i = 1;
    newline = 1;
    while (args[i] && is_dash_n(args[i]))
    {
        newline = 0;
        i++;
    }
    while (args[i])
    {
        ft_putstr_fd(args[i], 1);
        if (args[i + 1])
            ft_putstr_fd(" ", 1);
        i++;
    }
    if (newline)
        ft_putstr_fd("\n", 1);
    return (0);
}
```

**Purpose**: Print arguments to stdout.

**Features**:
- `-n` flag: suppress trailing newline
- Multiple `-n` or `-nnn` are valid

**Examples**:
```bash
echo hello world      # Output: hello world\n
echo -n hello         # Output: hello (no newline)
echo -n -n hello      # Output: hello (no newline)
echo "-n" hello       # Output: hello (no newline, -n is option)
echo hello -n         # Output: hello -n\n (-n not at start)
```

---

### is_dash_n()
```c
static int is_dash_n(char *s)
{
    int i;

    if (!s || s[0] != '-' || s[1] != 'n')
        return (0);
    i = 2;
    while (s[i] == 'n')
        i++;
    return (s[i] == '\0');
}
```

**Purpose**: Check if argument is `-n`, `-nn`, `-nnn`, etc.

**Valid**: `-n`, `-nn`, `-nnnnn`
**Invalid**: `-na`, `-n1`, `n`

---

## cd

### builtin_cd()
```c
int builtin_cd(char **args, t_env **env)
{
    char *oldpwd;
    char *target;
    int  print_after;

    oldpwd = dup_cwd();
    target = resolve_target(args, *env, &print_after);
    if (!target)
    {
        free(oldpwd);
        return (1);
    }
    if (change_directory(target))
    {
        free(target);
        free(oldpwd);
        return (1);
    }
    update_pwd_vars(env, oldpwd, print_after);
    free(oldpwd);
    free(target);
    return (0);
}
```

**Purpose**: Change current directory.

**Features**:
- `cd` (no args) → go to $HOME
- `cd path` → go to path
- `cd -` → go to previous directory (OLDPWD)
- Updates PWD and OLDPWD environment variables

---

### resolve_target()
```c
char *resolve_target(char **args, t_env *env, int *print_after)
{
    *print_after = 0;
    if (!args[1])  // No argument - go to HOME
    {
        char *home = get_env_value(env, "HOME");
        if (!home)
        {
            ft_putendl_fd("minishell: cd: HOME not set", 2);
            return (NULL);
        }
        return (ft_strdup(home));
    }
    if (ft_strcmp(args[1], "-") == 0)  // cd -
    {
        char *oldpwd = get_env_value(env, "OLDPWD");
        if (!oldpwd)
        {
            ft_putendl_fd("minishell: cd: OLDPWD not set", 2);
            return (NULL);
        }
        *print_after = 1;  // Print new directory
        return (ft_strdup(oldpwd));
    }
    return (ft_strdup(args[1]));
}
```

**Purpose**: Determine target directory.

---

## pwd

### builtin_pwd()
```c
int builtin_pwd(void)
{
    char *cwd;

    cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        perror("minishell: pwd");
        return (1);
    }
    ft_putendl_fd(cwd, 1);
    free(cwd);
    return (0);
}
```

**Purpose**: Print current working directory.

**Note**: `getcwd(NULL, 0)` allocates buffer automatically (POSIX extension).

---

## export

### builtin_export()
```c
int builtin_export(char **args, t_env **env)
{
    int i;
    int ret;
    char *key;
    char *value;

    if (!args[1])  // No args - print all
        return (print_export_list(*env));
    ret = 0;
    i = 1;
    while (args[i])
    {
        if (!is_valid_identifier(args[i]))
        {
            // Print error
            ret = 1;
        }
        else
        {
            parse_env_string(args[i], &key, &value);
            env_set_value(env, key, value);
            free(key);
            free(value);
        }
        i++;
    }
    return (ret);
}
```

**Purpose**: Set or display environment variables.

**Features**:
- `export` → print all variables (sorted, with `declare -x` in bash)
- `export VAR` → mark variable for export (no value change)
- `export VAR=value` → set and export variable

---

### is_valid_identifier()
```c
int is_valid_identifier(char *s)
{
    int i;

    if (!s || !*s)
        return (0);
    if (!ft_isalpha(s[0]) && s[0] != '_')
        return (0);
    i = 1;
    while (s[i] && s[i] != '=')
    {
        if (!ft_isalnum(s[i]) && s[i] != '_')
            return (0);
        i++;
    }
    return (1);
}
```

**Purpose**: Check if string is valid variable name.

**Rules**:
- Must start with letter or underscore
- Can contain letters, digits, underscores
- Stops at `=` sign

**Valid**: `VAR`, `_var`, `var123`, `VAR=value`
**Invalid**: `123var`, `var-name`, `var.name`

---

## unset

### builtin_unset()
```c
int builtin_unset(char **args, t_env **env)
{
    int i;

    if (!args[1])
        return (0);
    i = 1;
    while (args[i])
    {
        remove_env_node(env, args[i]);
        i++;
    }
    return (0);
}
```

**Purpose**: Remove environment variables.

**Note**: Silently ignores non-existent variables.

---

## env

### builtin_env()
```c
int builtin_env(t_env *env)
{
    while (env)
    {
        if (env->value)  // Only print if has value
        {
            ft_putstr_fd(env->key, 1);
            ft_putstr_fd("=", 1);
            ft_putendl_fd(env->value, 1);
        }
        env = env->next;
    }
    return (0);
}
```

**Purpose**: Print all environment variables.

**Format**: `KEY=VALUE` (one per line)

**Difference from export**: `env` only prints variables with values.

---

## exit

### builtin_exit()
```c
int builtin_exit(char **args, t_shell *shell)
{
    long long exit_code;
    char *arg;

    ft_putendl_fd("exit", 1);
    if (args[1] && args[2])
    {
        ft_putstr_fd("minishell: exit: too many arguments\n", 2);
        shell->exit_status = 1;
        return (1);  // Don't exit, just return error
    }
    if (!args[1])
        exit_code = shell->exit_status;
    else
    {
        arg = args[1];
        if (!is_valid_number(arg) || is_numeric_overflow(arg))
            exit_numeric_error(arg);  // Prints error and exits 255
        exit_code = ft_atoll(arg);
        exit_code = exit_code % 256;
        if (exit_code < 0)
            exit_code += 256;
    }
    exit(exit_code);
}
```

**Purpose**: Exit the shell.

**Behavior**:
- `exit` → exit with last command's status
- `exit 42` → exit with code 42
- `exit abc` → error, exit with 255
- `exit 1 2` → error, don't exit (too many args)

**Exit code normalization**:
- Codes are modulo 256 (0-255 range)
- Negative codes wrap around

---

## Builtin Summary Table

| Command | Purpose | Modifies Shell | Exit Codes |
|---------|---------|----------------|------------|
| `echo` | Print arguments | No | 0 |
| `cd` | Change directory | Yes (PWD, OLDPWD) | 0 or 1 |
| `pwd` | Print current dir | No | 0 or 1 |
| `export` | Set env variables | Yes | 0 or 1 |
| `unset` | Remove env variables | Yes | 0 |
| `env` | Print environment | No | 0 |
| `exit` | Exit shell | Yes (terminates) | N/A |

---

## Why Builtins Must Run in Parent

```
cd /tmp         # Must change PARENT's directory
export VAR=1    # Must modify PARENT's environment
exit 0          # Must exit PARENT process
```

If these ran in a child process:
1. Child's directory change is lost when child exits
2. Child's environment changes are lost
3. Child exits, but parent keeps running

That's why single builtins (not in a pipeline) run directly in the parent process.

---

## Builtins in Pipelines

```
export VAR=1 | cat    # VAR is NOT set in parent!
```

When a builtin is part of a pipeline, it runs in a child (to connect pipes). Changes are lost. This matches bash behavior.
