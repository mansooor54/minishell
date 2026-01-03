# Environment Management

## Files
- `src/environment/env.c`
- `src/environment/env_node.c`
- `src/environment/env_utils.c`
- `src/environment/env_array.c`

---

## What is the Environment?

Think of the environment like a **notebook with sticky notes**. Each sticky note has:
- A **name** (like "HOME")
- A **value** (like "/Users/mansoor")

Programs read these notes to know things like:
- Where is the user's home folder?
- Where to find programs like `ls` or `grep`?
- What is the username?

**Real-life analogy**:
Imagine you're a new employee. You get a notebook with:
- `BOSS_NAME=John`
- `OFFICE_LOCATION=Room 302`
- `COFFEE_MACHINE=Kitchen`

Now you know where to find things!

---

## Visualizing the Environment

### What your system gives us (envp)

When minishell starts, the system gives us an array of strings:
```
envp[0] = "HOME=/Users/mansoor"
envp[1] = "PATH=/usr/bin:/bin"
envp[2] = "USER=mansoor"
envp[3] = "SHELL=/bin/zsh"
envp[4] = NULL  ← End marker
```

### What we convert it to (linked list)

We convert this to a linked list for easier manipulation:
```
┌─────────────────────────┐    ┌─────────────────────────┐
│ t_env node #1           │    │ t_env node #2           │
│                         │    │                         │
│ key:   "HOME"           │───►│ key:   "PATH"           │───►...───► NULL
│ value: "/Users/mansoor" │    │ value: "/usr/bin:/bin"  │
└─────────────────────────┘    └─────────────────────────┘
```

---

## Data Structure Explained

```c
typedef struct s_env
{
    char            *key;    // The variable NAME
    char            *value;  // The variable VALUE
    struct s_env    *next;   // Pointer to next variable
}   t_env;
```

**Simple example**:
```
For "HOME=/Users/mansoor":

t_env node:
┌──────────────────────────────┐
│ key ────► "HOME"             │  ← The name
│ value ──► "/Users/mansoor"   │  ← The value
│ next ───► (next node or NULL)│  ← Link to next
└──────────────────────────────┘
```

---

## Step-by-Step: How Environment is Created

### Step 1: main() receives envp
```c
int main(int argc, char **argv, char **envp)
```

The `envp` looks like this in memory:
```
envp ───► ["HOME=/Users/mansoor", "PATH=/bin:/usr/bin", "USER=mansoor", NULL]
             ↑                      ↑                    ↑              ↑
           [0]                    [1]                  [2]          end marker
```

### Step 2: init_env() converts to linked list

```c
t_env *init_env(char **envp)
{
    // For each string like "HOME=/Users/mansoor"
    // 1. Find the '=' sign
    // 2. Split into key ("HOME") and value ("/Users/mansoor")
    // 3. Create a node
    // 4. Add to linked list
}
```

**Visual process**:
```
Input: "HOME=/Users/mansoor"
            ↑
         eq_pos (position of '=')

Step 1: key = "HOME"         (everything before '=')
Step 2: value = "/Users/mansoor"  (everything after '=')
Step 3: Create node with these values
Step 4: Add to list
```

---

## Functions Explained with Examples

### init_env() - Creating the environment

```c
t_env *init_env(char **envp)
```

**What it does**: Converts string array to linked list

**Step-by-step example**:
```
Input envp:
  envp[0] = "HOME=/Users/mansoor"
  envp[1] = "USER=mansoor"
  envp[2] = NULL

Processing envp[0]: "HOME=/Users/mansoor"
  │
  ├── Find '=' at position 4
  ├── Extract key: "HOME" (positions 0-3)
  ├── Extract value: "/Users/mansoor" (after position 4)
  └── Create node: { key: "HOME", value: "/Users/mansoor", next: NULL }

Processing envp[1]: "USER=mansoor"
  │
  ├── Find '=' at position 4
  ├── Extract key: "USER"
  ├── Extract value: "mansoor"
  └── Create node and link to previous

Result:
  ┌────────────────────┐    ┌────────────────────┐
  │ key: "HOME"        │───►│ key: "USER"        │───► NULL
  │ value: "/Users/..."│    │ value: "mansoor"   │
  └────────────────────┘    └────────────────────┘
```

---

### create_env_node() - Making a new node

```c
t_env *create_env_node(char *key, char *value)
```

**What it does**: Allocates memory and creates one node

**Example**:
```c
// Call:
node = create_env_node("GREETING", "Hello");

// Result:
node ───► ┌──────────────────────┐
          │ key ────► "GREETING" │
          │ value ──► "Hello"    │
          │ next ───► NULL       │
          └──────────────────────┘
```

