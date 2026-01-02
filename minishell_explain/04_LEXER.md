# Lexer (Tokenizer)

## Files
- `src/lexer/lexer.c`
- `src/lexer/lexer_operator.c`
- `src/lexer/lexer_operator_type.c`
- `src/lexer/lexer_utils.c`
- `src/lexer/lexer_unclose.c`

---

## What is a Lexer?

The lexer (also called tokenizer or scanner) is the first stage of command processing. It converts a raw input string into a sequence of **tokens** - meaningful units that the parser can understand.

```
Input:  "ls -la | grep foo"

Output: [WORD:"ls"] → [WORD:"-la"] → [PIPE:"|"] → [WORD:"grep"] → [WORD:"foo"]
```

---

## Token Types

```c
typedef enum e_token_type
{
    TOKEN_WORD,          // Regular word/argument
    TOKEN_PIPE,          // |
    TOKEN_REDIR_IN,      // <
    TOKEN_REDIR_OUT,     // >
    TOKEN_REDIR_APPEND,  // >>
    TOKEN_REDIR_HEREDOC, // <<
    TOKEN_EOF            // End marker
}   t_token_type;
```

---

## lexer.c

### lexer()
```c
t_token *lexer(char *input)
{
    t_token *tokens;
    t_token *new_token;

    tokens = NULL;
    while (*input)
    {
        new_token = next_token(&input);
        if (!new_token)
            break;
        add_token(&tokens, new_token);
    }
    return (tokens);
}
```

**Purpose**: Main entry point - converts input string to token list.

**How it works**:
1. Start with empty token list
2. Repeatedly call `next_token()` to get next token
3. Add each token to the linked list
4. Stop when input is exhausted

---

### next_token()
```c
static t_token *next_token(char **input)
{
    t_token *new_token;
    char    *word;
    int     len;

    while (is_whitespace(**input))  // Skip spaces/tabs
        (*input)++;
    if (!**input)                   // End of input
        return (NULL);
    if (is_operator(**input))       // Check for operator
        new_token = get_operator_token(input);
    else                            // Regular word
    {
        len = extract_word(*input, &word);
        new_token = create_token(TOKEN_WORD, word);
        free(word);
        *input += len;
    }
    return (new_token);
}
```

**Purpose**: Extract one token from the current position.

**Logic**:
1. Skip whitespace
2. If at operator char (`|`, `<`, `>`), get operator token
3. Otherwise, extract a word token
4. Advance input pointer past the consumed characters

---

### create_token()
```c
t_token *create_token(t_token_type type, char *value)
{
    t_token *token;

    token = malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    token->type = type;
    if (value)
        token->value = ft_strdup(value);
    else
        token->value = NULL;
    token->next = NULL;
    return (token);
}
```

**Purpose**: Allocate and initialize a new token.

---

### add_token()
```c
void add_token(t_token **tokens, t_token *new_token)
{
    t_token *current;

    if (!*tokens)
    {
        *tokens = new_token;
        return;
    }
    current = *tokens;
    while (current->next)
        current = current->next;
    current->next = new_token;
}
```

**Purpose**: Append token to end of linked list.

---

### free_tokens()
```c
void free_tokens(t_token *tokens)
{
    t_token *tmp;

    while (tokens)
    {
        tmp = tokens;
        tokens = tokens->next;
        free(tmp->value);
        free(tmp);
    }
}
```

**Purpose**: Free entire token list and all values.

---

## lexer_operator_type.c

### try_or_pipe()
```c
t_token *try_or_pipe(char **input)
{
    if (**input != '|')
        return (NULL);
    (*input)++;
    return (create_token(TOKEN_PIPE, "|"));
}
```

**Purpose**: Recognize pipe operator `|`.

**Note**: The `||` (logical OR) was removed per 42 mandatory requirements.

---

### try_inredir()
```c
t_token *try_inredir(char **input)
{
    if (**input == '<' && *(*input + 1) == '<')
    {
        *input += 2;
        return (create_token(TOKEN_REDIR_HEREDOC, "<<"));
    }
    if (**input == '<')
    {
        (*input)++;
        return (create_token(TOKEN_REDIR_IN, "<"));
    }
    return (NULL);
}
```

**Purpose**: Recognize `<` (input redirection) or `<<` (heredoc).

**Order matters**: Check for `<<` before `<` to avoid matching `<` twice.

---

### try_outredir()
```c
t_token *try_outredir(char **input)
{
    if (**input == '>' && *(*input + 1) == '>')
    {
        *input += 2;
        return (create_token(TOKEN_REDIR_APPEND, ">>"));
    }
    if (**input == '>')
    {
        (*input)++;
        return (create_token(TOKEN_REDIR_OUT, ">"));
    }
    return (NULL);
}
```

