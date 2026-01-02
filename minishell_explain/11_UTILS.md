# Utility Functions

## Files
- `src/utils/utils.c`
- `src/utils/utils_num.c`
- `src/utils/ft_atoll.c`

---

## utils.c

### free_array()
```c
void free_array(char **arr)
{
    int i;

    if (!arr)
        return;
    i = 0;
    while (arr[i])
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}
```

**Purpose**: Free a NULL-terminated array of strings.

**Used by**: Cleanup after `env_to_array()`, freeing args, etc.

---

### ft_strjoin_free()
```c
char *ft_strjoin_free(char *s1, char const *s2)
{
    char *result;

    if (!s1)
        return (ft_strdup(s2));
    if (!s2)
        return (s1);
    result = ft_strjoin(s1, s2);
    free(s1);
    return (result);
}
```

**Purpose**: Join strings and free the first one.

**Used for**: Building strings incrementally without leaks.

**Example**:
```c
str = ft_strdup("Hello");
str = ft_strjoin_free(str, " World");  // str now owns "Hello World"
```

---

### free_env()
```c
void free_env(t_env *env)
{
    t_env *tmp;

    while (env)
    {
        tmp = env;
        env = env->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}
```

**Purpose**: Free entire environment linked list.

**Used by**: Cleanup at shell exit.

---

### is_all_space()
```c
int is_all_space(const char *s)
{
    if (!s)
        return (1);
    while (*s)
    {
        if (*s != ' ' && *s != '\t' && *s != '\n')
            return (0);
        s++;
    }
    return (1);
}
```

**Purpose**: Check if string contains only whitespace.

**Used by**: Shell loop to skip empty/whitespace-only input.

---

### safe_strdup_or_empty()
```c
char *safe_strdup_or_empty(const char *src)
{
    if (!src)
        return (ft_strdup(""));
    return (ft_strdup(src));
}
```

**Purpose**: Duplicate string, or return empty string if NULL.

**Prevents**: NULL pointer issues when string might not exist.

---

## utils_num.c

### is_valid_number()
```c
int is_valid_number(char *str)
{
    int i;

    if (!str || !*str)
        return (0);
    i = 0;
    if (str[i] == '+' || str[i] == '-')
        i++;
    if (!str[i])
        return (0);  // Just "+" or "-" is not valid
    while (str[i])
    {
        if (!ft_isdigit(str[i]))
            return (0);
        i++;
    }
    return (1);
}
```

**Purpose**: Check if string is a valid integer.

**Valid**: `"123"`, `"-456"`, `"+789"`, `"0"`
**Invalid**: `""`, `"abc"`, `"12.34"`, `"12abc"`

**Used by**: `exit` builtin for argument validation.

---

### is_numeric_overflow()
```c
int is_numeric_overflow(char *str)
{
    long long   num;
    int         sign;
    int         i;

    i = 0;
    sign = 1;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    // Check if would overflow during ft_atoll
    // Compare against LLONG_MAX / LLONG_MIN
    // ... (implementation details)
    return (0);
}
```

**Purpose**: Check if number would overflow `long long`.

**Range**: -9223372036854775808 to 9223372036854775807

**Used by**: `exit` builtin - numbers outside range get "numeric argument required" error.

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
1. First char: letter or underscore
2. Rest: letters, digits, underscores
3. Stops at `=` (for "VAR=value" format)

**Used by**: `export` builtin.

---

## ft_atoll.c

### ft_atoll()
```c
long long ft_atoll(const char *str)
{
    long long   result;
    int         sign;

    result = 0;
    sign = 1;
    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;
    if (*str == '-' || *str == '+')
    {
        if (*str == '-')
            sign = -1;
        str++;
    }
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        str++;
    }
    return (result * sign);
}
```

**Purpose**: Convert string to `long long` integer.

**Like atoi but**: Handles larger numbers (64-bit range).

**Used by**: `exit` builtin for exit codes.

---

## Memory Management Utilities

### Common Patterns

**Pattern 1: Check before free**
```c
if (ptr)
    free(ptr);
// or simply:
free(ptr);  // free(NULL) is safe in C
```

**Pattern 2: Free and nullify**
```c
free(ptr);
ptr = NULL;  // Prevent double-free
```

**Pattern 3: Free old before assign**
```c
free(node->value);
node->value = ft_strdup(new_value);
```

---

## Error Handling Utilities

### print_error()
```c
void print_error(const char *function, const char *message)
{
    ft_putstr_fd("minishell: ", 2);
    ft_putstr_fd(function, 2);
    ft_putstr_fd(": ", 2);
    ft_putendl_fd(message, 2);
}
```

**Purpose**: Consistent error message format.

**Output**: `minishell: fork: Resource temporarily unavailable`

---

### perror_with_cmd()
```c
void perror_with_cmd(const char *cmd)
{
    ft_putstr_fd("minishell: ", 2);
    ft_putstr_fd(cmd, 2);
    ft_putstr_fd(": ", 2);
    ft_putendl_fd(strerror(errno), 2);
}
```

**Purpose**: Print error with command name and errno message.

---

## File Descriptor Utilities

### safe_close()
```c
void safe_close(int fd)
{
    if (fd >= 0)
        close(fd);
}
```

**Purpose**: Close fd only if valid.

**Why**: Closing -1 is undefined behavior.

---

### cleanup_pipe()
```c
void cleanup_pipe(int pipefd[2])
{
    safe_close(pipefd[0]);
    safe_close(pipefd[1]);
}
```

**Purpose**: Close both ends of a pipe.

---

## String Utilities from Libft

These come from your libft library:

| Function | Purpose |
|----------|---------|
| `ft_strdup()` | Duplicate string |
| `ft_strjoin()` | Concatenate strings |
| `ft_substr()` | Extract substring |
| `ft_strchr()` | Find char in string |
| `ft_strcmp()` | Compare strings |
| `ft_strlen()` | String length |
| `ft_isalpha()` | Is alphabetic? |
| `ft_isdigit()` | Is digit? |
| `ft_isalnum()` | Is alphanumeric? |
| `ft_itoa()` | Int to string |
| `ft_atoi()` | String to int |
| `ft_putstr_fd()` | Print string to fd |
| `ft_putendl_fd()` | Print string + newline to fd |

---

## Key Concepts

### Why Custom Utilities?
- 42 norm requires your own implementations
- Consistent error handling
- Memory management control

### Error Messages to stderr
Always use fd 2 for errors:
```c
ft_putstr_fd("error message", 2);  // stderr
```

### NULL Safety
Most utilities handle NULL gracefully:
```c
free_array(NULL);        // Safe, does nothing
ft_strjoin_free(NULL, "x"); // Returns ft_strdup("x")
is_all_space(NULL);      // Returns 1 (treat as empty)
```
