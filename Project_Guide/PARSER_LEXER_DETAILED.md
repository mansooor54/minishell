# Parser and Lexer - Comprehensive Documentation

This document provides detailed explanations for all functions in the parser and lexer modules of the minishell project.

---

## Table of Contents

1. [Overview](#overview)
2. [Lexer Module](#lexer-module)
   - [lexer.c](#lexerc)
     - [create_token](#create_token)
     - [add_token](#add_token)
     - [free_tokens](#free_tokens)
   - [lexer_utils.c](#lexer_utilsc)
     - [is_whitespace](#is_whitespace)
     - [is_operator](#is_operator)
     - [extract_word](#extract_word)
     - [get_operator_token](#get_operator_token)
     - [lexer](#lexer)
3. [Parser Module](#parser-module)
   - [parser.c](#parserc)
     - [create_redir](#create_redir)
     - [parse_redirections](#parse_redirections)
     - [count_args](#count_args)
     - [parse_command](#parse_command)
     - [free_pipeline](#free_pipeline)
   - [parser_pipeline.c](#parser_pipelinec)
     - [create_pipeline](#create_pipeline)
     - [parse_pipe_sequence](#parse_pipe_sequence)
     - [parser](#parser)

---

## Overview

The lexer and parser work together to transform user input into an executable data structure:

```
User Input (String)
      ↓
   LEXER
      ↓
Token Stream (Linked List)
      ↓
   PARSER
      ↓
Pipeline Structure (AST)
      ↓
   EXECUTOR
```

### Lexer (Lexical Analysis)

**Purpose:** Break input string into tokens (lexemes)

**Input:** `"ls -la | grep txt > output.txt"`

**Output:** Token list:
```
[WORD: "ls"] → [WORD: "-la"] → [PIPE: "|"] → [WORD: "grep"] → 
[WORD: "txt"] → [REDIR_OUT: ">"] → [WORD: "output.txt"]
```

### Parser (Syntax Analysis)

**Purpose:** Build Abstract Syntax Tree (AST) from tokens

**Input:** Token list (from lexer)

**Output:** Pipeline structure:
```
Pipeline {
    cmds: [
        Command { args: ["ls", "-la"], redirs: NULL },
        Command { args: ["grep", "txt"], redirs: [>output.txt] }
    ],
    logic_op: EOF
}
```

---

## Lexer Module

### Token Types

```c
typedef enum e_token_type {
    TOKEN_WORD,           // Command, argument, or filename
    TOKEN_PIPE,           // |
    TOKEN_REDIR_IN,       // <
    TOKEN_REDIR_OUT,      // >
    TOKEN_REDIR_APPEND,   // >>
    TOKEN_REDIR_HEREDOC,  // <<
    TOKEN_AND,            // && (bonus)
    TOKEN_OR,             // || (bonus)
    TOKEN_EOF             // End of input
} t_token_type;
```

### Token Structure

```c
typedef struct s_token {
    t_token_type    type;   // Type of token
    char            *value; // String value (for WORD tokens)
    struct s_token  *next;  // Next token in list
} t_token;
```

---

## lexer.c

### create_token

**Function Signature:**
```c
t_token *create_token(t_token_type type, char *value)
```

**Purpose:**  
Create and initialize a new token with the specified type and value.

**Parameters:**
- `type`: Token type (TOKEN_WORD, TOKEN_PIPE, etc.)
- `value`: String value to store in token (can be NULL for operators)

**Return Value:**
- Pointer to newly allocated token on success
- NULL if malloc fails

**Detailed Description:**

This function is the token factory. It allocates memory for a new token, initializes its fields, and returns a pointer to it.

**Implementation:**

```c
t_token *token;

token = malloc(sizeof(t_token));
if (!token)
    return (NULL);

token->type = type;

if (value)
    token->value = ft_strdup(value);  // Duplicate the string
else
    token->value = NULL;

token->next = NULL;  // Initialize as last token

return (token);
```

**Algorithm:**

```
1. Allocate memory for token structure
2. Check if malloc succeeded
3. Set token type
4. If value provided:
   - Duplicate value string
5. Else:
   - Set value to NULL
6. Initialize next pointer to NULL
7. Return token pointer
```

**Examples:**

**Example 1: Create WORD token**
```c
t_token *token = create_token(TOKEN_WORD, "ls");
// Result:
// token->type = TOKEN_WORD
// token->value = "ls" (allocated copy)
// token->next = NULL
```

**Example 2: Create PIPE token**
```c
t_token *token = create_token(TOKEN_PIPE, "|");
// Result:
// token->type = TOKEN_PIPE
// token->value = "|" (allocated copy)
// token->next = NULL
```

**Example 3: Create token with NULL value**
```c
t_token *token = create_token(TOKEN_PIPE, NULL);
// Result:
// token->type = TOKEN_PIPE
// token->value = NULL
// token->next = NULL
```

**Example 4: Malloc failure**
```c
// If malloc fails:
t_token *token = create_token(TOKEN_WORD, "test");
// Returns: NULL
```

**Memory Management:**

**Allocation:**
- Token structure: `sizeof(t_token)` bytes
- Value string: `strlen(value) + 1` bytes (if value is not NULL)

**Deallocation:**
- Caller must eventually call `free_tokens()` to free the entire token list
- Both token structure and value string must be freed

**Why Duplicate the Value?**

```c
if (value)
    token->value = ft_strdup(value);
```

We duplicate the value string because:
1. The original string might be temporary (stack-allocated)
2. The token needs to own its data
3. Prevents dangling pointers if original is freed

**Usage Pattern:**

```c
// Create token
t_token *token = create_token(TOKEN_WORD, "echo");

// Use token
printf("Type: %d, Value: %s\n", token->type, token->value);

// Eventually free (via free_tokens)
free(token->value);
free(token);
```

**Token Type vs Value:**

| Type | Value | Example |
|------|-------|---------|
| TOKEN_WORD | Command/arg | "ls", "-la", "file.txt" |
| TOKEN_PIPE | "|" | "|" |
| TOKEN_REDIR_IN | "<" | "<" |
| TOKEN_REDIR_OUT | ">" | ">" |
| TOKEN_REDIR_APPEND | ">>" | ">>" |
| TOKEN_REDIR_HEREDOC | "<<" | "<<" |
| TOKEN_AND | "&&" | "&&" |
| TOKEN_OR | "||" | "||" |

**Why Store Operator Strings?**

For operators like `|`, `>`, `>>`, we could just use the type. However, storing the string value:
- Makes debugging easier (can print token value)
- Keeps the interface consistent (all tokens have values)
- Allows for future extensions

---

### add_token

**Function Signature:**
```c
void add_token(t_token **tokens, t_token *new_token)
```

**Purpose:**  
Append a new token to the end of the token list.

**Parameters:**
- `tokens`: Pointer to pointer to the head of the token list
- `new_token`: Token to append to the list

**Return Value:**  
void (modifies the list in place)

**Detailed Description:**

This function implements the append operation for a singly-linked list. It handles two cases:
1. Empty list: Set new token as head
2. Non-empty list: Traverse to end and append

**Implementation:**

```c
t_token *current;

if (!*tokens) {
    // List is empty, new token becomes head
    *tokens = new_token;
    return;
}

// List is not empty, traverse to end
current = *tokens;
while (current->next)
    current = current->next;

// Append new token
current->next = new_token;
```

**Algorithm:**

```
1. Check if list is empty (*tokens == NULL):
   a. If YES:
      - Set *tokens = new_token
      - Return
   
2. If NO (list has elements):
   a. Start at head: current = *tokens
   b. Traverse to last node:
      - While current->next != NULL:
        - Move to next: current = current->next
   c. Append: current->next = new_token
```

**Examples:**

**Example 1: Add to empty list**
```c
t_token *tokens = NULL;
t_token *token1 = create_token(TOKEN_WORD, "ls");

add_token(&tokens, token1);

// Result:
// tokens → [WORD: "ls"] → NULL
```

**Example 2: Add to non-empty list**
```c
t_token *tokens = create_token(TOKEN_WORD, "ls");
t_token *token2 = create_token(TOKEN_WORD, "-la");

add_token(&tokens, token2);

// Result:
// tokens → [WORD: "ls"] → [WORD: "-la"] → NULL
```

**Example 3: Build complete token list**
```c
t_token *tokens = NULL;

add_token(&tokens, create_token(TOKEN_WORD, "echo"));
add_token(&tokens, create_token(TOKEN_WORD, "hello"));
add_token(&tokens, create_token(TOKEN_PIPE, "|"));
add_token(&tokens, create_token(TOKEN_WORD, "cat"));

// Result:
// tokens → [WORD: "echo"] → [WORD: "hello"] → 
//          [PIPE: "|"] → [WORD: "cat"] → NULL
```

**Time Complexity:**

- **Best case:** O(1) - Empty list
- **Worst case:** O(n) - Must traverse entire list
- **Average case:** O(n) - Linear traversal

**Why O(n)?**

Because we must traverse to the end of the list to append. For a list of n tokens, we visit n nodes.

**Optimization:**

To achieve O(1) append, we could maintain a tail pointer:

```c
typedef struct s_token_list {
    t_token *head;
    t_token *tail;
} t_token_list;

void add_token_fast(t_token_list *list, t_token *new_token) {
    if (!list->head) {
        list->head = new_token;
        list->tail = new_token;
    } else {
        list->tail->next = new_token;
        list->tail = new_token;
    }
}
```

However, the current implementation is simpler and sufficient for minishell (typical input has < 100 tokens).

**Why Pass `**tokens`?**

```c
void add_token(t_token **tokens, ...)
```

We need `**tokens` (pointer to pointer) because:
1. We might modify the head pointer (when list is empty)
2. We need to update the caller's pointer

Example:
```c
t_token *tokens = NULL;  // Empty list

add_token(&tokens, new_token);
// tokens now points to new_token
// If we only had *tokens, we couldn't update the pointer
```

**Invariants:**

After `add_token()`:
- List is never empty (at least contains new_token)
- new_token is at the end of the list
- new_token->next is NULL (unless it was already set)

---

### free_tokens

**Function Signature:**
```c
void free_tokens(t_token *tokens)
```

**Purpose:**  
Free all tokens in the list and their associated memory.

**Parameters:**
- `tokens`: Head of the token list to free

**Return Value:**  
void

**Detailed Description:**

This function performs a complete cleanup of a token list. It traverses the list, freeing both the value string and the token structure for each node.

**Implementation:**

```c
t_token *tmp;

while (tokens) {
    tmp = tokens;           // Save current token
    tokens = tokens->next;  // Move to next
    free(tmp->value);       // Free value string
    free(tmp);              // Free token structure
}
```

**Algorithm:**

```
1. While tokens != NULL:
   a. Save current token: tmp = tokens
   b. Move to next: tokens = tokens->next
   c. Free value string: free(tmp->value)
   d. Free token structure: free(tmp)
```

**Why This Order?**

```c
tmp = tokens;           // 1. Save pointer
tokens = tokens->next;  // 2. Move to next BEFORE freeing
free(tmp->value);       // 3. Free string
free(tmp);              // 4. Free structure
```

If we did this:
```c
free(tokens);           // ❌ Wrong!
tokens = tokens->next;  // ❌ Accessing freed memory!
```

We'd access freed memory (undefined behavior).

**Examples:**

**Example 1: Free single token**
```c
t_token *tokens = create_token(TOKEN_WORD, "ls");

free_tokens(tokens);
// All memory freed
```

**Example 2: Free multiple tokens**
```c
t_token *tokens = NULL;
add_token(&tokens, create_token(TOKEN_WORD, "echo"));
add_token(&tokens, create_token(TOKEN_WORD, "hello"));
add_token(&tokens, create_token(TOKEN_PIPE, "|"));
add_token(&tokens, create_token(TOKEN_WORD, "cat"));

free_tokens(tokens);
// All 4 tokens and their values freed
```

**Example 3: Free NULL list**
```c
t_token *tokens = NULL;

free_tokens(tokens);
// No-op, while loop doesn't execute
```

**Memory Freed:**

For each token:
1. **Value string:** `strlen(value) + 1` bytes (if value != NULL)
2. **Token structure:** `sizeof(t_token)` bytes

Total for n tokens: `n * sizeof(t_token) + sum of all value lengths`

**NULL Value Handling:**

```c
free(tmp->value);  // Safe even if value is NULL
```

`free(NULL)` is safe according to C standard - it's a no-op.

**Usage Pattern:**

```c
// Create and use tokens
t_token *tokens = lexer("echo hello | cat");

// ... use tokens ...

// Clean up
free_tokens(tokens);
tokens = NULL;  // Good practice: prevent dangling pointer
```

**Why Not Recursive?**

Alternative recursive implementation:
```c
void free_tokens_recursive(t_token *tokens) {
    if (!tokens)
        return;
    free_tokens_recursive(tokens->next);
    free(tokens->value);
    free(tokens);
}
```

Problems:
- Stack overflow for very long token lists
- Unnecessary overhead
- Iterative version is simpler and more efficient

**Memory Leak Prevention:**

Always call `free_tokens()` after using a token list:

```c
// ❌ Memory leak:
t_token *tokens = lexer(input);
// ... forgot to free ...

// ✅ Correct:
t_token *tokens = lexer(input);
// ... use tokens ...
free_tokens(tokens);
```

---

## lexer_utils.c

### is_whitespace

**Function Signature:**
```c
static int is_whitespace(char c)
```

**Purpose:**  
Check if a character is whitespace (space or tab).

**Parameters:**
- `c`: Character to check

**Return Value:**
- 1 if character is space or tab
- 0 otherwise

**Detailed Description:**

This is a simple helper function that identifies whitespace characters. In minishell, we only consider space and tab as whitespace (not newline, as each line is processed separately).

**Implementation:**

```c
return (c == ' ' || c == '\t');
```

**Examples:**

```c
is_whitespace(' ');   // Returns: 1
is_whitespace('\t');  // Returns: 1
is_whitespace('a');   // Returns: 0
is_whitespace('\n');  // Returns: 0
is_whitespace('\0');  // Returns: 0
```

**Why Not Use isspace()?**

Standard `isspace()` also includes:
- `\n` (newline)
- `\r` (carriage return)
- `\v` (vertical tab)
- `\f` (form feed)

We only want space and tab because:
- Newlines delimit commands (handled by readline)
- Other whitespace is not relevant in shell syntax

**Usage in Lexer:**

```c
// Skip whitespace
while (is_whitespace(*input))
    input++;
```

This skips all consecutive spaces and tabs.

**Why Static?**

```c
static int is_whitespace(char c)
```

- Only used within lexer_utils.c
- Not needed by other files
- Reduces namespace pollution
- Allows compiler optimizations

---

### is_operator

**Function Signature:**
```c
static int is_operator(char c)
```

**Purpose:**  
Check if a character is a shell operator character.

**Parameters:**
- `c`: Character to check

**Return Value:**
- 1 if character is `|`, `<`, `>`, or `&`
- 0 otherwise

**Detailed Description:**

This function identifies characters that begin shell operators. It's used to determine when to stop extracting a word token.

**Implementation:**

```c
return (c == '|' || c == '<' || c == '>' || c == '&');
```

**Recognized Operators:**

| Character | Operators |
|-----------|-----------|
| `|` | `|` (pipe), `||` (or) |
| `<` | `<` (input), `<<` (heredoc) |
| `>` | `>` (output), `>>` (append) |
| `&` | `&&` (and) |

**Examples:**

```c
is_operator('|');   // Returns: 1
is_operator('<');   // Returns: 1
is_operator('>');   // Returns: 1
is_operator('&');   // Returns: 1
is_operator('a');   // Returns: 0
is_operator(' ');   // Returns: 0
is_operator('\0');  // Returns: 0
```

**Why These Characters?**

These are the characters that begin multi-character operators:
- `|` can be `|` or `||`
- `<` can be `<` or `<<`
- `>` can be `>` or `>>`
- `&` can be `&&`

**Usage in Lexer:**

```c
// Extract word until operator or whitespace
while (input[i] && !is_whitespace(input[i]) && !is_operator(input[i]))
    i++;
```

This extracts characters until we hit whitespace or an operator.

**Example:**

Input: `"ls -la|grep"`

```
Position 0: 'l' - not operator, not whitespace → continue
Position 1: 's' - not operator, not whitespace → continue
Position 2: ' ' - whitespace → stop (word: "ls")

Position 3: '-' - not operator, not whitespace → continue
Position 4: 'l' - not operator, not whitespace → continue
Position 5: 'a' - not operator, not whitespace → continue
Position 6: '|' - operator → stop (word: "-la")

Position 6: '|' - operator → extract operator token
```

**Why Not Include Other Characters?**

Characters like `;`, `(`, `)`, `{`, `}` are not included because:
- `;` is not required by 42 subject
- `()` and `{}` are bonus features
- Keeping it simple for mandatory part

---

### extract_word

**Function Signature:**
```c
static int extract_word(char *input, char **word)
```

**Purpose:**  
Extract a word token from input, handling quotes properly.

**Parameters:**
- `input`: Input string starting at the word to extract
- `word`: Output pointer to store the extracted word (allocated)

**Return Value:**  
Number of characters extracted (length of word)

**Detailed Description:**

This function extracts a word from the input string, handling quotes to allow spaces and operators within quoted strings. It's one of the most complex functions in the lexer.

**Implementation:**

```c
int i;
int in_quote;
char quote_char;

i = 0;
in_quote = 0;

// Extract characters
while (input[i] && (in_quote || (!is_whitespace(input[i]) 
                                 && !is_operator(input[i])))) {
    if (!in_quote && (input[i] == '\'' || input[i] == '"')) {
        // Start quote
        quote_char = input[i];
        in_quote = 1;
    } else if (in_quote && input[i] == quote_char) {
        // End quote
        in_quote = 0;
    }
    i++;
}

// Allocate and copy
*word = malloc(i + 1);
if (!*word)
    return (0);
ft_strncpy(*word, input, i);
(*word)[i] = '\0';

return (i);
```

**Algorithm:**

```
1. Initialize:
   - i = 0 (position)
   - in_quote = 0 (not in quote)

2. While not at end AND (in quote OR not whitespace/operator):
   a. If not in quote AND char is quote (' or "):
      - Save quote character
      - Set in_quote = 1
   
   b. Else if in quote AND char matches quote_char:
      - Set in_quote = 0
   
   c. Increment i

3. Allocate memory for word (i + 1 bytes)
4. Copy i characters from input to word
5. Null-terminate word
6. Return i (length)
```

**Examples:**

**Example 1: Simple word**
```c
char *word;
int len;

len = extract_word("hello world", &word);
// len = 5
// word = "hello"
```

**Example 2: Word with single quotes**
```c
len = extract_word("'hello world' test", &word);
// len = 13
// word = "'hello world'"
```

**Example 3: Word with double quotes**
```c
len = extract_word("\"hello world\" test", &word);
// len = 13
// word = "\"hello world\""
```

**Example 4: Mixed quotes**
```c
len = extract_word("'hello \"world\"' test", &word);
// len = 16
// word = "'hello \"world\"'"
```

**Example 5: Operator in quotes**
```c
len = extract_word("'ls | grep' test", &word);
// len = 11
// word = "'ls | grep'"
```

**Example 6: Unclosed quote**
```c
len = extract_word("'hello world", &word);
// len = 12
// word = "'hello world"
// Note: Unclosed quote extends to end of input
```

**Quote Handling:**

**State Machine:**

```
State: NOT_IN_QUOTE
  - See ' or " → Enter IN_QUOTE, save quote char
  - See whitespace/operator → Stop extraction
  - See other char → Continue

State: IN_QUOTE
  - See matching quote char → Exit to NOT_IN_QUOTE
  - See any other char → Continue (including whitespace/operators)
```

**Example Trace:**

Input: `"'hello world' test"`

```
Position 0: ''' - not in quote, is quote → enter quote (quote_char = ''')
Position 1: 'h' - in quote → continue
Position 2: 'e' - in quote → continue
Position 3: 'l' - in quote → continue
Position 4: 'l' - in quote → continue
Position 5: 'o' - in quote → continue
Position 6: ' ' - in quote → continue (space allowed in quotes!)
Position 7: 'w' - in quote → continue
Position 8: 'o' - in quote → continue
Position 9: 'r' - in quote → continue
Position 10: 'l' - in quote → continue
Position 11: 'd' - in quote → continue
Position 12: ''' - in quote, matches quote_char → exit quote
Position 13: ' ' - not in quote, is whitespace → STOP

Result: "'hello world'" (length 13)
```

**Why Include Quotes in Word?**

The extracted word includes the quote characters:
```c
Input: "'hello world'"
Word: "'hello world'"  // Quotes included
```

This is intentional because:
1. The expander will remove quotes later
2. Allows distinguishing between quoted and unquoted strings
3. Preserves original input structure

**Memory Management:**

```c
*word = malloc(i + 1);
```

- Allocates `i + 1` bytes (i characters + null terminator)
- Caller must free the allocated word
- Returns 0 if malloc fails

**Edge Cases:**

**Case 1: Empty input**
```c
len = extract_word("", &word);
// len = 0
// word = "" (empty string)
```

**Case 2: Only quotes**
```c
len = extract_word("''", &word);
// len = 2
// word = "''"
```

**Case 3: Nested quotes (different types)**
```c
len = extract_word("\"hello 'world'\"", &word);
// len = 15
// word = "\"hello 'world'\""
// Inner quotes are just characters
```

**Case 4: Nested quotes (same type)**
```c
len = extract_word("'hello 'world''", &word);
// len = 7
// word = "'hello '"
// First closing quote ends the string
```

**Why This Matters:**

Quotes allow:
- Spaces in arguments: `echo "hello world"`
- Operators in strings: `echo "ls | grep"`
- Special characters: `echo "test$var"`

---

### get_operator_token

**Function Signature:**
```c
static t_token *get_operator_token(char **input)
```

**Purpose:**  
Identify and create a token for shell operators, handling both single and double-character operators.

**Parameters:**
- `input`: Pointer to pointer to current position in input string (modified to advance past operator)

**Return Value:**
- Pointer to newly created operator token
- NULL if no operator found (shouldn't happen if called correctly)

**Detailed Description:**

This function recognizes shell operators and creates appropriate tokens. It handles:
- Single-character operators: `|`, `<`, `>`
- Double-character operators: `||`, `&&`, `<<`, `>>`

**Implementation:**

```c
// Check for double-character operators first
if (**input == '|' && *(*input + 1) == '|') {
    (*input) += 2;
    return (create_token(TOKEN_OR, "||"));
}

if (**input == '|') {
    (*input)++;
    return (create_token(TOKEN_PIPE, "|"));
}

if (**input == '&' && *(*input + 1) == '&') {
    (*input) += 2;
    return (create_token(TOKEN_AND, "&&"));
}

if (**input == '<' && *(*input + 1) == '<') {
    (*input) += 2;
    return (create_token(TOKEN_REDIR_HEREDOC, "<<"));
}

if (**input == '<') {
    (*input)++;
    return (create_token(TOKEN_REDIR_IN, "<"));
}

if (**input == '>' && *(*input + 1) == '>') {
    (*input) += 2;
    return (create_token(TOKEN_REDIR_APPEND, ">>"));
}

if (**input == '>') {
    (*input)++;
    return (create_token(TOKEN_REDIR_OUT, ">"));
}

return (NULL);
```

**Algorithm:**

```
1. Check current and next character:
   
   If "||":
     - Advance input by 2
     - Return TOKEN_OR
   
   If "|":
     - Advance input by 1
     - Return TOKEN_PIPE
   
   If "&&":
     - Advance input by 2
     - Return TOKEN_AND
   
   If "<<":
     - Advance input by 2
     - Return TOKEN_REDIR_HEREDOC
   
   If "<":
     - Advance input by 1
     - Return TOKEN_REDIR_IN
   
   If ">>":
     - Advance input by 2
     - Return TOKEN_REDIR_APPEND
   
   If ">":
     - Advance input by 1
     - Return TOKEN_REDIR_OUT

2. Return NULL (no operator found)
```

**Examples:**

**Example 1: Single pipe**
```c
char *input = "| grep";
t_token *token = get_operator_token(&input);

// token->type = TOKEN_PIPE
// token->value = "|"
// input now points to " grep"
```

**Example 2: Double pipe (OR)**
```c
char *input = "|| echo";
t_token *token = get_operator_token(&input);

// token->type = TOKEN_OR
// token->value = "||"
// input now points to " echo"
```

**Example 3: Heredoc**
```c
char *input = "<< EOF";
t_token *token = get_operator_token(&input);

// token->type = TOKEN_REDIR_HEREDOC
// token->value = "<<"
// input now points to " EOF"
```

**Example 4: Append redirection**
```c
char *input = ">> file.txt";
t_token *token = get_operator_token(&input);

// token->type = TOKEN_REDIR_APPEND
// token->value = ">>"
// input now points to " file.txt"
```

**Why Check Double-Character First?**

```c
// ✅ Correct order:
if (**input == '|' && *(*input + 1) == '|')  // Check || first
    return TOKEN_OR;
if (**input == '|')                           // Then check |
    return TOKEN_PIPE;

// ❌ Wrong order:
if (**input == '|')                           // Matches | in ||
    return TOKEN_PIPE;  // Would never reach ||!
if (**input == '|' && *(*input + 1) == '|')
    return TOKEN_OR;  // Unreachable!
```

If we check single-character operators first, we'd never recognize double-character operators.

**Input Pointer Advancement:**

```c
(*input) += 2;  // For double-character operators
(*input)++;     // For single-character operators
```

This advances the input pointer past the operator, so the lexer continues from the next character.

**Example Trace:**

Input: `"ls||echo"`

```
Position: "ls||echo"
          ^
Call extract_word() → "ls", advance by 2

Position: "||echo"
          ^
is_operator('|') → true
Call get_operator_token()
  Check "||" → match!
  Create TOKEN_OR
  Advance by 2

Position: "echo"
          ^
Call extract_word() → "echo", advance by 4

Result: [WORD:"ls"] → [OR:"||"] → [WORD:"echo"]
```

**Operator Precedence:**

The order of checks matters:

| Check Order | Operator | Reason |
|-------------|----------|--------|
| 1 | `||` | Must check before `|` |
| 2 | `|` | Single pipe |
| 3 | `&&` | Double ampersand |
| 4 | `<<` | Must check before `<` |
| 5 | `<` | Single less-than |
| 6 | `>>` | Must check before `>` |
| 7 | `>` | Single greater-than |

**Why Pass `**input`?**

```c
static t_token *get_operator_token(char **input)
```

We need `**input` (pointer to pointer) because:
1. We need to modify the input pointer
2. We need the change to persist after function returns

Example:
```c
char *input = "| grep";
get_operator_token(&input);
// input now points to " grep" (advanced past '|')
```

**Comma Operator Usage:**

```c
return ((*input)++, create_token(TOKEN_PIPE, "|"));
```

This is equivalent to:
```c
(*input)++;
return create_token(TOKEN_PIPE, "|");
```

The comma operator evaluates left expression, then right expression, and returns the right value.

---

### lexer

**Function Signature:**
```c
t_token *lexer(char *input)
```

**Purpose:**  
Main lexer function that tokenizes an input string into a linked list of tokens.

**Parameters:**
- `input`: Input string to tokenize

**Return Value:**
- Pointer to head of token list
- NULL if input is empty or only whitespace

**Detailed Description:**

This is the main entry point for lexical analysis. It orchestrates the tokenization process by:
1. Skipping whitespace
2. Identifying operators
3. Extracting words
4. Building the token list

**Implementation:**

```c
t_token *tokens;
t_token *new_token;
char *word;
int len;

tokens = NULL;

while (*input) {
    // Skip whitespace
    while (is_whitespace(*input))
        input++;
    
    // Check if end of input
    if (!*input)
        break;
    
    // Check for operator
    if (is_operator(*input)) {
        new_token = get_operator_token(&input);
    } else {
        // Extract word
        len = extract_word(input, &word);
        new_token = create_token(TOKEN_WORD, word);
        free(word);  // Free temporary word
        input += len;
    }
    
    // Add token to list
    if (new_token)
        add_token(&tokens, new_token);
}

return (tokens);
```

**Algorithm:**

```
1. Initialize empty token list

2. While not at end of input:
   a. Skip all whitespace
   
   b. If at end of input:
      - Break
   
   c. If current char is operator:
      - Call get_operator_token()
      - Advances input automatically
   
   d. Else (word):
      - Call extract_word()
      - Create TOKEN_WORD
      - Free temporary word string
      - Advance input by word length
   
   e. Add new token to list

3. Return token list
```

**Examples:**

**Example 1: Simple command**
```c
t_token *tokens = lexer("ls -la");

// Result:
// [WORD:"ls"] → [WORD:"-la"] → NULL
```

**Example 2: Command with pipe**
```c
t_token *tokens = lexer("ls | grep txt");

// Result:
// [WORD:"ls"] → [PIPE:"|"] → [WORD:"grep"] → [WORD:"txt"] → NULL
```

**Example 3: Command with redirections**
```c
t_token *tokens = lexer("cat < input.txt > output.txt");

// Result:
// [WORD:"cat"] → [REDIR_IN:"<"] → [WORD:"input.txt"] → 
// [REDIR_OUT:">"] → [WORD:"output.txt"] → NULL
```

**Example 4: Complex command**
```c
t_token *tokens = lexer("echo 'hello world' | grep hello >> log.txt");

// Result:
// [WORD:"echo"] → [WORD:"'hello world'"] → [PIPE:"|"] → 
// [WORD:"grep"] → [WORD:"hello"] → [REDIR_APPEND:">>"] → 
// [WORD:"log.txt"] → NULL
```

**Example 5: Logical operators (bonus)**
```c
t_token *tokens = lexer("make && ./program || echo failed");

// Result:
// [WORD:"make"] → [AND:"&&"] → [WORD:"./program"] → 
// [OR:"||"] → [WORD:"echo"] → [WORD:"failed"] → NULL
```

**Example 6: Heredoc**
```c
t_token *tokens = lexer("cat << EOF");

// Result:
// [WORD:"cat"] → [REDIR_HEREDOC:"<<"] → [WORD:"EOF"] → NULL
```

**Detailed Trace:**

Input: `"ls -la | grep txt"`

```
Step 1: input = "ls -la | grep txt"
        Skip whitespace: none
        is_operator('l')? No
        extract_word() → "ls", len=2
        Create [WORD:"ls"]
        Advance input by 2

Step 2: input = " -la | grep txt"
        Skip whitespace: 1 space
        input = "-la | grep txt"
        is_operator('-')? No
        extract_word() → "-la", len=3
        Create [WORD:"-la"]
        Advance input by 3

Step 3: input = " | grep txt"
        Skip whitespace: 1 space
        input = "| grep txt"
        is_operator('|')? Yes
        get_operator_token() → [PIPE:"|"]
        Advances input automatically

Step 4: input = " grep txt"
        Skip whitespace: 1 space
        input = "grep txt"
        is_operator('g')? No
        extract_word() → "grep", len=4
        Create [WORD:"grep"]
        Advance input by 4

Step 5: input = " txt"
        Skip whitespace: 1 space
        input = "txt"
        is_operator('t')? No
        extract_word() → "txt", len=3
        Create [WORD:"txt"]
        Advance input by 3

Step 6: input = ""
        End of input, break

Result: [WORD:"ls"] → [WORD:"-la"] → [PIPE:"|"] → 
        [WORD:"grep"] → [WORD:"txt"] → NULL
```

**Whitespace Handling:**

```c
while (is_whitespace(*input))
    input++;
```

This skips all consecutive whitespace before each token. Multiple spaces are collapsed:

```c
lexer("ls    -la")  // Multiple spaces
// Result: [WORD:"ls"] → [WORD:"-la"]
// Spaces are skipped, not tokenized
```

**Empty Input:**

```c
t_token *tokens = lexer("");
// tokens = NULL (empty list)

tokens = lexer("   ");  // Only whitespace
// tokens = NULL (empty list)
```

**Memory Management:**

```c
len = extract_word(input, &word);
new_token = create_token(TOKEN_WORD, word);
free(word);  // Important! Free temporary word
```

- `extract_word()` allocates `word`
- `create_token()` duplicates `word` internally
- We must free the temporary `word`

**Error Handling:**

The lexer doesn't validate syntax. It only tokenizes. Invalid syntax is caught by the parser:

```c
// Lexer accepts this:
tokens = lexer("| | |");
// Result: [PIPE:"|"] → [PIPE:"|"] → [PIPE:"|"]
// Parser will reject it as invalid syntax
```

**Quote Handling:**

Quotes are preserved in tokens:

```c
tokens = lexer("echo 'hello world'");
// Result: [WORD:"echo"] → [WORD:"'hello world'"]
// Quotes included in token value
// Expander will remove them later
```

**Return Value:**

- Returns pointer to head of token list
- Returns NULL if no tokens (empty input)
- Caller must call `free_tokens()` when done

**Usage Pattern:**

```c
// Tokenize input
t_token *tokens = lexer(input);

// Check if empty
if (!tokens) {
    // Empty input
    return;
}

// Use tokens (pass to parser)
t_pipeline *pipeline = parser(tokens);

// Clean up
free_tokens(tokens);
```

---

## Parser Module

### Data Structures

**Redirection Structure:**
```c
typedef struct s_redir {
    t_token_type    type;   // Type of redirection
    char            *file;  // Target filename or heredoc delimiter
    struct s_redir  *next;  // Next redirection
} t_redir;
```

**Command Structure:**
```c
typedef struct s_cmd {
    char            **args;   // NULL-terminated argument array
    t_redir         *redirs;  // Linked list of redirections
    struct s_cmd    *next;    // Next command in pipeline
} t_cmd;
```

**Pipeline Structure:**
```c
typedef struct s_pipeline {
    t_cmd               *cmds;      // Linked list of commands
    t_token_type        logic_op;   // Logical operator (&&, ||, or EOF)
    struct s_pipeline   *next;      // Next pipeline
} t_pipeline;
```

### Parser Overview

The parser transforms a flat token list into a hierarchical structure:

```
Token List:
[ls] → [-la] → [|] → [grep] → [txt] → [>] → [out.txt]

↓ PARSE ↓

Pipeline Structure:
Pipeline {
    cmds: [
        Command {
            args: ["ls", "-la", NULL]
            redirs: NULL
        },
        Command {
            args: ["grep", "txt", NULL]
            redirs: [> out.txt]
        }
    ],
    logic_op: EOF
}
```

---

## parser.c

### create_redir

**Function Signature:**
```c
static t_redir *create_redir(t_token_type type, char *file)
```

**Purpose:**  
Create and initialize a new redirection node.

**Parameters:**
- `type`: Type of redirection (TOKEN_REDIR_IN, TOKEN_REDIR_OUT, etc.)
- `file`: Target filename or heredoc delimiter

**Return Value:**
- Pointer to newly allocated redirection
- NULL if malloc fails

**Detailed Description:**

This function creates a redirection node that stores information about input/output redirection for a command.

**Implementation:**

```c
t_redir *redir;

redir = malloc(sizeof(t_redir));
if (!redir)
    return (NULL);

redir->type = type;
redir->file = ft_strdup(file);
redir->next = NULL;

return (redir);
```

**Examples:**

**Example 1: Input redirection**
```c
t_redir *redir = create_redir(TOKEN_REDIR_IN, "input.txt");
// redir->type = TOKEN_REDIR_IN
// redir->file = "input.txt"
// redir->next = NULL
```

**Example 2: Output redirection**
```c
t_redir *redir = create_redir(TOKEN_REDIR_OUT, "output.txt");
// redir->type = TOKEN_REDIR_OUT
// redir->file = "output.txt"
```

**Example 3: Append redirection**
```c
t_redir *redir = create_redir(TOKEN_REDIR_APPEND, "log.txt");
// redir->type = TOKEN_REDIR_APPEND
// redir->file = "log.txt"
```

**Example 4: Heredoc**
```c
t_redir *redir = create_redir(TOKEN_REDIR_HEREDOC, "EOF");
// redir->type = TOKEN_REDIR_HEREDOC
// redir->file = "EOF" (delimiter, not filename)
```

**Memory Management:**

- Allocates redirection structure
- Duplicates file string
- Caller must free via `free_pipeline()`

---

### parse_redirections

**Function Signature:**
```c
t_redir *parse_redirections(t_token **tokens)
```

**Purpose:**  
Parse all consecutive redirection tokens and build a linked list of redirections.

**Parameters:**
- `tokens`: Pointer to pointer to current position in token list (modified)

**Return Value:**
- Pointer to head of redirection list
- NULL if no redirections found

**Detailed Description:**

This function extracts all redirection operators and their target files from the token stream. It handles multiple redirections for a single command.

**Implementation:**

```c
t_redir *redirs;
t_redir *new_redir;
t_redir *current;

redirs = NULL;

while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN
                || (*tokens)->type == TOKEN_REDIR_OUT
                || (*tokens)->type == TOKEN_REDIR_APPEND
                || (*tokens)->type == TOKEN_REDIR_HEREDOC)) {
    
    // Create redirection with next token's value as filename
    new_redir = create_redir((*tokens)->type, (*tokens)->next->value);
    
    // Add to list
    if (!redirs)
        redirs = new_redir;
    else {
        current = redirs;
        while (current->next)
            current = current->next;
        current->next = new_redir;
    }
    
    // Skip both operator and filename tokens
    *tokens = (*tokens)->next->next;
}

return (redirs);
```

**Algorithm:**

```
1. Initialize empty redirection list

2. While current token is a redirection operator:
   a. Create redirection:
      - Type: current token type
      - File: next token value
   
   b. Add to list:
      - If list empty: set as head
      - Else: append to end
   
   c. Skip two tokens:
      - Redirection operator
      - Filename/delimiter

3. Return redirection list
```

**Examples:**

**Example 1: Single input redirection**
```c
// Tokens: [<] → [input.txt] → [WORD:"cat"] → ...
t_redir *redirs = parse_redirections(&tokens);

// Result:
// redirs: [< input.txt] → NULL
// tokens now points to [WORD:"cat"]
```

**Example 2: Multiple redirections**
```c
// Tokens: [<] → [in.txt] → [>] → [out.txt] → [WORD:"cat"] → ...
t_redir *redirs = parse_redirections(&tokens);

// Result:
// redirs: [< in.txt] → [> out.txt] → NULL
// tokens now points to [WORD:"cat"]
```

**Example 3: No redirections**
```c
// Tokens: [WORD:"ls"] → [WORD:"-la"] → ...
t_redir *redirs = parse_redirections(&tokens);

// Result:
// redirs: NULL (no redirections)
// tokens unchanged
```

**Example 4: Heredoc and output**
```c
// Tokens: [<<] → [EOF] → [>>] → [log.txt] → [WORD:"cat"] → ...
t_redir *redirs = parse_redirections(&tokens);

// Result:
// redirs: [<< EOF] → [>> log.txt] → NULL
```

**Token Consumption:**

For each redirection, two tokens are consumed:
1. Operator token (`<`, `>`, `<<`, `>>`)
2. Filename/delimiter token

```c
*tokens = (*tokens)->next->next;
```

This skips both tokens.

**Why Check `(*tokens)->next->value`?**

```c
new_redir = create_redir((*tokens)->type, (*tokens)->next->value);
```

The filename is in the **next** token after the operator:

```
[<] → [input.txt]
 ^       ^
 |       └─ Filename (next token)
 └─ Operator (current token)
```

**Order Matters:**

Redirections are processed in order:

```c
// cat < in1.txt < in2.txt
// Last input redirection wins: reads from in2.txt

// cat > out1.txt > out2.txt
// Last output redirection wins: writes to out2.txt
```

---

### count_args

**Function Signature:**
```c
static int count_args(t_token *tokens)
```

**Purpose:**  
Count the number of word tokens (arguments) before a pipe or logical operator.

**Parameters:**
- `tokens`: Current position in token list

**Return Value:**  
Number of word tokens found

**Detailed Description:**

This function counts arguments for memory allocation. It skips over redirection tokens to count only word tokens.

**Implementation:**

```c
int count;

count = 0;

while (tokens && tokens->type == TOKEN_WORD) {
    count++;
    tokens = tokens->next;
    
    // Skip redirections
    while (tokens && (tokens->type == TOKEN_REDIR_IN
                   || tokens->type == TOKEN_REDIR_OUT
                   || tokens->type == TOKEN_REDIR_APPEND
                   || tokens->type == TOKEN_REDIR_HEREDOC)) {
        tokens = tokens->next->next;  // Skip operator and filename
    }
}

return (count);
```

**Algorithm:**

```
1. Initialize count = 0

2. While current token is WORD:
   a. Increment count
   b. Move to next token
   c. Skip any redirections:
      - For each redirection:
        - Skip operator token
        - Skip filename token

3. Return count
```

**Examples:**

**Example 1: Simple command**
```c
// Tokens: [ls] → [- la] → [|] → ...
int count = count_args(tokens);
// count = 2
```

**Example 2: Command with redirections**
```c
// Tokens: [cat] → [<] → [in.txt] → [file.txt] → [>] → [out.txt] → [|] → ...
int count = count_args(tokens);
// count = 2 (cat, file.txt)
// Redirections are skipped
```

**Example 3: No arguments**
```c
// Tokens: [|] → ...
int count = count_args(tokens);
// count = 0
```

**Example 4: Complex command**
```c
// Tokens: [grep] → [pattern] → [file1] → [<] → [in] → [file2] → [>] → [out] → [|]
int count = count_args(tokens);
// count = 4 (grep, pattern, file1, file2)
```

**Why Skip Redirections?**

Redirections are not arguments to the command. They're metadata about I/O:

```bash
cat < input.txt file.txt > output.txt
```

Arguments: `["cat", "file.txt"]`  
Redirections: `[< input.txt, > output.txt]`

**Usage:**

```c
// Count arguments
int arg_count = count_args(tokens);

// Allocate array (+ 1 for NULL terminator)
char **args = malloc(sizeof(char *) * (arg_count + 1));
```

---

### parse_command

**Function Signature:**
```c
t_cmd *parse_command(t_token **tokens)
```

**Purpose:**  
Parse a single command with its arguments and redirections.

**Parameters:**
- `tokens`: Pointer to pointer to current position in token list (modified)

**Return Value:**
- Pointer to newly allocated command structure
- NULL if malloc fails

**Detailed Description:**

This function builds a command structure from tokens. It extracts arguments and redirections until it hits a pipe or end of tokens.

**Implementation:**

```c
t_cmd *cmd;
int i;
int arg_count;

cmd = malloc(sizeof(t_cmd));
if (!cmd)
    return (NULL);

// Count arguments for allocation
arg_count = count_args(*tokens);
cmd->args = malloc(sizeof(char *) * (arg_count + 1));

i = 0;
while (*tokens && (*tokens)->type == TOKEN_WORD) {
    // Copy argument
    cmd->args[i++] = ft_strdup((*tokens)->value);
    *tokens = (*tokens)->next;
    
    // Parse any redirections after this argument
    cmd->redirs = parse_redirections(tokens);
}

cmd->args[i] = NULL;  // NULL-terminate
cmd->next = NULL;

return (cmd);
```

**Algorithm:**

```
1. Allocate command structure

2. Count arguments

3. Allocate argument array (size + 1 for NULL)

4. While current token is WORD:
   a. Duplicate token value to args array
   b. Move to next token
   c. Parse any redirections

5. NULL-terminate argument array

6. Initialize next pointer to NULL

7. Return command
```

**Examples:**

**Example 1: Simple command**
```c
// Tokens: [ls] → [-la] → [|] → ...
t_cmd *cmd = parse_command(&tokens);

// cmd->args = ["ls", "-la", NULL]
// cmd->redirs = NULL
// tokens now points to [|]
```

**Example 2: Command with redirections**
```c
// Tokens: [cat] → [<] → [in.txt] → [>] → [out.txt] → [|] → ...
t_cmd *cmd = parse_command(&tokens);

// cmd->args = ["cat", NULL]
// cmd->redirs = [< in.txt] → [> out.txt]
// tokens now points to [|]
```

**Example 3: Multiple arguments and redirections**
```c
// Tokens: [grep] → [pattern] → [<] → [in] → [file.txt] → [>] → [out] → [|]
t_cmd *cmd = parse_command(&tokens);

// cmd->args = ["grep", "pattern", "file.txt", NULL]
// cmd->redirs = [< in] → [> out]
```

**Redirection Parsing:**

```c
cmd->redirs = parse_redirections(tokens);
```

This is called after each word token, allowing redirections to appear anywhere:

```bash
cat < input.txt file.txt > output.txt
```

Parsed as:
```
args: ["cat", "file.txt"]
redirs: [< input.txt, > output.txt]
```

**NULL Termination:**

```c
cmd->args[i] = NULL;
```

The argument array must be NULL-terminated for `execve()`:

```c
execve(path, args, envp);
// args must be: ["command", "arg1", "arg2", ..., NULL]
```

**Memory Management:**

- Command structure: allocated
- Argument array: allocated
- Each argument string: duplicated
- Redirections: allocated by `parse_redirections()`

All must be freed via `free_pipeline()`.

---

### free_pipeline

**Function Signature:**
```c
void free_pipeline(t_pipeline *pipeline)
```

**Purpose:**  
Free all memory associated with a pipeline structure, including all commands and redirections.

**Parameters:**
- `pipeline`: Head of pipeline list to free

**Return Value:**  
void

**Detailed Description:**

This function performs a complete cleanup of a pipeline structure. It recursively frees:
1. Pipeline nodes
2. Command nodes
3. Argument arrays
4. Redirection nodes

**Implementation:**

```c
t_pipeline *tmp_pipe;
t_cmd *tmp_cmd;
t_redir *tmp_redir;

while (pipeline) {
    tmp_pipe = pipeline;
    
    // Free all commands in this pipeline
    while (pipeline->cmds) {
        tmp_cmd = pipeline->cmds;
        
        // Free argument array
        free_array(tmp_cmd->args);
        
        // Free all redirections
        while (tmp_cmd->redirs) {
            tmp_redir = tmp_cmd->redirs;
            tmp_cmd->redirs = tmp_redir->next;
            free(tmp_redir->file);
            free(tmp_redir);
        }
        
        pipeline->cmds = tmp_cmd->next;
        free(tmp_cmd);
    }
    
    pipeline = pipeline->next;
    free(tmp_pipe);
}
```

**Algorithm:**

```
1. For each pipeline:
   a. For each command:
      i. Free argument array
      ii. For each redirection:
          - Free filename string
          - Free redirection structure
      iii. Free command structure
   b. Free pipeline structure

2. Continue until all pipelines freed
```

**Memory Freed:**

For a pipeline like: `cat < in.txt | grep pattern > out.txt`

```
Pipeline structure
├── Command 1
│   ├── args: ["cat", NULL]
│   │   ├── "cat" (string)
│   │   └── array itself
│   ├── redirs
│   │   └── [< in.txt]
│   │       ├── "in.txt" (string)
│   │       └── redir structure
│   └── cmd structure
├── Command 2
│   ├── args: ["grep", "pattern", NULL]
│   │   ├── "grep" (string)
│   │   ├── "pattern" (string)
│   │   └── array itself
│   ├── redirs
│   │   └── [> out.txt]
│   │       ├── "out.txt" (string)
│   │       └── redir structure
│   └── cmd structure
└── pipeline structure
```

All of this memory is freed.

**Usage Pattern:**

```c
// Parse input
t_token *tokens = lexer(input);
t_pipeline *pipeline = parser(tokens);

// Execute
executor(pipeline, shell);

// Clean up
free_pipeline(pipeline);
free_tokens(tokens);
```

---

## parser_pipeline.c

### create_pipeline

**Function Signature:**
```c
static t_pipeline *create_pipeline(void)
```

**Purpose:**  
Create and initialize a new pipeline node.

**Parameters:**  
None

**Return Value:**
- Pointer to newly allocated pipeline
- NULL if malloc fails

**Detailed Description:**

This function creates an empty pipeline structure with default values.

**Implementation:**

```c
t_pipeline *pipeline;

pipeline = malloc(sizeof(t_pipeline));
if (!pipeline)
    return (NULL);

pipeline->cmds = NULL;
pipeline->logic_op = TOKEN_EOF;
pipeline->next = NULL;

return (pipeline);
```

**Examples:**

```c
t_pipeline *pipeline = create_pipeline();
// pipeline->cmds = NULL
// pipeline->logic_op = TOKEN_EOF
// pipeline->next = NULL
```

---

### parse_pipe_sequence

**Function Signature:**
```c
static t_cmd *parse_pipe_sequence(t_token **tokens)
```

**Purpose:**  
Parse a sequence of commands separated by pipes.

**Parameters:**
- `tokens`: Pointer to pointer to current position in token list (modified)

**Return Value:**  
Pointer to head of command list

**Detailed Description:**

This function builds a linked list of commands connected by pipes. It stops when it encounters a logical operator (`&&` or `||`) or end of tokens.

**Implementation:**

```c
t_cmd *cmds;
t_cmd *new_cmd;
t_cmd *current;

cmds = NULL;

while (*tokens && (*tokens)->type != TOKEN_AND 
               && (*tokens)->type != TOKEN_OR) {
    
    // Parse one command
    new_cmd = parse_command(tokens);
    
    // Add to list
    if (!cmds)
        cmds = new_cmd;
    else {
        current = cmds;
        while (current->next)
            current = current->next;
        current->next = new_cmd;
    }
    
    // Check for pipe
    if (*tokens && (*tokens)->type == TOKEN_PIPE) {
        *tokens = (*tokens)->next;  // Skip pipe
    } else {
        break;  // No more pipes
    }
}

return (cmds);
```

**Examples:**

**Example 1: Single command**
```c
// Tokens: [ls] → [-la]
t_cmd *cmds = parse_pipe_sequence(&tokens);
// cmds: [Command: ls -la] → NULL
```

**Example 2: Two commands with pipe**
```c
// Tokens: [ls] → [|] → [grep] → [txt]
t_cmd *cmds = parse_pipe_sequence(&tokens);
// cmds: [Command: ls] → [Command: grep txt] → NULL
```

**Example 3: Three commands**
```c
// Tokens: [cat] → [file] → [|] → [grep] → [pattern] → [|] → [wc] → [-l]
t_cmd *cmds = parse_pipe_sequence(&tokens);
// cmds: [cat file] → [grep pattern] → [wc -l] → NULL
```

---

### parser

**Function Signature:**
```c
t_pipeline *parser(t_token *tokens)
```

**Purpose:**  
Main parser function that transforms tokens into pipeline structures.

**Parameters:**
- `tokens`: Head of token list from lexer

**Return Value:**  
Pointer to head of pipeline list

**Detailed Description:**

This is the main entry point for parsing. It builds the complete Abstract Syntax Tree (AST) from tokens, handling pipes and logical operators.

**Implementation:**

```c
t_pipeline *pipelines;
t_pipeline *new_pipeline;
t_pipeline *current;

pipelines = NULL;

while (tokens) {
    // Create new pipeline
    new_pipeline = create_pipeline();
    
    // Parse pipe sequence
    new_pipeline->cmds = parse_pipe_sequence(&tokens);
    
    // Check for logical operator
    if (tokens && (tokens->type == TOKEN_AND || tokens->type == TOKEN_OR)) {
        new_pipeline->logic_op = tokens->type;
        tokens = tokens->next;
    }
    
    // Add to list
    if (!pipelines)
        pipelines = new_pipeline;
    else {
        current = pipelines;
        while (current->next)
            current = current->next;
        current->next = new_pipeline;
    }
    
    // Check for end
    if (!tokens || tokens->type == TOKEN_EOF)
        break;
}

return (pipelines);
```

**Examples:**

**Example 1: Simple command**
```c
// Input: "ls -la"
// Tokens: [ls] → [-la]
t_pipeline *pipeline = parser(tokens);

// Result:
// Pipeline {
//     cmds: [Command: ls -la]
//     logic_op: EOF
// }
```

**Example 2: Pipeline**
```c
// Input: "ls | grep txt"
// Tokens: [ls] → [|] → [grep] → [txt]
t_pipeline *pipeline = parser(tokens);

// Result:
// Pipeline {
//     cmds: [Command: ls] → [Command: grep txt]
//     logic_op: EOF
// }
```

**Example 3: Logical operators (bonus)**
```c
// Input: "make && ./program || echo failed"
// Tokens: [make] → [&&] → [./program] → [||] → [echo] → [failed]
t_pipeline *pipeline = parser(tokens);

// Result:
// Pipeline 1 {
//     cmds: [Command: make]
//     logic_op: AND
// } →
// Pipeline 2 {
//     cmds: [Command: ./program]
//     logic_op: OR
// } →
// Pipeline 3 {
//     cmds: [Command: echo failed]
//     logic_op: EOF
// }
```

---

## Summary

The lexer and parser modules consist of 13 functions across 4 files:

**Lexer (8 functions):**
- `create_token` - Create new token
- `add_token` - Append token to list
- `free_tokens` - Free token list
- `is_whitespace` - Check for whitespace
- `is_operator` - Check for operator
- `extract_word` - Extract word with quote handling
- `get_operator_token` - Create operator token
- `lexer` - Main tokenization function

**Parser (5 functions):**
- `create_redir` - Create redirection node
- `parse_redirections` - Parse redirection list
- `count_args` - Count command arguments
- `parse_command` - Parse single command
- `free_pipeline` - Free pipeline structure
- `create_pipeline` - Create pipeline node
- `parse_pipe_sequence` - Parse piped commands
- `parser` - Main parsing function

These functions transform user input into an executable data structure that the executor can process.

---

**End of Documentation**
