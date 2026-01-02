# Environment Management

## Files
- `src/environment/env.c`
- `src/environment/env_node.c`
- `src/environment/env_utils.c`
- `src/environment/env_array.c`

---

## What is the Environment?

The environment is a set of key-value pairs inherited from the parent process. Programs use these to configure behavior (PATH for finding executables, HOME for user directory, etc.).

Minishell stores the environment as a linked list for easy modification.

---

## Data Structure

```c
typedef struct s_env
{
    char            *key;    // Variable name (e.g., "HOME")
    char            *value;  // Variable value (e.g., "/Users/john")
    struct s_env    *next;   // Next node in list
}   t_env;
```

**Example environment**:
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ key: "HOME"     │───►│ key: "PATH"     │───►│ key: "USER"     │───► NULL
│ value: "/home"  │    │ value: "/bin"   │    │ value: "john"   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

---

## env.c

### init_env()
```c
t_env *init_env(char **envp)
{
    t_env   *env;
    int     i;
    char    *key;
    char    *value;
    char    *eq_pos;

    env = NULL;
    i = 0;
    while (envp[i])
    {
        eq_pos = ft_strchr(envp[i], '=');
        if (eq_pos)
        {
            key = ft_substr(envp[i], 0, eq_pos - envp[i]);
            value = ft_strdup(eq_pos + 1);
            add_env_node(&env, create_env_node(key, value));
            free(key);
            free(value);
        }
        i++;
    }
    return (env);
}
```

**Purpose**: Convert envp array to linked list.

**Input**: `envp` from main() - array of "KEY=VALUE" strings
**Output**: Linked list of t_env nodes

**Process**:
1. Find `=` position in each string
2. Extract key (before `=`)
3. Extract value (after `=`)
4. Create and add node

---

### init_shell()
```c
void init_shell(t_shell *shell, char **envp)
{
    ft_bzero(shell, sizeof(*shell));
    if (isatty(STDIN_FILENO))
        shell->interactive = 1;
    shell->env = init_env(envp);
    shell->exit_status = 0;
    shell->should_exit = 0;
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}
```

**Purpose**: Initialize shell state including environment.

---

### increment_shlvl()
```c
void increment_shlvl(t_env **env)
{
    t_env   *current;
    int     shlvl_value;

    current = *env;
    while (current)
    {
        if (ft_strcmp(current->key, "SHLVL") == 0)
        {
            shlvl_value = ft_atoi(current->value);
            if (shlvl_value < 0)
                shlvl_value = 0;
            shlvl_value++;
            free(current->value);
            current->value = ft_itoa(shlvl_value);
            if (!current->value)
                current->value = ft_strdup("1");
            return;
        }
        current = current->next;
    }
    add_env_node(env, create_env_node("SHLVL", "1"));
}
```

**Purpose**: Increment SHLVL when starting shell.

**SHLVL**: Tracks shell nesting level.
```bash
echo $SHLVL    # 1
./minishell
echo $SHLVL    # 2
./minishell
echo $SHLVL    # 3
```

---

## env_node.c

### create_env_node()
```c
t_env *create_env_node(char *key, char *value)
{
    t_env *node;

    node = malloc(sizeof(t_env));
    if (!node)
        return (NULL);
    node->key = ft_strdup(key);
    if (!node->key)
    {
        free(node);
        return (NULL);
    }
    if (value)
        node->value = ft_strdup(value);
    else
        node->value = NULL;
    node->next = NULL;
    return (node);
}
```

**Purpose**: Create a new environment node.

**Note**: Node owns copies of key and value (duplicated).

---

### add_env_node()
```c
void add_env_node(t_env **env, t_env *new_node)
{
    t_env *current;

    if (!new_node)
        return;
    if (!*env)
    {
        *env = new_node;
        return;
    }
    current = *env;
    while (current->next)
        current = current->next;
    current->next = new_node;
}
```

**Purpose**: Append node to end of list.

---

### remove_env_node()
```c
void remove_env_node(t_env **env, char *key)
{
    t_env *current;
    t_env *prev;

    if (!*env || !key)
        return;
    current = *env;
    prev = NULL;
    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            if (prev)
                prev->next = current->next;
            else
                *env = current->next;
            free(current->key);
            free(current->value);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}
```

