# Expander

## Files
- `src/expander/expander_core.c`
- `src/expander/expander_vars.c`
- `src/expander/expander_utils.c`
- `src/expander/expander_utils_helpers.c`
- `src/expander/expander_quotes.c`
- `src/expander/expander_pipeline.c`

---

## What is the Expander?

The expander processes command arguments after parsing to:
1. **Expand variables**: `$HOME` → `/Users/john`
2. **Expand exit status**: `$?` → `0`
3. **Remove quotes**: `"hello"` → `hello`

```
Before: echo "$HOME/file" '$USER'
After:  echo  /Users/john/file  $USER
              ↑ expanded         ↑ literal (single quotes)
```

---

## Expansion Context Structure

```c
typedef struct s_exp_ctx
{
    char    *str;        // Original string
    char    *result;     // Output buffer
    int     i;           // Current position in str
    int     j;           // Current position in result
    char    in_quote;    // 0, '\'', or '"'
    t_env   *env;        // Environment variables
    int     exit_status; // Last command exit code
}   t_exp_ctx;
```

---

## expander_core.c

### expand_variables()
```c
char *expand_variables(char *str, t_env *env, int exit_status)
{
    t_exp_ctx c;

    if (!init_ctx(&c, str, env, exit_status))
        return (NULL);
    while (str[c.i])
    {
        if (is_ctx_quote(&c))
            handle_quote(&c);
        else if (should_expand(&c))
            process_dollar(&c);
        else
            c.result[c.j++] = c.str[c.i++];
    }
    c.result[c.j] = '\0';
    return (c.result);
}
```

**Purpose**: Main expansion function - process one string.

**Logic**:
1. For each character:
   - If quote char → toggle quote state
   - If `$` and not in single quotes → expand variable
   - Otherwise → copy character as-is

---

### init_ctx()
```c
static int init_ctx(t_exp_ctx *c, char *s, t_env *env, int st)
{
    size_t cap;

    if (!s)
        return (0);
    c->str = s;
    c->env = env;
    c->exit_status = st;
    c->in_quote = 0;
    c->i = 0;
    c->j = 0;
    cap = ft_strlen(s) * 10 + 4096;  // Generous buffer
    c->result = malloc(cap);
    if (!c->result)
        return (0);
    return (c->result != NULL);
}
```

**Purpose**: Initialize expansion context.

**Why large buffer**: Variable expansion can make strings much longer (e.g., `$PATH` is often 500+ characters).

---

### handle_quote()
```c
static void handle_quote(t_exp_ctx *c)
{
    if (!c->in_quote)
        c->in_quote = c->str[c->i];    // Enter quote mode
    else if (c->in_quote == c->str[c->i])
        c->in_quote = 0;                // Exit quote mode
    c->result[c->j++] = c->str[c->i++]; // Copy quote char
}
```

**Purpose**: Track quote state for expansion decisions.

**Note**: Quotes are copied to result here. They're removed later by `remove_quotes()`.

---

### should_expand()
```c
static int should_expand(t_exp_ctx *c)
{
    return (c->str[c->i] == '$' && c->in_quote != '\'');
}
```

**Purpose**: Check if `$` should trigger expansion.

**Rule**: `$` is expanded EXCEPT inside single quotes.

---

## expander_vars.c

### get_env_value()
```c
char *get_env_value(t_env *env, char *key)
{
    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
            return (env->value);
        env = env->next;
    }
    return (NULL);
}
```

**Purpose**: Look up variable value in environment.

**Returns**: NULL if not found (expands to empty string).

---

### expand_exit_status()
```c
void expand_exit_status(char *result, int *j, int exit_status)
{
    char *tmp;
    int  len;

    if (!result || !j)
        return;
    tmp = ft_itoa(exit_status);
    if (!tmp)
        return;
    len = ft_strlen(tmp);
    ft_strcpy(&result[*j], tmp);
    *j += len;
    free(tmp);
}
```

**Purpose**: Expand `$?` to exit status number.

**Example**: If last command exited with 127, `$?` → `"127"`.

---

### expand_var_name()
```c
void expand_var_name(t_exp_ctx *ctx)
{
    int     start;
    char    *key;
    char    *val;

    start = ctx->i;
    while (ft_isalnum((unsigned char)ctx->str[ctx->i])
        || ctx->str[ctx->i] == '_')
        ctx->i++;
    if (ctx->i == start)
    {
        ctx->result[ctx->j++] = '$';  // Lone $ stays
        return;
    }
    key = ft_substr(ctx->str, start, ctx->i - start);
    if (!key)
        return;
    val = get_env_value(ctx->env, key);
    if (val)
        while (*val)
            ctx->result[ctx->j++] = *val++;
    free(key);
}
```

**Purpose**: Expand `$VARNAME` to its value.

**Variable name rules**:
- Starts with letter or underscore
- Contains letters, digits, underscores
- `$123` → `$` followed by `123` (not a valid var name)

---

## expander_utils.c

### process_dollar()
```c
void process_dollar(t_exp_ctx *c)
{
    if (c->in_quote == '\'')
    {
        c->result[c->j++] = c->str[c->i++];  // Copy $ literally
        return;
    }
    c->i++;  // Skip the $
    if (c->str[c->i] == '?')
    {
        expand_exit_status(c->result, &c->j, c->exit_status);
        c->i++;
        return;
    }
    expand_var_name(c);
}
```