**Important**: It COPIES the strings (doesn't just point to them)
```c
// Inside the function:
node->key = ft_strdup(key);      // Makes a COPY of "GREETING"
node->value = ft_strdup(value);  // Makes a COPY of "Hello"
```

Why copy? Because the original might be freed or changed later!

---

### add_env_node() - Adding to the list

```c
void add_env_node(t_env **env, t_env *new_node)
```

**What it does**: Adds new node to end of list

**Example**:
```
Before:
  env ───► [HOME] ───► [PATH] ───► NULL

Call: add_env_node(&env, [USER node])

After:
  env ───► [HOME] ───► [PATH] ───► [USER] ───► NULL
                                    ↑
                              new node added!
```

**Step-by-step**:
```
1. Start at first node (HOME)
2. Walk to end: HOME → PATH → NULL
3. Found NULL? Stop at PATH
4. Set PATH->next = new_node
5. Done!
```

---

### remove_env_node() - Removing from list (for `unset`)

```c
void remove_env_node(t_env **env, char *key)
```

**What it does**: Finds and removes a node by key

**Example - Removing "PATH"**:
```
Before:
  env ───► [HOME] ───► [PATH] ───► [USER] ───► NULL

Call: remove_env_node(&env, "PATH")

Step 1: Start at HOME, is key "PATH"? No
Step 2: Move to PATH, is key "PATH"? YES!
Step 3: Link HOME to USER (skip PATH)
Step 4: Free PATH node

After:
  env ───► [HOME] ───► [USER] ───► NULL
                  ↑
        PATH is gone! (and memory freed)
```

**Visual of relinking**:
```
Before removal:
  [HOME]──►[PATH]──►[USER]
    │        │        │
   prev   current    next

After removal:
  [HOME]───────────►[USER]
    │                 │
   prev    (PATH      next
           freed)
```

---

### env_set_value() - Setting a variable (for `export`)

```c
void env_set_value(t_env **env, char *key, char *value)
```

**What it does**:
- If variable exists → update its value
- If variable doesn't exist → create new

**Example 1 - Update existing**:
```
Before:
  [HOME="/old/path"] ───► [USER="john"] ───► NULL

Call: env_set_value(&env, "HOME", "/new/path")

After:
  [HOME="/new/path"] ───► [USER="john"] ───► NULL
         ↑
   value changed!
```

**Example 2 - Create new**:
```
Before:
  [HOME="/path"] ───► [USER="john"] ───► NULL

Call: env_set_value(&env, "GREETING", "Hello")

After:
  [HOME="/path"] ───► [USER="john"] ───► [GREETING="Hello"] ───► NULL
                                                ↑
                                         new node added!
```

---

### env_to_array() - Converting back to array (for `execve`)

```c
char **env_to_array(t_env *env)
```

**What it does**: Converts linked list back to string array

**Why needed**: When we run external programs with `execve()`, it needs `char **envp` format, not our linked list!

**Example**:
```
Input (our linked list):
  [HOME="/Users/mansoor"] ───► [USER="mansoor"] ───► NULL

Output (array for execve):
  envp[0] = "HOME=/Users/mansoor"
  envp[1] = "USER=mansoor"
  envp[2] = NULL
```

**Process**:
```
For each node:
  1. Join key + "=" + value
     "HOME" + "=" + "/Users/mansoor" = "HOME=/Users/mansoor"
  2. Store in array
```

---

## Complete Example: What happens with `export`

User types: `export GREETING=Hello`

```
┌────────────────────────────────────────────────────────────────┐
│ STEP 1: Parse the argument                                     │
│                                                                │
│ Input: "GREETING=Hello"                                        │
│                                                                │
│ parse_env_string("GREETING=Hello", &key, &value)              │
│   → key = "GREETING"                                           │
│   → value = "Hello"                                            │
└────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────┐
│ STEP 2: Check if valid identifier                              │
│                                                                │
│ is_valid_identifier("GREETING") → YES                          │
│   ✓ Starts with letter                                         │
│   ✓ Contains only letters/numbers/underscore                   │
└────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────┐
│ STEP 3: Set the value                                          │
│                                                                │
│ env_set_value(&shell->env, "GREETING", "Hello")               │
│                                                                │
│ Search list for "GREETING"...                                  │
│   [HOME] → not it                                              │
│   [USER] → not it                                              │
│   [PATH] → not it                                              │
│   NULL → not found!                                            │
│                                                                │
│ Create new node: [GREETING="Hello"]                            │
│ Add to end of list                                             │
└────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────┐
│ RESULT                                                         │
│                                                                │
│ Before:                                                        │
│   [HOME] ───► [USER] ───► [PATH] ───► NULL                    │
│                                                                │
│ After:                                                         │
│   [HOME] ───► [USER] ───► [PATH] ───► [GREETING] ───► NULL    │
│                                              ↑                 │
│                                         new variable!          │
└────────────────────────────────────────────────────────────────┘
```

---

## Complete Example: What happens with `unset`

User types: `unset PATH`

```
┌────────────────────────────────────────────────────────────────┐
│ STEP 1: Find the node                                          │
│                                                                │
│ remove_env_node(&shell->env, "PATH")                          │
│                                                                │
│ Search:                                                        │
│   [HOME] → is key "PATH"? No                                   │
│   [USER] → is key "PATH"? No                                   │
│   [PATH] → is key "PATH"? YES! Found it!                       │
└────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────┐
│ STEP 2: Relink the list                                        │
│                                                                │
│ Before:                                                        │
│   [HOME] ───► [USER] ───► [PATH] ───► [GREETING] ───► NULL    │
│                 │           │              │                   │
│                prev      current         next                  │
│                                                                │
│ Link prev->next to current->next:                              │
│   [USER]->next = [PATH]->next                                  │
│   [USER]->next = [GREETING]                                    │
└────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────┐
│ STEP 3: Free the removed node                                  │
│                                                                │
│ free(current->key);    // Free "PATH"                          │
│ free(current->value);  // Free "/usr/bin:/bin"                 │
│ free(current);         // Free the node itself                 │
└────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌────────────────────────────────────────────────────────────────┐
│ RESULT                                                         │
│                                                                │
│ After:                                                         │
│   [HOME] ───► [USER] ───► [GREETING] ───► NULL                │
│                      ↑                                         │
│              PATH is gone!                                     │
└────────────────────────────────────────────────────────────────┘
```

---

## Why Linked List Instead of Array?

### With Array (harder):
```
To add "NEW=value":
  1. Count current size (n)
  2. Allocate new array of size n+2
  3. Copy all n strings
  4. Add new string
  5. Free old array

To remove:
  1. Find index
  2. Allocate new array of size n
  3. Copy all strings except removed one
  4. Free old strings and array
```

### With Linked List (easier):
```
To add "NEW=value":
  1. Create node
  2. Walk to end
  3. Link it

To remove:
  1. Find node
  2. Update previous->next
  3. Free node
```

Much simpler!

---

## Special Variables

| Variable | What it's for | Example Value | Who sets it |
|----------|---------------|---------------|-------------|
| `HOME` | User's home directory | `/Users/mansoor` | System |
| `PATH` | Where to find programs | `/usr/bin:/bin` | System |
| `PWD` | Current directory | `/Users/mansoor/minishell` | `cd` command |
| `OLDPWD` | Previous directory | `/Users/mansoor` | `cd` command |
| `USER` | Username | `mansoor` | System |
| `SHLVL` | Shell nesting level | `1`, `2`, `3`... | Shell |
| `?` | Exit status | `0`, `1`, `127`... | Shell (special) |

---

## Memory Management Rules

### Rule 1: Always copy strings
```c
// WRONG:
node->key = key;  // Just points to original!

// RIGHT:
node->key = ft_strdup(key);  // Makes a copy!
```

### Rule 2: Free before replacing
```c
// WRONG:
node->value = new_value;  // Old value is leaked!

// RIGHT:
free(node->value);        // Free old value first
node->value = ft_strdup(new_value);  // Then set new
```

### Rule 3: Free everything when removing
```c
// When removing a node:
free(node->key);    // Free the key string
free(node->value);  // Free the value string
free(node);         // Free the node itself
```

---

## Common Questions

### Q: Why does `env` show variables but not `export VAR` (without value)?

**Answer**: `env` only shows variables that have values assigned.

```bash
export VAR          # Creates VAR but no value
env | grep VAR      # Nothing! (no value means not shown)
export VAR=hello    # Now VAR has a value
env | grep VAR      # VAR=hello (now it shows!)
```

### Q: What happens to $PATH after `unset PATH`?

**Answer**: Commands like `ls` won't be found!

```bash
echo $PATH          # /usr/bin:/bin
ls                  # Works!
unset PATH
ls                  # minishell: ls: command not found
```

### Q: Why do we need to convert back to array for execve?

**Answer**: That's what the system call requires!

```c
// execve signature:
int execve(const char *path, char *const argv[], char *const envp[]);
                                                        ↑
                                            Must be array, not linked list!
```