**Purpose**: Remove node by key (for `unset`).

---

## env_utils.c

### parse_env_string()
```c
void parse_env_string(char *env_str, char **key, char **value)
{
    char *eq;

    eq = ft_strchr(env_str, '=');
    if (eq)
    {
        *key = ft_substr(env_str, 0, eq - env_str);
        *value = ft_strdup(eq + 1);
    }
    else
    {
        *key = ft_strdup(env_str);
        *value = NULL;
    }
}
```

**Purpose**: Split "KEY=VALUE" or "KEY" string.

**Used by**: `export` command

---

### env_to_array()
```c
char **env_to_array(t_env *env)
{
    char    **envp;
    char    *tmp;
    int     i;

    envp = malloc(sizeof(char *) * (count_env(env) + 1));
    if (!envp)
        return (NULL);
    i = 0;
    while (env)
    {
        if (env->value)
        {
            tmp = ft_strjoin(env->key, "=");
            if (!tmp)
                break;
            envp[i] = ft_strjoin(tmp, env->value);
            free(tmp);
            if (!envp[i])
                break;
            i++;
        }
        env = env->next;
    }
    envp[i] = NULL;
    return (envp);
}
```

**Purpose**: Convert linked list back to array for `execve()`.

**Why needed**: `execve()` requires `char **envp` format.

---

### env_set_value()
```c
void env_set_value(t_env **env, char *key, char *value)
{
    t_env *cur;

    cur = *env;
    while (cur)
    {
        if (ft_strcmp(cur->key, key) == 0)
        {
            free(cur->value);
            if (value)
                cur->value = ft_strdup(value);
            else
                cur->value = NULL;
            return;
        }
        cur = cur->next;
    }
    add_env_node(env, create_env_node(key, value));
}
```

**Purpose**: Set variable (update if exists, create if not).

**Used by**: `export`, `cd` (for PWD/OLDPWD)

---

## Environment Flow Diagram

```
Program Start (main)
         │
         ▼
    ┌──────────────────────────────────────┐
    │            init_env(envp)             │
    │                                       │
    │   envp[]:                             │
    │   ["HOME=/home/user",                 │
    │    "PATH=/usr/bin:/bin",              │
    │    "USER=john", ...]                  │
    │                                       │
    │            ▼                          │
    │                                       │
    │   t_env linked list:                  │
    │   HOME→PATH→USER→...→NULL             │
    └──────────────────────────────────────┘
         │
         ▼
    Shell Operations
         │
    ┌────┴────┐
    │         │
    ▼         ▼
  export    unset
  VAR=x     VAR
    │         │
    ▼         ▼
env_set   remove_env
_value()  _node()
    │         │
    └────┬────┘
         │
         ▼
    execve() needs array
         │
         ▼
    ┌──────────────────────────────────────┐
    │          env_to_array()               │
    │                                       │
    │   t_env list → char **envp            │
    └──────────────────────────────────────┘
```

---

## Special Environment Variables

| Variable | Purpose | Set By |
|----------|---------|--------|
| `PATH` | Executable search path | Inherited |
| `HOME` | User's home directory | Inherited |
| `PWD` | Current directory | `cd` builtin |
| `OLDPWD` | Previous directory | `cd` builtin |
| `SHLVL` | Shell nesting level | Shell init |
| `?` | Exit status (pseudo-var) | Shell |

---

## Key Concepts

### Why Linked List?
- Easy insertion/deletion for `export`/`unset`
- Variable order doesn't matter
- Avoids reallocation when modifying

### Variables Without Values
```bash
export VAR      # VAR exists but has no value
echo $VAR       # Prints nothing
env | grep VAR  # Not shown (env only shows KEY=VALUE)
```

In our implementation, `node->value = NULL` means "exists but no value".

### Environment Inheritance
When `fork()` is called:
1. Child gets copy of parent's environment
2. We convert our linked list to array for `execve()`
3. Child program receives the array

### Memory Management
- Each node owns its key and value (copies, not references)
- When removing/updating, must `free()` old strings
- `free_env()` walks list and frees everything
