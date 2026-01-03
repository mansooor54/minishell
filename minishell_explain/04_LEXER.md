# Lexer (Tokenizer)

## Files
- `src/lexer/lexer.c`
- `src/lexer/lexer_operator.c`
- `src/lexer/lexer_operator_type.c`
- `src/lexer/lexer_utils.c`
- `src/lexer/lexer_unclose.c`

---

## What is a Lexer?

**Real-life analogy**: The lexer is like a **language translator's first step**:

Imagine you receive a message in a foreign language:
```
"Bonjour comment allez vous"
```

Before understanding the meaning, you first identify individual words:
```
["Bonjour"] ["comment"] ["allez"] ["vous"]
```

That's what the lexer does with shell commands!

---

## Lexer in Action

### Example 1: Simple Command
```
Input:   "ls -la"
         ↓ ↓↓↓↓
         │ │└┴┴── word "-la"
         │ └────── space (delimiter)
         └──────── word "ls"

Output:  [WORD:"ls"] → [WORD:"-la"] → NULL
```

### Example 2: Command with Pipe
```
Input:   "cat file.txt | grep error"

Step by step:
┌──────────────────────────────────────────────────────┐
│ Position 0-2: "cat"        → [WORD:"cat"]           │
│ Position 3:   " "          → skip (whitespace)      │
│ Position 4-11: "file.txt"  → [WORD:"file.txt"]      │
│ Position 12:  " "          → skip                   │
│ Position 13:  "|"          → [PIPE:"|"]             │
│ Position 14:  " "          → skip                   │
│ Position 15-18: "grep"     → [WORD:"grep"]          │
│ Position 19:  " "          → skip                   │
│ Position 20-24: "error"    → [WORD:"error"]         │
└──────────────────────────────────────────────────────┘

Output:  [WORD:"cat"] → [WORD:"file.txt"] → [PIPE:"|"] →
         [WORD:"grep"] → [WORD:"error"] → NULL
```

### Example 3: Redirections
```
Input:   "echo hello > output.txt"

Output:  [WORD:"echo"] → [WORD:"hello"] → [REDIR_OUT:">"] →
         [WORD:"output.txt"] → NULL
```

### Example 4: Heredoc
```
Input:   "cat << EOF"

Output:  [WORD:"cat"] → [REDIR_HEREDOC:"<<"] → [WORD:"EOF"] → NULL
```

---

## Token Types - Complete List

```c
typedef enum e_token_type
{
    TOKEN_WORD,          // Any regular word/argument
    TOKEN_PIPE,          // |
    TOKEN_REDIR_IN,      // <
    TOKEN_REDIR_OUT,     // >
    TOKEN_REDIR_APPEND,  // >>
    TOKEN_REDIR_HEREDOC, // <<
    TOKEN_EOF            // End marker
}   t_token_type;
```

### Visual Guide

```
┌────────────────────────────────────────────────────────────────┐
│                        TOKEN TYPES                              │
├────────────────┬───────────┬───────────────────────────────────┤
│ TOKEN_WORD     │ "ls"      │ Commands, arguments, filenames    │
│                │ "-la"     │                                   │
│                │ "hello"   │                                   │
├────────────────┼───────────┼───────────────────────────────────┤
│ TOKEN_PIPE     │   |       │ Connects command output to input  │
├────────────────┼───────────┼───────────────────────────────────┤
│ TOKEN_REDIR_IN │   <       │ Read input from file              │
├────────────────┼───────────┼───────────────────────────────────┤
│ TOKEN_REDIR_OUT│   >       │ Write output to file (overwrite)  │
├────────────────┼───────────┼───────────────────────────────────┤
│TOKEN_REDIR_    │   >>      │ Write output to file (append)     │
│ APPEND         │           │                                   │
├────────────────┼───────────┼───────────────────────────────────┤
│TOKEN_REDIR_    │   <<      │ Here-document (inline input)      │
│ HEREDOC        │           │                                   │
├────────────────┼───────────┼───────────────────────────────────┤
│ TOKEN_EOF      │   NULL    │ End of token list                 │
└────────────────┴───────────┴───────────────────────────────────┘
```

