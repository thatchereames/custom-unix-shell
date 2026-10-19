# pish — UNIX Command Shell

A lightweight, reverse-engineered shell inspired by PowerShell workflows, built from scratch in C using low-level POSIX system calls. Supports interactive command execution, batch script processing, and session-persistent command history.

---

## 🚀 Key Features

- **Interactive & Batch Execution Modes**: Operates as an interactive REPL with a custom prompt or executes automated shell scripts sequentially.
- **Process Lifecycle Management**: Employs core UNIX system calls (`fork`, `execvp`, `wait`) for child process creation, program execution, and synchronization.
- **Command Parsing & Tokenization**: Custom parser for dynamic argument tokenization and execution vector (`argc`/`argv`) construction.
- **Built-in Commands**:
  - `cd <dir>`: Native directory navigation via `chdir()`.
  - `history`: Session-persistent command tracking stored at `~/.pish_history`.
  - `exit`: Graceful termination and resource cleanup.
- **Robust Error Handling**: Standardized error reporting for system call failures and invalid command syntax.

---

## 🛠️ Architecture & Systems Implementation

| Component | Implementation Details |
| :--- | :--- |
| **Language** | C (C99 standard, compiled with strict flags: `-Wall -Wextra`) |
| **System Calls** | `fork()`, `execvp()`, `wait()`, `chdir()`, `getpwuid()`, `getuid()` |
| **Parsing** | Whitespace tokenization into structured `pish_arg` vectors |
| **History Persistence** | File I/O streaming to user's home directory (`~/.pish_history`) |
| **Memory Management** | Dynamic heap allocation (`malloc`/`free`) per command cycle |

---

## ⚡ Quickstart

### Build
```bash
make
```

### Run Interactive Mode
```bash
./pish
```

### Run Batch / Script Mode
```bash
./pish script.sh
```

### Clean
```bash
make clean
```
