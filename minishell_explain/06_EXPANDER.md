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

**Real-life analogy**: The expander is like a **translator with a dictionary**:

Imagine you receive a message with abbreviations:
```
"Hello MR_SMITH, your apt is at ADDR"
```

You look up in your notebook:
- MR_SMITH → John Smith
- ADDR → 123 Main Street

Result:
```
"Hello John Smith, your apt is at 123 Main Street"
```

The shell expander does the same with environment variables!

---

## Expander in Action

### Example 1: Variable Expansion
```
Before: echo $HOME
        ↓
Dictionary lookup: HOME = /Users/mansoor
        ↓
After:  echo /Users/mansoor
```

### Example 2: Exit Status
```
Previous command returned: 0

Before: echo $?
        ↓
Lookup: ? = last exit status = 0
        ↓
After:  echo 0
```

### Example 3: Quote Handling
```
Before: echo "$HOME" '$HOME'
             ↓          ↓
           expand    don't expand
           (double)   (single)
        ↓
After:  echo "/Users/mansoor" '$HOME'
        ↓
Remove quotes:
After:  echo /Users/mansoor $HOME
```

---

## The Quote Rules - Most Important!

```
┌─────────────────────────────────────────────────────────────────┐
│                     QUOTE BEHAVIOR TABLE                         │
├────────────────┬────────────────┬────────────────────────────────┤
│ Quote Type     │ Variables      │ Example                        │
├────────────────┼────────────────┼────────────────────────────────┤
│ No quotes      │ EXPANDED       │ echo $HOME → /Users/mansoor   │
│                │                │                                │
├────────────────┼────────────────┼────────────────────────────────┤
│ Double quotes  │ EXPANDED       │ echo "$HOME" → /Users/mansoor │
│ "..."          │                │ (but preserves as one word)   │
│                │                │                                │
├────────────────┼────────────────┼────────────────────────────────┤
│ Single quotes  │ NOT EXPANDED   │ echo '$HOME' → $HOME          │
│ '...'          │ (literal)      │ ($ is just a character)       │
│                │                │                                │
└────────────────┴────────────────┴────────────────────────────────┘
```

### Why the Difference?

**Double quotes** = "I want spaces preserved, but expand variables"
```bash
FILE="my file.txt"
cat "$FILE"          # Works! Treated as ONE argument
cat $FILE            # Broken! Treated as TWO arguments: "my" and "file.txt"
```

**Single quotes** = "I want EXACTLY these characters, nothing special"
```bash
echo '$HOME'         # Prints: $HOME (literally)
echo 'use * for wildcards'   # * is literal, not expanded
```

---

## The Expansion Context

```c
typedef struct s_exp_ctx
{
    char    *str;        // Original string being processed
    char    *result;     // Output buffer (expanded string)
    int     i;           // Current position in str
    int     j;           // Current position in result
    char    in_quote;    // 0, '\'', or '"'
    t_env   *env;        // Environment variables
    int     exit_status; // Last command exit code
}   t_exp_ctx;
```

**Visual for expanding `"Hello $USER"`**:
```
str:    " H e l l o   $ U S E R "
         ↑
         i (moving through input)

result: H e l l o   m a n s o o r
                                ↑
                                j (building output)

in_quote: '"' (we're inside double quotes)
```

---

## Step-by-Step Expansion Examples

### Example 1: Simple Variable

**Input**: `$HOME`
**Environment**: HOME=/Users/mansoor

```
┌──────────────────────────────────────────────────────┐
│ Step 1: See '$'                                       │
│         in_quote = 0 (not in quotes)                 │
│         Should we expand? YES                        │
│                                                       │
│ Step 2: Skip the '$'                                 │
│         i moves from 0 to 1                          │
│                                                       │
│ Step 3: Read variable name                           │
│         Read: H, O, M, E (all valid name chars)      │
│         Stop at end of string                        │
│         key = "HOME"                                 │
│                                                       │
│ Step 4: Look up in environment                       │
│         get_env_value(env, "HOME")                   │
│         Returns: "/Users/mansoor"                    │
│                                                       │
│ Step 5: Copy value to result                         │
│         result = "/Users/mansoor"                    │
└──────────────────────────────────────────────────────┘

Result: "/Users/mansoor"
```

### Example 2: Variable in Double Quotes

**Input**: `"Hello $USER"`
**Environment**: USER=mansoor

