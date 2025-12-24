#!/usr/bin/env bash
# run_exitcode_suite.sh
#
# Runs the SAME input (commands + `echo $?` after each) through:
#   - bash (or any shell you pass)
#   - your minishell binary (run this script twice with different --shell)
#
# It does not hardcode or “decide” expected outputs. It only captures what the shell prints.
#
# Usage:
#   chmod +x run_exitcode_suite.sh
#   ./run_exitcode_suite.sh --shell /bin/bash --out bash_out.txt
#   ./run_exitcode_suite.sh --shell ./minishell --out mini_out.txt
#   diff -u bash_out.txt mini_out.txt

set -e

SHELL_BIN=""
OUT_FILE=""
WORKDIR="./_exitcode_suite"

usage() {
  cat <<'USAGE'
Usage:
  ./run_exitcode_suite.sh --shell /path/to/shell --out output.txt

Examples:
  ./run_exitcode_suite.sh --shell /bin/bash --out bash_out.txt
  ./run_exitcode_suite.sh --shell ./minishell --out mini_out.txt
USAGE
}

die() {
  echo "Error: $*" 1>&2
  exit 1
}

while [ $# -gt 0 ]; do
  if [ "$1" = "--shell" ]; then
    shift
    SHELL_BIN="${1:-}"
  elif [ "$1" = "--out" ]; then
    shift
    OUT_FILE="${1:-}"
  elif [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    usage
    exit 0
  else
    die "Unknown argument: $1"
  fi
  shift
done

if [ -z "$SHELL_BIN" ] || [ -z "$OUT_FILE" ]; then
  usage
  exit 1
fi

if [ ! -x "$SHELL_BIN" ]; then
  die "Shell not found or not executable: $SHELL_BIN"
fi

# (Re)create a controlled workspace so tests are reproducible
rm -rf "$WORKDIR"
mkdir -p "$WORKDIR/dir_ok"
mkdir -p "$WORKDIR/dircmd"
mkdir -p "$WORKDIR/home"

# Files for permission / redirection tests
printf "hello\n" > "$WORKDIR/input.txt"

cat > "$WORKDIR/ok_exec.sh" <<'EOF'
#!/bin/sh
echo OK_EXEC
exit 0
EOF
chmod 755 "$WORKDIR/ok_exec.sh"

cat > "$WORKDIR/no_exec.sh" <<'EOF'
#!/bin/sh
echo NO_EXEC
exit 0
EOF
chmod 644 "$WORKDIR/no_exec.sh"

echo "READONLY" > "$WORKDIR/readonly.txt"
chmod 444 "$WORKDIR/readonly.txt"

echo "SECRET" > "$WORKDIR/noperm.txt"
chmod 000 "$WORKDIR/noperm.txt"

# Build the input that will be fed to the target shell.
# Each test is: command; echo $?
# No expected outputs are hardcoded.
cat > "$WORKDIR/tests.in" <<'EOF'
cd _exitcode_suite

echo "=== basic success/fail ==="
/bin/echo hello ; echo $?
/usr/bin/true ; echo $?
/usr/bin/false ; echo $?

echo "=== command not found / missing path ==="
this_command_does_not_exist ; echo $?
./no_such_file ; echo $?

echo "=== execution / permission errors ==="
./ok_exec.sh ; echo $?
./no_exec.sh ; echo $?
./dircmd ; echo $?

echo "=== redirections / files ==="
/bin/cat < missing.txt ; echo $?
/bin/echo hi > readonly.txt ; echo $?
/bin/echo hi > dircmd ; echo $?
/bin/cat noperm.txt ; echo $?

echo "=== pipelines (status is last command in pipeline) ==="
/usr/bin/false | /usr/bin/true ; echo $?
/usr/bin/true | /usr/bin/false ; echo $?
this_command_does_not_exist | /bin/cat ; echo $?

echo "=== cd errors ==="
cd dir_ok ; echo $?
cd does_not_exist_dir ; echo $?

echo "=== export invalid identifier (bash prints error; exit code shown by echo $?) ==="
export 1A=2 ; echo $?

# Put syntax-error tests LAST because many shells stop processing further input after a syntax error.
echo "=== syntax errors (kept at end) ==="
echo hi | ; echo $?
echo hi && ; echo $?
echo hi > ; echo $?
echo "unterminated ; echo $?
EOF

# If running bash, force a clean startup (no user rc files)
SHELL_ARGS=()
base="$(basename "$SHELL_BIN")"
if [ "$base" = "bash" ]; then
  SHELL_ARGS+=(--noprofile --norc)
fi

# Run and capture stdout+stderr exactly as produced by the shell
# Note: minishell may print prompts even in non-interactive mode; that will be captured too.
env \
  LC_ALL=C LANG=C TERM=dumb \
  HOME="$WORKDIR/home" \
  PATH="/usr/bin:/bin:/usr/sbin:/sbin" \
  "$SHELL_BIN" "${SHELL_ARGS[@]}" < "$WORKDIR/tests.in" > "$OUT_FILE" 2>&1