---

## The Token Structure

```c
typedef struct s_token
{
    t_token_type    type;   // What kind of token
    char            *value; // The actual text
    struct s_token  *next;  // Pointer to next token
}   t_token;
```

**Visual representation**:
```
For input: "ls | grep test"

┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ type: WORD      │    │ type: PIPE      │    │ type: WORD      │    │ type: WORD      │
│ value: "ls"     │───►│ value: "|"      │───►│ value: "grep"   │───►│ value: "test"   │───► NULL
│ next: ──────────┼─┐  │ next: ──────────┼─┐  │ next: ──────────┼─┐  │ next: NULL      │
└─────────────────┘ │  └─────────────────┘ │  └─────────────────┘ │  └─────────────────┘
                    └──────────────────────┴──────────────────────┘
```

---

## lexer.c - Main Functions

### lexer() - The Entry Point

```c
t_token *lexer(char *input)
{
    t_token *tokens;     // Our result - linked list of tokens
    t_token *new_token;  // Each new token we create

    tokens = NULL;       // Start with empty list
    while (*input)       // While there are more characters
    {
        new_token = next_token(&input);  // Get next token
        if (!new_token)
            break;
        add_token(&tokens, new_token);   // Add to list
    }
    return (tokens);
}
```

**Step-by-step for "ls -la"**:
```
Iteration 1:
  input = "ls -la"
  next_token() returns [WORD:"ls"]
  input now = " -la"
  tokens = [WORD:"ls"]

Iteration 2:
  input = " -la"  (skip space in next_token)
  next_token() returns [WORD:"-la"]
  input now = ""
  tokens = [WORD:"ls"] → [WORD:"-la"]

Iteration 3:
  input = ""
  *input is '\0' (end)
  Loop ends

Return: [WORD:"ls"] → [WORD:"-la"] → NULL
```

---

### next_token() - Getting One Token

```c
static t_token *next_token(char **input)
{
    t_token *new_token;
    char    *word;
    int     len;

    // Step 1: Skip whitespace
    while (is_whitespace(**input))
        (*input)++;

    // Step 2: Check if we're done
    if (!**input)
        return (NULL);

    // Step 3: Is it an operator?
    if (is_operator(**input))
        new_token = get_operator_token(input);
    else
    {
        // Step 4: It's a regular word
        len = extract_word(*input, &word);
        new_token = create_token(TOKEN_WORD, word);
        free(word);
        *input += len;
    }
    return (new_token);
}
```

**Example walkthrough for "|"**:
```
Input:  " | grep"
        ^
        *input points here

Step 1: Skip space
        " | grep"
          ^
        *input points here now

Step 2: **input == '|', not '\0', continue

Step 3: is_operator('|') == TRUE
        Call get_operator_token()
        Returns [PIPE:"|"]
        *input now points past the |

Return: [PIPE:"|"]
```

---

## Operator Recognition

### try_or_pipe() - Recognizing |

```c
t_token *try_or_pipe(char **input)
{
    if (**input != '|')
        return (NULL);
    (*input)++;  // Move past the |
    return (create_token(TOKEN_PIPE, "|"));
}
```

**Simple!** If we see `|`, create a PIPE token.

### try_inredir() - Recognizing < and <<

```c
t_token *try_inredir(char **input)
{
    // Check for << FIRST (heredoc)
    if (**input == '<' && *(*input + 1) == '<')
    {
        *input += 2;  // Skip both characters
        return (create_token(TOKEN_REDIR_HEREDOC, "<<"));
    }
    // Then check for single <
    if (**input == '<')
    {
        (*input)++;
        return (create_token(TOKEN_REDIR_IN, "<"));
    }
    return (NULL);
}
```

