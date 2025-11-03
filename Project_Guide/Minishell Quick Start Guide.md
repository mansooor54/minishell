# Minishell Quick Start Guide

## 🚀 Getting Started in 3 Steps

### 1. Extract the Project
```bash
unzip minishell_complete.zip
cd minishell
```

### 2. Compile
```bash
make
```

This will automatically:
- Compile libft library
- Compile all minishell source files
- Link everything together

### 3. Run
```bash
./minishell
```

## 📋 What's Included

### ✅ Mandatory Features
- All 7 built-in commands: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
- Redirections: `<`, `>`, `>>`, `<<`
- Pipes: `|`
- Environment variables: `$VAR`, `$?`
- Quote handling: `'single'` and `"double"`
- Signal handling: Ctrl-C, Ctrl-D, Ctrl-\

### ⭐ Bonus Features
- Logical operators: `&&` and `||`
- Wildcard support structure

### 📚 Complete libft Library
All utility functions are implemented in the `libft/` directory:
- String functions (strlen, strdup, strjoin, split, etc.)
- Memory functions (calloc, memset)
- Character checks (isalnum, isalpha, isdigit)
- Conversion functions (atoi, itoa)
- Output functions (putstr_fd, putendl_fd, putnbr_fd)

## 🧪 Quick Tests

```bash
# Basic commands
echo "Hello, World!"
pwd
cd ..
ls -la

# Pipes
ls | grep minishell | wc -l

# Redirections
echo "test" > file.txt
cat < file.txt
echo "more" >> file.txt

# Environment variables
export TEST=value
echo $TEST
echo $?

# Logical operators (bonus)
make && echo "Success!"
false || echo "This runs because false failed"

# Heredoc
cat << EOF
line 1
line 2
