# Whiteboard Explanation: File Descriptor Management for `ls | grep .c`

**A Step-by-Step Visual Guide for Defense**

---

## Table of Contents

1. [Introduction](#introduction)
2. [Initial Setup](#initial-setup)
3. [Step-by-Step Process](#step-by-step-process)
4. [Visual Diagrams](#visual-diagrams)
5. [Code Walkthrough](#code-walkthrough)
6. [Common Questions](#common-questions)
7. [Presentation Script](#presentation-script)

---

## Introduction

This document provides a **whiteboard-ready explanation** of how file descriptors are managed when executing a piped command.

**Example Command:**
```bash
ls | grep .c
```

**What This Demonstrates:**
- Pipe creation
- File descriptor duplication
- Process forking
- Inter-process communication
- Proper fd cleanup

**Time Required:** 5-7 minutes to present

---

## Initial Setup

### Understanding File Descriptors

**Draw this table first:**

```
┌─────────────────────────────────────────┐
│     Standard File Descriptors           │
├─────────┬───────────────────────────────┤
│   FD    │         Purpose               │
├─────────┼───────────────────────────────┤
│    0    │  STDIN  (standard input)      │
│    1    │  STDOUT (standard output)     │
│    2    │  STDERR (standard error)      │
└─────────┴───────────────────────────────┘
```

**Say:**
> "Every process starts with three file descriptors: 0 for input, 1 for output, and 2 for errors. When we create a pipe, we'll manipulate these to connect processes."

---

### What is a Pipe?

**Draw this diagram:**

```
┌──────────────────────────────────────────┐
│              PIPE                        │
│                                          │
│   Write End  ──────────▶  Read End      │
│   (pipefd[1])            (pipefd[0])    │
│                                          │
│   Data flows one direction only ───▶    │
└──────────────────────────────────────────┘
```

**Say:**
> "A pipe is a one-way communication channel. Data written to the write end can be read from the read end. The pipe() system call creates both ends and gives us two file descriptors."

---

## Step-by-Step Process

### Overview

**Draw this high-level flow:**

```
┌─────────────────────────────────────────────────────┐
│                 PARENT PROCESS                      │
│                  (minishell)                        │
│                                                     │
│  1. Create pipe                                     │
│  2. Fork for "ls"                                   │
│  3. Fork for "grep .c"                              │
│  4. Wait for both children                          │
└─────────────────────────────────────────────────────┘
         │                    │
         ▼                    ▼
    ┌─────────┐          ┌──────────┐
    │   ls    │  ──────▶ │ grep .c  │
    │ (child1)│   pipe   │ (child2) │
    └─────────┘          └──────────┘
```

---

### STEP 0: Initial State

**Draw this:**

```
┌──────────────────────────────────────┐
│        PARENT PROCESS                │
│         (minishell)                  │
│                                      │
│  FD 0: stdin  ──▶ terminal          │
│  FD 1: stdout ──▶ terminal          │
│  FD 2: stderr ──▶ terminal          │
└──────────────────────────────────────┘
```

**Say:**
> "We start with the parent process (our minishell) with standard file descriptors pointing to the terminal."

---

### STEP 1: Create Pipe

**Code:**
```c
int pipefd[2];
pipe(pipefd);  // Creates the pipe
```

**Draw this:**

```
┌──────────────────────────────────────────────────┐
│            PARENT PROCESS                        │
│                                                  │
│  FD 0: stdin  ──▶ terminal                      │
│  FD 1: stdout ──▶ terminal                      │
│  FD 2: stderr ──▶ terminal                      │
│  FD 3: pipefd[0] (READ END)  ◀──┐               │
│  FD 4: pipefd[1] (WRITE END) ───┼──▶ [PIPE]    │
│                                  └───────────    │
└──────────────────────────────────────────────────┘

pipefd[0] = 3  (read end)
pipefd[1] = 4  (write end)
```

**Say:**
> "After calling pipe(), we have two new file descriptors. FD 3 is the read end, FD 4 is the write end. The actual FD numbers may vary, but the concept is the same. These are the lowest available FDs."

**Key Point:**
> "The pipe is just a kernel buffer. Data written to FD 4 can be read from FD 3."

---

### STEP 2: Fork First Child (ls)

**Code:**
```c
pid_t pid1 = fork();
if (pid1 == 0) {
    // Child 1 code
}
```

**Draw this:**

```
                    AFTER FORK
                        │
        ┌───────────────┴───────────────┐
        ▼                               ▼
┌──────────────────┐          ┌──────────────────┐
│  PARENT PROCESS  │          │  CHILD 1 (ls)    │
│                  │          │                  │
│  FD 0: stdin     │          │  FD 0: stdin     │
│  FD 1: stdout    │          │  FD 1: stdout    │
│  FD 2: stderr    │          │  FD 2: stderr    │
│  FD 3: pipe READ │          │  FD 3: pipe READ │
│  FD 4: pipe WRITE│          │  FD 4: pipe WRITE│
└──────────────────┘          └──────────────────┘
```

**Say:**
> "After fork(), we have two processes. The child is an exact copy, so it has the same file descriptors pointing to the same pipe."

---

### STEP 3: Child 1 Setup (ls)

**What we want:**
- ls should write to the pipe (not terminal)
- ls doesn't need to read from pipe

**Code:**
```c
// In child 1
close(pipefd[0]);              // Close read end (don't need it)
dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to write end
close(pipefd[1]);              // Close original write end
```

**Draw this step-by-step:**

**3a. Close read end:**
```
┌──────────────────────────────┐
│     CHILD 1 (ls)             │
│                              │
│  FD 0: stdin                 │
│  FD 1: stdout ──▶ terminal  │
│  FD 2: stderr                │
│  FD 3: [CLOSED]              │ ◀── close(pipefd[0])
│  FD 4: pipe WRITE ──▶ [PIPE]│
└──────────────────────────────┘
```

**3b. dup2(pipefd[1], STDOUT_FILENO):**
```
┌──────────────────────────────┐
│     CHILD 1 (ls)             │
│                              │
│  FD 0: stdin                 │
│  FD 1: pipe WRITE ──▶ [PIPE]│ ◀── dup2(4, 1)
│  FD 2: stderr                │
│  FD 3: [CLOSED]              │
│  FD 4: pipe WRITE ──▶ [PIPE]│
└──────────────────────────────┘
```

**Say:**
> "dup2 copies FD 4 to FD 1. Now both FD 1 and FD 4 point to the write end of the pipe. Anything written to stdout (FD 1) goes to the pipe."

**3c. close(pipefd[1]):**
```
┌──────────────────────────────┐
│     CHILD 1 (ls)             │
│                              │
│  FD 0: stdin                 │
│  FD 1: pipe WRITE ──▶ [PIPE]│
│  FD 2: stderr                │
│  FD 3: [CLOSED]              │
│  FD 4: [CLOSED]              │ ◀── close(pipefd[1])
└──────────────────────────────┘
```

**Say:**
> "We close FD 4 because we already have FD 1 pointing to the pipe. We don't need two file descriptors to the same place."

---

### STEP 4: Child 1 Executes

**Code:**
```c
execve("/bin/ls", args, envp);
```

**Draw this:**

```
┌──────────────────────────────┐
│     CHILD 1 (ls)             │
│                              │
│  FD 0: stdin  ──▶ terminal  │
│  FD 1: stdout ──▶ [PIPE] ───┼──▶ Output goes to pipe
│  FD 2: stderr ──▶ terminal  │
└──────────────────────────────┘

ls executes and writes to stdout (FD 1)
  ↓
Output goes to PIPE (not terminal)
```

**Say:**
> "When ls runs, it writes its output to stdout. But stdout is now the pipe, so the output goes into the pipe buffer, not the terminal."

---

### STEP 5: Parent Closes Write End

**Code:**
```c
// In parent, after forking child 1
close(pipefd[1]);  // Close write end
```

**Draw this:**

```
┌──────────────────────────────┐
│     PARENT PROCESS           │
│                              │
│  FD 0: stdin                 │
│  FD 1: stdout                │
│  FD 2: stderr                │
│  FD 3: pipe READ ◀─ [PIPE]  │
│  FD 4: [CLOSED]              │ ◀── close(pipefd[1])
└──────────────────────────────┘
```

**Say:**
> "The parent closes the write end because it won't write to the pipe. This is crucial: if the parent keeps the write end open, the pipe will never get EOF."

**Important:**
> "When all write ends are closed, readers get EOF. If parent keeps write end open, grep would wait forever."

---

### STEP 6: Fork Second Child (grep)

**Code:**
```c
pid_t pid2 = fork();
if (pid2 == 0) {
    // Child 2 code
}
```

**Draw this:**

```
                    AFTER FORK
                        │
        ┌───────────────┴───────────────┐
        ▼                               ▼
┌──────────────────┐          ┌──────────────────┐
│  PARENT PROCESS  │          │  CHILD 2 (grep)  │
│                  │          │                  │
│  FD 0: stdin     │          │  FD 0: stdin     │
│  FD 1: stdout    │          │  FD 1: stdout    │
│  FD 2: stderr    │          │  FD 2: stderr    │
│  FD 3: pipe READ │          │  FD 3: pipe READ │
│  FD 4: [CLOSED]  │          │  FD 4: [CLOSED]  │
└──────────────────┘          └──────────────────┘
```

**Say:**
> "We fork again for grep. The child inherits the parent's file descriptors, including the pipe read end."

---

### STEP 7: Child 2 Setup (grep)

**What we want:**
- grep should read from the pipe (not terminal)
- grep should write to terminal (normal stdout)

**Code:**
```c
// In child 2
dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to read end
close(pipefd[0]);               // Close original read end
```

**Draw this step-by-step:**

**7a. dup2(pipefd[0], STDIN_FILENO):**
```
┌──────────────────────────────┐
│     CHILD 2 (grep)           │
│                              │
│  FD 0: pipe READ ◀─ [PIPE]  │ ◀── dup2(3, 0)
│  FD 1: stdout ──▶ terminal  │
│  FD 2: stderr ──▶ terminal  │
│  FD 3: pipe READ ◀─ [PIPE]  │
│  FD 4: [CLOSED]              │
└──────────────────────────────┘
```

**Say:**
> "dup2 copies FD 3 to FD 0. Now stdin (FD 0) reads from the pipe. grep will read from the pipe instead of the terminal."

**7b. close(pipefd[0]):**
```
┌──────────────────────────────┐
│     CHILD 2 (grep)           │
│                              │
│  FD 0: pipe READ ◀─ [PIPE]  │
│  FD 1: stdout ──▶ terminal  │
│  FD 2: stderr ──▶ terminal  │
│  FD 3: [CLOSED]              │ ◀── close(pipefd[0])
│  FD 4: [CLOSED]              │
└──────────────────────────────┘
```

**Say:**
> "We close FD 3 because FD 0 already points to the pipe. Again, we don't need duplicate file descriptors."

---

### STEP 8: Child 2 Executes

**Code:**
```c
execve("/usr/bin/grep", args, envp);
```

**Draw this:**

```
┌──────────────────────────────┐
│     CHILD 2 (grep)           │
│                              │
│  FD 0: stdin  ◀─ [PIPE]     │ ◀── Reads from pipe
│  FD 1: stdout ──▶ terminal  │ ──▶ Writes to terminal
│  FD 2: stderr ──▶ terminal  │
└──────────────────────────────┘

grep reads from stdin (FD 0)
  ↓
Input comes from PIPE (ls output)
  ↓
grep filters lines containing ".c"
  ↓
Output goes to stdout (terminal)
```

**Say:**
> "When grep runs, it reads from stdin, which is now the pipe. It gets the output from ls, filters for lines containing '.c', and writes to stdout, which is still the terminal."

---

### STEP 9: Parent Closes Read End

**Code:**
```c
// In parent, after forking child 2
close(pipefd[0]);  // Close read end
```

**Draw this:**

```
┌──────────────────────────────┐
│     PARENT PROCESS           │
│                              │
│  FD 0: stdin                 │
│  FD 1: stdout                │
│  FD 2: stderr                │
│  FD 3: [CLOSED]              │ ◀── close(pipefd[0])
│  FD 4: [CLOSED]              │
└──────────────────────────────┘
```

**Say:**
> "The parent closes the read end because it won't read from the pipe. Now the parent has no pipe file descriptors open."

---

### STEP 10: Parent Waits

**Code:**
```c
waitpid(pid1, &status1, 0);
waitpid(pid2, &status2, 0);
```

**Say:**
> "The parent waits for both children to complete. This prevents zombie processes and allows us to get exit codes."

---

## Visual Diagrams

### Complete Data Flow

**Draw this final diagram:**

```
┌─────────────────────────────────────────────────────────────┐
│                    COMPLETE DATA FLOW                       │
│                                                             │
│  ┌──────────┐                              ┌─────────────┐ │
│  │    ls    │                              │  grep .c    │ │
│  │          │                              │             │ │
│  │  FD 0: ──┼──▶ terminal (not used)      │  FD 0: ◀────┼─┼─┐
│  │  FD 1: ──┼──┐                           │  FD 1: ─────┼─┼─┼──▶ terminal
│  │  FD 2: ──┼──┼──▶ terminal (errors)     │  FD 2: ─────┼─┼─┼──▶ terminal
│  └──────────┘  │                           └─────────────┘ │ │
│                │                                           │ │
│                │         ┌──────────────┐                  │ │
│                └────────▶│     PIPE     │──────────────────┘ │
│                          │    BUFFER    │                    │
│                          └──────────────┘                    │
│                                                              │
│  Data Flow:                                                  │
│  ls output ──▶ pipe ──▶ grep input ──▶ terminal            │
└─────────────────────────────────────────────────────────────┘
```

---

### File Descriptor Timeline

**Draw this timeline:**

```
TIME ────────────────────────────────────────────────────────▶

PARENT:
  │
  ├─ pipe() ──────────────── FD 3, 4 created
  │
  ├─ fork() ──────────────── Child 1 created
  │
  ├─ close(pipefd[1]) ───── FD 4 closed
  │
  ├─ fork() ──────────────── Child 2 created
  │
  ├─ close(pipefd[0]) ───── FD 3 closed
  │
  ├─ waitpid(pid1) ──────── Wait for ls
  │
  └─ waitpid(pid2) ──────── Wait for grep


CHILD 1 (ls):
  │
  ├─ close(pipefd[0]) ───── FD 3 closed
  │
  ├─ dup2(pipefd[1], 1) ─── FD 1 → pipe write
  │
  ├─ close(pipefd[1]) ───── FD 4 closed
  │
  └─ execve("/bin/ls") ──── ls runs, writes to FD 1 (pipe)


CHILD 2 (grep):
  │
  ├─ dup2(pipefd[0], 0) ─── FD 0 → pipe read
  │
  ├─ close(pipefd[0]) ───── FD 3 closed
  │
  └─ execve("/bin/grep") ── grep runs, reads from FD 0 (pipe)
```

---

## Code Walkthrough

### Complete Implementation

**Show this code on whiteboard or screen:**

```c
void execute_pipeline(t_cmd *cmd1, t_cmd *cmd2)
{
    int pipefd[2];
    pid_t pid1, pid2;
    int status;

    // Step 1: Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    // Step 2: Fork first child (ls)
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        return;
    }

    if (pid1 == 0) {
        // CHILD 1: ls
        
        // Step 3: Setup file descriptors
        close(pipefd[0]);              // Close read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[1]);              // Close original write end

        // Step 4: Execute
        execve("/bin/ls", cmd1->args, envp);
        perror("execve");
        exit(127);
    }

    // PARENT continues here

    // Step 5: Close write end in parent
    close(pipefd[1]);

    // Step 6: Fork second child (grep)
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        return;
    }

    if (pid2 == 0) {
        // CHILD 2: grep

        // Step 7: Setup file descriptors
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to pipe
        close(pipefd[0]);               // Close original read end

        // Step 8: Execute
        execve("/usr/bin/grep", cmd2->args, envp);
        perror("execve");
        exit(127);
    }

    // PARENT continues here

    // Step 9: Close read end in parent
    close(pipefd[0]);

    // Step 10: Wait for both children
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
}
```

---

## Common Questions

### Q1: Why close pipe fds?

**Answer:**

> "We close pipe file descriptors for three reasons:
>
> **1. EOF Detection:**
> - When all write ends are closed, readers get EOF
> - If parent keeps write end open, grep waits forever
>
> **2. Resource Management:**
> - Each process has a limit on open fds (usually 1024)
> - Unclosed fds accumulate and can exhaust the limit
>
> **3. Clean Design:**
> - Each process should only have the fds it needs
> - Prevents accidental writes/reads"

**Draw this:**

```
If parent doesn't close write end:

Parent: FD 4 (write) ──┐
                       ├──▶ [PIPE] ──▶ grep (waiting for EOF)
Child1: FD 1 (write) ──┘

grep never gets EOF because write end is still open!
```

---

### Q2: Why dup2 instead of just using the pipe fd?

**Answer:**

> "We use dup2 because:
>
> **1. Standard Convention:**
> - Programs expect stdin=0, stdout=1, stderr=2
> - ls writes to stdout (FD 1), not some random FD
>
> **2. Compatibility:**
> - We don't control the program's code
> - ls will always write to FD 1
> - We need FD 1 to point to the pipe
>
> **3. Simplicity:**
> - Easier than modifying every program
> - Standard Unix pattern"

---

### Q3: What happens if we don't close the original fd after dup2?

**Answer:**

> "It's not wrong, but it's wasteful:
>
> **Without close:**
> ```
> FD 1: pipe write
> FD 4: pipe write  ← Same pipe, unnecessary
> ```
>
> **With close:**
> ```
> FD 1: pipe write
> FD 4: [CLOSED]
> ```
>
> **Why close:**
> - Saves file descriptor slot
> - Cleaner design
> - Prevents accidental use
> - Good practice"

---

### Q4: What if we dup2 in wrong order?

**Answer:**

> "Order matters! Wrong order can overwrite needed fds.
>
> **Correct:**
> ```c
> dup2(pipefd[1], STDOUT_FILENO);  // Copy pipe to stdout
> close(pipefd[1]);                 // Close original
> ```
>
> **Wrong:**
> ```c
> close(pipefd[1]);                 // ❌ Close first
> dup2(pipefd[1], STDOUT_FILENO);  // ❌ Copying closed fd!
> ```
>
> **Rule:** Always dup2 first, then close."

---

### Q5: Can both children run simultaneously?

**Answer:**

> "Yes! That's the beauty of pipes.
>
> **Parallel Execution:**
> - ls starts writing to pipe
> - grep starts reading from pipe
> - Both run at the same time
> - Pipe buffer holds data temporarily
>
> **Synchronization:**
> - If pipe buffer fills, ls blocks until grep reads
> - If pipe is empty, grep blocks until ls writes
> - Kernel handles this automatically
>
> **Efficiency:**
> - No need to wait for ls to finish
> - Data flows as it's produced
> - Faster than sequential execution"

**Draw this:**

```
Time ────────────────────────────────────▶

ls:    ████████████░░░░░░░░
       (running)   (waiting for grep)

grep:  ░░░░████████████████
       (waiting) (running)

Pipe:  [data][data][data]...
       ↑ ls writes
                    ↓ grep reads
```

---

## Presentation Script

### Opening

**Say:**

> "Let me explain how file descriptors are managed when executing a piped command. I'll use `ls | grep .c` as an example.
>
> [Draw standard fd table]
>
> First, let's review: every process has three standard file descriptors. FD 0 for input, FD 1 for output, FD 2 for errors."

---

### Step 1: Create Pipe

**Say:**

> "Step 1: We create a pipe using the pipe() system call.
>
> [Draw pipe diagram]
>
> This gives us two new file descriptors. Let's say FD 3 is the read end and FD 4 is the write end. The pipe is a one-way channel in the kernel."

---

### Step 2-4: First Child

**Say:**

> "Step 2: We fork to create the first child for 'ls'.
>
> [Draw fork diagram]
>
> After fork, both parent and child have the same file descriptors.
>
> Step 3: In the child, we need to redirect stdout to the pipe.
>
> [Draw close, dup2, close sequence]
>
> First, close the read end - we don't need it. Then, dup2 copies the write end to stdout. Now anything written to stdout goes to the pipe. Finally, close the original write end.
>
> Step 4: Execute ls. It writes to stdout, which is now the pipe."

---

### Step 5: Parent Closes Write

**Say:**

> "Step 5: Back in the parent, we close the write end.
>
> [Draw parent closing write end]
>
> This is critical. If we don't close it, grep will never get EOF because the write end is still open in the parent."

---

### Step 6-8: Second Child

**Say:**

> "Step 6: We fork again for 'grep'.
>
> [Draw second fork]
>
> Step 7: In this child, we redirect stdin to the pipe.
>
> [Draw dup2 for stdin]
>
> dup2 copies the read end to stdin. Now grep reads from the pipe instead of the terminal. We close the original read end.
>
> Step 8: Execute grep. It reads from stdin (the pipe), filters for '.c', and writes to stdout (the terminal)."

---

### Step 9-10: Cleanup

**Say:**

> "Step 9: The parent closes the read end.
>
> [Draw parent closing read end]
>
> Now the parent has no pipe file descriptors open.
>
> Step 10: The parent waits for both children to complete.
>
> [Draw complete data flow]
>
> Here's the complete picture: ls writes to the pipe, grep reads from the pipe and writes to the terminal. The data flows through the pipe without ever touching disk."

---

### Closing

**Say:**

> "The key points are:
>
> 1. Create pipe before forking
> 2. Each child redirects appropriate fd using dup2
> 3. Close unused pipe ends in each process
> 4. Parent closes both ends after forking
> 5. Wait for children to prevent zombies
>
> This pattern works for any number of piped commands. For three commands, you'd create two pipes and three children, following the same principles."

---

## Quick Reference

### Key System Calls

```c
int pipe(int pipefd[2]);
// Creates pipe, returns read end in pipefd[0], write end in pipefd[1]

pid_t fork(void);
// Creates child process, returns 0 in child, child's PID in parent

int dup2(int oldfd, int newfd);
// Copies oldfd to newfd, closes newfd first if open

int close(int fd);
// Closes file descriptor

int execve(const char *path, char *const argv[], char *const envp[]);
// Replaces current process with new program

pid_t waitpid(pid_t pid, int *status, int options);
// Waits for child process to terminate
```

---

### Common Mistakes

**Mistake 1: Not closing pipe fds**
```c
// ❌ Wrong
dup2(pipefd[1], STDOUT_FILENO);
// Forgot to close pipefd[1]!

// ✅ Correct
dup2(pipefd[1], STDOUT_FILENO);
close(pipefd[1]);
```

**Mistake 2: Closing before dup2**
```c
// ❌ Wrong
close(pipefd[1]);
dup2(pipefd[1], STDOUT_FILENO);  // Copying closed fd!

// ✅ Correct
dup2(pipefd[1], STDOUT_FILENO);
close(pipefd[1]);
```

**Mistake 3: Parent not closing pipe ends**
```c
// ❌ Wrong
fork();
// Parent keeps both pipe ends open

// ✅ Correct
fork();
close(pipefd[1]);  // Close write end after first fork
fork();
close(pipefd[0]);  // Close read end after second fork
```

**Mistake 4: Closing wrong end**
```c
// ❌ Wrong (in ls child)
close(pipefd[1]);  // Closing write end we need!
dup2(pipefd[0], STDOUT_FILENO);

// ✅ Correct
close(pipefd[0]);  // Close read end (don't need)
dup2(pipefd[1], STDOUT_FILENO);
```

---

## Whiteboard Checklist

**Before presenting:**
- [ ] Clean whiteboard
- [ ] Multiple colored markers
- [ ] Practice drawing diagrams
- [ ] Know the sequence by heart

**During presentation:**
- [ ] Draw standard fd table first
- [ ] Draw pipe concept
- [ ] Show each step sequentially
- [ ] Use different colors for different processes
- [ ] Label all file descriptors clearly
- [ ] Show data flow with arrows
- [ ] Explain why each step is necessary

**Key diagrams to draw:**
1. Standard file descriptors (0, 1, 2)
2. Pipe structure
3. Process tree (parent, child1, child2)
4. File descriptor tables for each process
5. Complete data flow

---

## Time Management

**Total: 5-7 minutes**

- Introduction (30 sec)
- Pipe concept (30 sec)
- Step 1-4: First child (2 min)
- Step 5-8: Second child (2 min)
- Step 9-10: Cleanup (30 sec)
- Complete diagram (1 min)
- Questions (flexible)

---

## Final Tips

**Do:**
- ✅ Draw diagrams step by step
- ✅ Use consistent notation
- ✅ Explain why, not just what
- ✅ Show data flow clearly
- ✅ Emphasize closing fds
- ✅ Be confident

**Don't:**
- ❌ Rush through steps
- ❌ Skip the "why"
- ❌ Forget to label fds
- ❌ Make diagram too complex
- ❌ Assume knowledge

---

**You're ready to explain file descriptor management like a pro!** 🚀

---

**End of Whiteboard Guide**