**Why check << before <?**
```
If we checked < first for input "<<EOF":

Wrong way:
  "<<EOF"
   ^
   See '<', return [REDIR_IN:"<"]
   Now input is "<EOF" - WRONG!

Right way:
  "<<EOF"
   ^^
   See '<<', return [REDIR_HEREDOC:"<<"]
   Now input is "EOF" - CORRECT!
```

### try_outredir() - Recognizing > and >>

```c
t_token *try_outredir(char **input)
{
    // Check for >> FIRST (append)
    if (**input == '>' && *(*input + 1) == '>')
    {
        *input += 2;
        return (create_token(TOKEN_REDIR_APPEND, ">>"));
    }
    // Then check for single >
    if (**input == '>')
    {
        (*input)++;
        return (create_token(TOKEN_REDIR_OUT, ">"));
    }
    return (NULL);
}
```

Same logic as `<` / `<<`.

---

## Quote Handling - The Tricky Part

### The Challenge

Quotes change how we interpret characters:

```
"hello world"    ← This is ONE word, not two!
echo ">"         ← The > is NOT a redirection, it's text!
echo '$HOME'     ← The $HOME is literal, not expanded!
```

### measure_word() - Counting Characters

```c
static int measure_word(char *s)
{
    int i = 0;
    int in_quote = 0;

    while (is_word_cont(s, i, in_quote))
    {
        // Entering a quote
        if (!in_quote && (s[i] == '\'' || s[i] == '"'))
        {
            in_quote = s[i];  // Remember which quote
            i++;
            continue;
        }
        // Exiting a quote
        if (in_quote && s[i] == in_quote)
        {
            in_quote = 0;     // No longer in quote
            i++;
            continue;
        }
        i++;
    }
    return (i);
}
```

**Example: "hello world"**
```
Position 0: '"'  → Enter quote mode (in_quote = '"')
Position 1: 'h'  → In quote, keep going
Position 2: 'e'  → In quote, keep going
Position 3: 'l'  → In quote, keep going
Position 4: 'l'  → In quote, keep going
Position 5: 'o'  → In quote, keep going
Position 6: ' '  → In quote, space is NOT a delimiter!
Position 7: 'w'  → In quote, keep going
Position 8: 'o'  → In quote, keep going
Position 9: 'r'  → In quote, keep going
Position 10:'l'  → In quote, keep going
Position 11:'d'  → In quote, keep going
Position 12:'"'  → Exit quote mode (in_quote = 0)
Position 13:'\0' → End of string

Return: 13 (the whole thing is ONE token!)
```

**Example: hello world (no quotes)**
```
Position 0: 'h'  → Not in quote, keep going
Position 1: 'e'  → Not in quote, keep going
Position 2: 'l'  → Not in quote, keep going
Position 3: 'l'  → Not in quote, keep going
Position 4: 'o'  → Not in quote, keep going
Position 5: ' '  → Not in quote, space IS a delimiter!
           STOP HERE!

Return: 5 (only "hello" is this token)
```

---

## Complete Lexer Examples

### Example 1: Complex Command

```
Input: cat < input.txt | grep "error" >> log.txt

Lexer output:
┌─────────────────┐
│ WORD: "cat"     │
└────────┬────────┘
         ▼
┌─────────────────┐
│ REDIR_IN: "<"   │
└────────┬────────┘
         ▼
┌─────────────────┐
│ WORD: "input.txt"│
└────────┬────────┘
         ▼
┌─────────────────┐
│ PIPE: "|"       │
└────────┬────────┘
         ▼
┌─────────────────┐
│ WORD: "grep"    │
└────────┬────────┘
         ▼
┌─────────────────┐
│ WORD: "\"error\"" │  ← Quotes are KEPT in value!
└────────┬────────┘
         ▼
┌─────────────────┐
│ REDIR_APPEND: ">>"|
└────────┬────────┘
         ▼
┌─────────────────┐
│ WORD: "log.txt" │
└────────┬────────┘
         ▼
        NULL
```

### Example 2: Heredoc

