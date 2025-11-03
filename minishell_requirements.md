# Minishell 42 School Requirements

## Mandatory Features

Based on research from 42 documentation and community resources:

### Core Requirements
1. **Prompt Display**: Display a prompt when waiting for a new command
2. **Command History**: Working history
3. **Command Execution**: Search and launch the right executable (based on PATH or using relative/absolute path)
4. **Quote Handling**: 
   - Single quotes `'` prevent interpretation of metacharacters
   - Double quotes `"` prevent interpretation except for `$`
5. **Redirections**:
   - `<` redirect input
   - `>` redirect output
   - `<<` heredoc (read input until delimiter)
   - `>>` append output
6. **Pipes**: `|` character should pipe output to input of next command
7. **Environment Variables**: `$VAR` expansion
8. **Special Variable**: `$?` expand to exit status of most recent pipeline
9. **Signal Handling**:
   - `Ctrl-C` display new prompt
   - `Ctrl-D` exit shell
   - `Ctrl-\` do nothing

### Built-in Commands (Mandatory)
1. `echo` with option `-n`
2. `cd` with only relative or absolute path
3. `pwd` with no options
4. `export` with no options
5. `unset` with no options
6. `env` with no options or arguments
7. `exit` with no options

## Bonus Features

### Logical Operators
- `&&` (AND) - execute next command only if previous succeeded
- `||` (OR) - execute next command only if previous failed

### Wildcards
- `*` should work for current working directory

## Architecture Recommendation
From 42 Docs: `lexer` -> `parser` -> `expander` -> `executor`

## Important Constraints
- No memory leaks
- No file descriptor leaks (including child processes)
- Shell must never crash
- Follow Norminette coding style
- Max 5 functions per file
- Max 25 lines per function