**Purpose**: Handle `$` - dispatch to appropriate expansion.

**Cases**:
- In single quotes → keep literal `$`
- `$?` → expand to exit status
- `$VARNAME` → expand to variable value

---

### expand_arg()
```c
void expand_arg(char **arg, t_env *env, int exit_status)
{
    char *expanded;
    char *unquoted;

    expanded = expand_variables(*arg, env, exit_status);
    unquoted = remove_quotes(expanded);
    free(*arg);
    free(expanded);
    *arg = unquoted;
}
```

**Purpose**: Full processing of one argument.

**Steps**:
1. Expand variables
2. Remove quotes
3. Replace original string

---

### expand_cmd_args()
```c
void expand_cmd_args(t_cmd *cmd, t_env *env, int exit_status)
{
    int i;

    i = 0;
    while (cmd->args && cmd->args[i])
    {
        expand_arg(&cmd->args[i], env, exit_status);
        i++;
    }
    cmd->args = compact_args(cmd->args);
}
```

**Purpose**: Expand all arguments of a command.

**compact_args**: Removes empty strings (from undefined variables).

---

## expander_quotes.c

### remove_quotes()
```c
char *remove_quotes(char *s)
{
    t_quote_ctx c;
    char        *res;

    if (!s)
        return (NULL);
    c.i = 0;
    c.j = 0;
    c.quote = 0;
    c.str = s;
    res = malloc(ft_strlen(s) + 1);
    if (!res)
        return (NULL);
    c.res = res;
    while (s[c.i])
    {
        if (handle_quote(&c, s))
            continue;
        c.res[c.j++] = s[c.i++];
    }
    c.res[c.j] = '\0';
    return (res);
}
```

**Purpose**: Remove quotes from string.

**Logic**:
- Track quote state
- When encountering quote that matches current state, skip it
- Copy all other characters

---

## Expansion Examples

### Variable Expansion
```
Input:   echo $HOME
Step 1:  $HOME → /Users/john
Step 2:  No quotes to remove
Output:  echo /Users/john
```

### Single vs Double Quotes
```
Input:   echo "$HOME" '$HOME'
Step 1:  "$HOME" → "/Users/john"  (expanded, quotes kept)
         '$HOME' → '$HOME'         (NOT expanded)
Step 2:  "/Users/john" → /Users/john  (quotes removed)
         '$HOME' → $HOME              (quotes removed)
Output:  echo /Users/john $HOME
```

### Exit Status
```
Input:   echo $?
Step 1:  $? → 0 (or whatever last exit code was)
Output:  echo 0
```

### Empty Variable
```
Input:   echo $UNDEFINED hello
Step 1:  $UNDEFINED → "" (empty)
Step 2:  compact_args removes empty string
Output:  echo hello
```

### Mixed Quoting
```
Input:   echo "Hello $USER, path is "'$PATH'
Step 1:  "Hello $USER, path is " → "Hello john, path is "
         '$PATH' → '$PATH' (not expanded)
Step 2:  Remove quotes
Output:  echo Hello john, path is $PATH
```

---

## Expansion Flow Diagram

```
     t_cmd with raw args
          │
          ▼
    ┌─────────────────────────────────────────┐
    │         expand_cmd_args()                │
    │                                          │
    │   For each arg:                          │
    │   ┌───────────────────────────────────┐ │
    │   │         expand_arg()               │ │
    │   │                                    │ │
    │   │   ┌─────────────────────────┐     │ │
    │   │   │   expand_variables()     │     │ │
    │   │   │                          │     │ │
    │   │   │   For each char:         │     │ │
    │   │   │   - Quote? toggle state  │     │ │
    │   │   │   - $? expand exit       │     │ │
    │   │   │   - $VAR? expand var     │     │ │
    │   │   │   - else copy            │     │ │
    │   │   └──────────┬──────────────┘     │ │
    │   │              │                     │ │
    │   │   ┌──────────▼──────────────┐     │ │
    │   │   │    remove_quotes()       │     │ │
    │   │   └─────────────────────────┘     │ │
    │   └───────────────────────────────────┘ │
    │                                          │
    │   ┌───────────────────────────────────┐ │
    │   │       compact_args()               │ │
    │   │   (remove empty strings)           │ │
    │   └───────────────────────────────────┘ │
    └─────────────────────────────────────────┘
          │
          ▼
    t_cmd with expanded args
```

---

## Key Concepts

### Quote Rules Summary
| Context | `$VAR` behavior |
|---------|-----------------|
| No quotes | Expanded |
| Double quotes `"$VAR"` | Expanded |
| Single quotes `'$VAR'` | NOT expanded (literal) |

### Why Two Passes?
1. **Expand variables first**: Preserves quote context for expansion decisions
2. **Remove quotes after**: Quotes served their purpose, now remove them

### Edge Cases
- `$` at end of string → literal `$`
- `$$` → process ID (not implemented, kept literal)
- `$1`, `$2` → positional params (not implemented, kept literal)
- `$?` → always expands to exit status

### Empty Variable Handling
When `$UNDEFINED` expands to empty:
```
echo $UNDEFINED hello  →  echo hello  (not echo "" hello)
```
The `compact_args()` function removes empty strings to match bash behavior.