```
Input: cat << EOF

Lexer output:
[WORD:"cat"] → [REDIR_HEREDOC:"<<"] → [WORD:"EOF"] → NULL
```

### Example 3: Mixed Quotes

```
Input: echo "hello" 'world' mix"ed"quotes

Lexer output:
[WORD:"echo"] → [WORD:"\"hello\""] → [WORD:"'world'"] →
[WORD:"mix\"ed\"quotes"] → NULL

Note: Quotes stay attached to words! The expander handles them later.
```

---

## Lexer Flow Diagram

```
                Input String: "ls -la | grep foo"
                              │
                              ▼
         ┌─────────────────────────────────────────────┐
         │                  lexer()                     │
         │                                              │
         │   tokens = NULL                              │
         │                                              │
         │   Loop while *input:                         │
         │   ┌───────────────────────────────────────┐ │
         │   │           next_token()                 │ │
         │   │                                        │ │
         │   │   ┌─────────────────────────┐         │ │
         │   │   │ Skip whitespace         │         │ │
         │   │   │ "ls -la | grep foo"     │         │ │
         │   │   │  ^^                     │         │ │
         │   │   └───────────┬─────────────┘         │ │
         │   │               ▼                        │ │
         │   │   ┌─────────────────────────┐         │ │
         │   │   │ Is operator? 'l' → NO   │         │ │
         │   │   └───────────┬─────────────┘         │ │
         │   │               ▼                        │ │
         │   │   ┌─────────────────────────┐         │ │
         │   │   │ extract_word("ls")      │         │ │
         │   │   │ measure = 2             │         │ │
         │   │   │ create [WORD:"ls"]      │         │ │
         │   │   └─────────────────────────┘         │ │
         │   └───────────────────────────────────────┘ │
         │                                              │
         │   add_token(&tokens, new_token)              │
         │   Repeat for next character...               │
         │                                              │
         └──────────────────────────────────────────────┘
                              │
                              ▼
         [WORD:"ls"] → [WORD:"-la"] → [PIPE:"|"] →
         [WORD:"grep"] → [WORD:"foo"] → NULL
```

---

## Key Concepts

### 1. Why Keep Quotes in Token Values?

The lexer's job is ONLY to split input into tokens. It doesn't interpret meaning.

```
Input: echo "$HOME"

Lexer sees:  echo  "$HOME"
                   ^^^^^^^^
                   All one word

Token value: "\"$HOME\""  (quotes included)

Later, expander will:
1. See the quotes
2. Expand $HOME because it's in double quotes
3. Remove the quotes

This separation keeps code simple and maintainable!
```

### 2. Whitespace as Delimiter

```
"ls-la"     →  [WORD:"ls-la"]      (1 token, no space)
"ls -la"    →  [WORD:"ls"] [WORD:"-la"]  (2 tokens, space splits)
"ls  -la"   →  [WORD:"ls"] [WORD:"-la"]  (still 2 tokens, extra spaces ignored)
```

### 3. Operators Break Words

```
"echo>file"  →  [WORD:"echo"] [REDIR_OUT:">"] [WORD:"file"]
                No space needed! > is an operator.

"echo>>file" →  [WORD:"echo"] [REDIR_APPEND:">>"] [WORD:"file"]
```

---

## Common Questions

### Q: What if quotes are unbalanced?

**A**: The lexer still works! It reads until end of string:
```
Input: echo "hello

Lexer produces: [WORD:"echo"] [WORD:"\"hello"]

The shell_loop detects unclosed quotes and asks for more input ("> " prompt).
```

### Q: What happens with empty input?

**A**:
```
Input: ""
Lexer returns: NULL (empty token list)
```

### Q: How are escape characters handled?

**A**: The lexer doesn't handle escapes specially. It just reads characters.
Inside quotes, everything (including \) is read literally.

```
Input: echo "hello\nworld"
Lexer: [WORD:"echo"] [WORD:"\"hello\\nworld\""]

The \n is two characters: \ and n
Whether it becomes a newline depends on the command!
```