```
┌──────────────────────────────────────────────────────┐
│ Position 0: '"'                                       │
│   in_quote becomes '"'                               │
│   Copy to result: "                                  │
│                                                       │
│ Position 1-6: 'H' 'e' 'l' 'l' 'o' ' '               │
│   Normal characters, just copy                       │
│   result: "Hello                                     │
│                                                       │
│ Position 7: '$'                                       │
│   in_quote == '"' (double quote)                     │
│   Should expand? YES (double quotes allow expansion) │
│                                                       │
│ Position 8-11: 'U' 'S' 'E' 'R'                       │
│   Read variable name: USER                           │
│   Look up: mansoor                                   │
│   Copy value to result                               │
│   result: "Hello mansoor                             │
│                                                       │
│ Position 12: '"'                                      │
│   Matches in_quote                                   │
│   in_quote becomes 0                                 │
│   Copy to result: "                                  │
│   result: "Hello mansoor"                            │
└──────────────────────────────────────────────────────┘

After expansion:  "Hello mansoor"
After remove_quotes: Hello mansoor
```

### Example 3: Variable in Single Quotes

**Input**: `'$HOME'`
**Environment**: HOME=/Users/mansoor

```
┌──────────────────────────────────────────────────────┐
│ Position 0: '\''                                      │
│   in_quote becomes '\''                              │
│   Copy to result: '                                  │
│                                                       │
│ Position 1: '$'                                       │
│   in_quote == '\'' (single quote)                    │
│   Should expand? NO! Single quotes = literal         │
│   Just copy the $                                    │
│   result: '$                                         │
│                                                       │
│ Position 2-5: 'H' 'O' 'M' 'E'                        │
│   Normal characters (we're in single quotes)         │
│   Just copy them                                     │
│   result: '$HOME                                     │
│                                                       │
│ Position 6: '\''                                      │
│   Matches in_quote                                   │
│   in_quote becomes 0                                 │
│   Copy to result: '                                  │
│   result: '$HOME'                                    │
└──────────────────────────────────────────────────────┘

After expansion:  '$HOME'
After remove_quotes: $HOME   ← Literal string!
```

### Example 4: Exit Status

**Input**: `$?`
**Last exit status**: 127

```
┌──────────────────────────────────────────────────────┐
│ Position 0: '$'                                       │
│   Should expand? YES                                 │
│   Skip it, move to next char                         │
│                                                       │
│ Position 1: '?'                                       │
│   Special case! $? means exit status                 │
│   Call expand_exit_status()                          │
│   Convert 127 to string "127"                        │
│   Copy to result                                     │
│   result: 127                                        │
└──────────────────────────────────────────────────────┘

Result: "127"
```

### Example 5: Undefined Variable

**Input**: `$UNDEFINED`
**Environment**: UNDEFINED is not set

```
┌──────────────────────────────────────────────────────┐
│ Position 0: '$'                                       │
│   Should expand? YES                                 │
│                                                       │
│ Position 1-9: 'U' 'N' 'D' 'E' 'F' 'I' 'N' 'E' 'D'   │
│   Read variable name: UNDEFINED                      │
│                                                       │
│ Look up in environment:                              │
│   get_env_value(env, "UNDEFINED")                    │
│   Returns: NULL (not found!)                         │
│                                                       │
│ Since NULL, copy nothing to result                   │
│   result: "" (empty)                                 │
└──────────────────────────────────────────────────────┘

Result: "" (empty string)
```

---

## Key Functions Explained

### expand_variables() - Main Function

```c
char *expand_variables(char *str, t_env *env, int exit_status)
{
    t_exp_ctx c;

    init_ctx(&c, str, env, exit_status);

    while (str[c.i])  // Process each character
    {
        if (is_quote_char(&c))
            handle_quote(&c);           // Track quote state
        else if (should_expand(&c))
            process_dollar(&c);         // Expand $VAR
        else
            c.result[c.j++] = c.str[c.i++];  // Just copy
    }

    c.result[c.j] = '\0';
    return (c.result);
}
```

### should_expand() - When to Expand

```c
static int should_expand(t_exp_ctx *c)
{
    // Expand $ only if NOT in single quotes
    return (c->str[c->i] == '$' && c->in_quote != '\'');
}
```

**Logic**:
- See `$`? Check quote state
- In single quotes (`'`)? Don't expand
- Otherwise? Expand it!

### process_dollar() - Handle $ Character

```c
void process_dollar(t_exp_ctx *c)
{
    // In single quotes? Just copy the $
    if (c->in_quote == '\'')
    {
        c->result[c->j++] = c->str[c->i++];
        return;
    }

    c->i++;  // Skip the $

    // Is it $? (exit status)?
    if (c->str[c->i] == '?')
    {
        expand_exit_status(c->result, &c->j, c->exit_status);
        c->i++;
        return;
    }

    // Otherwise, it's a variable name
    expand_var_name(c);
}
```