**Purpose**: Recognize `>` (output) or `>>` (append).

---

### try_and()
```c
t_token *try_and(char **input)
{
    (void)input;
    return (NULL);
}
```

**Purpose**: Placeholder for `&&` (removed from mandatory).

---

## lexer_utils.c

### is_whitespace()
```c
int is_whitespace(char c)
{
    return (c == ' ' || c == '\t');
}
```

**Purpose**: Check if character is a delimiter.

---

### is_operator()
```c
int is_operator(char c)
{
    if (!c)
        return (0);
    if (c == '|')
        return (1);
    if (c == '>')
        return (1);
    if (c == '<')
        return (1);
    if (c == '&')
        return (1);
    return (0);
}
```

**Purpose**: Check if character starts an operator.

---

### extract_word()
```c
int extract_word(char *input, char **word)
{
    int len;

    len = measure_word(input);
    *word = malloc((size_t)len + 1);
    if (!*word)
        return (0);
    ft_strncpy(*word, input, len);
    (*word)[len] = '\0';
    return (len);
}
```

**Purpose**: Extract a word (non-operator, non-whitespace sequence).

---

### measure_word()
```c
static int measure_word(char *s)
{
    int i;
    int in_quote;

    i = 0;
    in_quote = 0;
    while (is_word_cont(s, i, in_quote))
    {
        if (!in_quote && (s[i] == '\'' || s[i] == '"'))
        {
            in_quote = s[i];
            i++;
            continue;
        }
        if (in_quote && s[i] == in_quote)
        {
            in_quote = 0;
            i++;
            continue;
        }
        i++;
    }
    return (i);
}
```

**Purpose**: Measure length of a word, respecting quotes.

**Quote handling**:
- When `"` or `'` is encountered outside quotes, enter quote mode
- In quote mode, spaces and operators are NOT word terminators
- When matching quote found, exit quote mode

**Example**:
```
"hello world"   → one word of length 13 (quotes included)
hello world     → "hello" is one word of length 5
```

---

## Lexer Examples

### Simple Command
```
Input:  "ls -la"
Tokens: [WORD:"ls"] → [WORD:"-la"]
```

### Command with Pipe
```
Input:  "cat file.txt | grep error"
Tokens: [WORD:"cat"] → [WORD:"file.txt"] → [PIPE:"|"] →
        [WORD:"grep"] → [WORD:"error"]
```

### Redirections
```
Input:  "echo hello > output.txt"
Tokens: [WORD:"echo"] → [WORD:"hello"] → [REDIR_OUT:">"] → [WORD:"output.txt"]
```

### Quoted Strings
```
Input:  echo "hello world"
Tokens: [WORD:"echo"] → [WORD:"\"hello world\""]

Note: Quotes are preserved in the token value.
      The expander/quote_removal step removes them later.
```

### Complex Example
```
Input:  cat << EOF | grep "pattern" >> out.txt

Tokens: [WORD:"cat"] → [REDIR_HEREDOC:"<<"] → [WORD:"EOF"] →
        [PIPE:"|"] → [WORD:"grep"] → [WORD:"\"pattern\""] →
        [REDIR_APPEND:">>"] → [WORD:"out.txt"]
```

---

## Token Flow Diagram

```
     Input String
          │
          ▼
    ┌─────────────┐
    │   lexer()   │
    └──────┬──────┘
           │
           ▼
    ┌─────────────────────────────────────────┐
    │              next_token()                │
    │                                          │
    │   ┌───────────────┐                     │
    │   │ Skip spaces   │                     │
    │   └───────┬───────┘                     │
    │           │                              │
    │           ▼                              │
    │   ┌───────────────┐                     │
    │   │ Operator?     │──yes──► get_operator_token()
    │   └───────┬───────┘                     │
    │           │ no                           │
    │           ▼                              │
    │   ┌───────────────┐                     │
    │   │ extract_word()│──► measure_word()   │
    │   └───────────────┘                     │
    └─────────────────────────────────────────┘
           │
           ▼
    Token Linked List
    [type, value, next] → [type, value, next] → NULL
```

---

## Key Concepts

### Why Quotes Stay in Token Values?
The lexer's job is just to split input into tokens. It doesn't interpret the tokens. Quote handling (removing quotes, deciding what gets expanded) is done by the **expander** later.

### Order of Operator Checking
When checking for `<<` vs `<`:
1. Must check `<<` FIRST
2. Otherwise `<<` would match as two `<` tokens

### Whitespace as Delimiter
Spaces/tabs separate tokens but are not tokens themselves:
- `ls-la` → one token `[WORD:"ls-la"]`
- `ls -la` → two tokens `[WORD:"ls"]` `[WORD:"-la"]`