### Variable Name Rules

```c
// Valid variable name characters:
// - First char: letter or underscore
// - Rest: letter, digit, or underscore

// Examples:
// HOME      → valid
// _private  → valid
// var123    → valid
// 123var    → INVALID (starts with digit)
// var-name  → INVALID (contains hyphen)
```

---

## Quote Removal

After expansion, quotes are removed:

### remove_quotes() - Strip Quotes

```c
char *remove_quotes(char *s)
{
    char *result;
    int i = 0, j = 0;
    char quote = 0;

    while (s[i])
    {
        if (!quote && (s[i] == '\'' || s[i] == '"'))
        {
            quote = s[i];  // Enter quote
            i++;           // Skip the opening quote
            continue;
        }
        if (quote && s[i] == quote)
        {
            quote = 0;     // Exit quote
            i++;           // Skip the closing quote
            continue;
        }
        result[j++] = s[i++];  // Copy character
    }
    result[j] = '\0';
    return (result);
}
```

**Example**:
```
Input:  "hello" 'world'
        ↓
Process:
  " → enter double quote, skip
  h e l l o → copy
  " → exit double quote, skip
    → copy space
  ' → enter single quote, skip
  w o r l d → copy
  ' → exit single quote, skip
        ↓
Output: hello world
```

---

## Complete Expansion Flow

```
            Original argument
                   │
                   ▼
    ┌──────────────────────────────┐
    │      expand_variables()       │
    │                               │
    │   ┌────────────────────────┐ │
    │   │ For each character:    │ │
    │   │                        │ │
    │   │  Quote? → track state  │ │
    │   │  $? → expand variable  │ │
    │   │  else → copy           │ │
    │   └────────────────────────┘ │
    └──────────────┬───────────────┘
                   │
                   ▼
              Expanded string
           (still has quotes)
                   │
                   ▼
    ┌──────────────────────────────┐
    │       remove_quotes()         │
    │                               │
    │   Remove " and ' characters  │
    │   (respecting quote pairing) │
    └──────────────┬───────────────┘
                   │
                   ▼
             Final string
          (ready for executor)
```

---

## Practical Examples

### Example: Building a Path

```bash
DIR="/home"
FILE="config.txt"
cat "$DIR/$FILE"
```

**Expansion process**:
```
"$DIR/$FILE"
    ↓
Step 1: Expand $DIR → /home
        "/home/$FILE"
    ↓
Step 2: Expand $FILE → config.txt
        "/home/config.txt"
    ↓
Step 3: Remove quotes
        /home/config.txt
```

### Example: Preserving Spaces

```bash
NAME="John Doe"
echo "Hello, $NAME"
```

**Why quotes matter**:
```
Without quotes:
  echo Hello, $NAME
  → echo Hello, John Doe
  → 3 arguments: "Hello,", "John", "Doe"

With quotes:
  echo "Hello, $NAME"
  → echo "Hello, John Doe"
  → 1 argument: "Hello, John Doe"
```

### Example: Literal Dollar Sign

```bash
echo 'Price: $5.99'
echo "Price: \$5.99"  # (we don't handle \, but concept is same)
```

**In minishell**:
```
echo 'Price: $5.99'
    ↓
Single quotes: no expansion
    ↓
Output: Price: $5.99
```

---

## Edge Cases

### Empty Variable
```bash
unset EMPTY
echo "[$EMPTY]"
# Output: []
# The variable expands to nothing
```

### Dollar at End
```bash
echo "Cost is $"
# The $ has nothing after it
# Output: Cost is $
# Lone $ at end stays as $
```

### Adjacent Variables
```bash
A=Hello
B=World
echo "$A$B"
# Output: HelloWorld
```

### Variable in Redirection Filename
```bash
FILE="output.txt"
echo hello > $FILE
# Expander runs on "output.txt"
# Then executor opens output.txt
```

---

## Common Questions

### Q: Why not expand in single quotes?
**A**: Single quotes mean "take everything literally". This is useful for:
- Printing special characters: `echo '$HOME'`
- Avoiding accidents: `echo 'don't delete *'`

### Q: What if a variable contains quotes?
**A**: They're just characters in the value!
```bash
VAR='hello "world"'
echo $VAR
# Output: hello "world"
# The quotes in the value are literal characters
```

### Q: Why expand before removing quotes?
**A**: We need the quotes to know what to expand:
```
"$HOME"  → Expand $HOME, then remove quotes
'$HOME'  → Don't expand, then remove quotes
```
If we removed quotes first, we'd lose this information!

### Q: What about $$ (process ID)?
**A**: Not implemented in mandatory minishell. It stays as literal `$$`.
