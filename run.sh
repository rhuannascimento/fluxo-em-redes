#!/usr/bin/env zsh
set -euo pipefail

ROOT_DIR=$(cd -- "$(dirname "$0")" && pwd)
PROJECT_DIR="$ROOT_DIR/project"
INCLUDE_DIR="$PROJECT_DIR/include"
SRC_DIR="$PROJECT_DIR/src"
BIN_DIR="$ROOT_DIR/bin"
BIN="$BIN_DIR/bin_graph"

compile() {
  local CXX
  if command -v clang++ >/dev/null 2>&1; then
    CXX=clang++
  elif command -v g++ >/dev/null 2>&1; then
    CXX=g++
  else
    echo "No C++ compiler found (clang++/g++)." >&2
    exit 1
  fi

  "$CXX" -std=c++20 -O2 -Wall -Wextra -I "$INCLUDE_DIR" \
    "$SRC_DIR/graph.cpp" "$SRC_DIR/io.cpp" "$SRC_DIR/bellman_ford.cpp" "$SRC_DIR/main.cpp" -o "$BIN"
}

run() {
  if [[ ! -x "$BIN" ]]; then
    echo "Binary not found, compiling..."
    compile
  fi
  # repassa todos os argumentos ao binário (flags de instância)
  "$BIN" "$@"
}

case ${1:-all} in
  build)
    mkdir -p "$BIN_DIR" && compile ;;
  run)
    shift
    mkdir -p "$BIN_DIR" && run "$@" ;;
  all)
    shift
    mkdir -p "$BIN_DIR" && compile && run "$@" ;;
  plot)
    shift
    mkdir -p "$BIN_DIR"
    [[ -x "$BIN" ]] || compile
    DOT_PATH="$BIN_DIR/graph.dot"
    run "$@" --dot "$DOT_PATH"
    if command -v dot >/dev/null 2>&1; then
      PNG_PATH="${DOT_PATH%.dot}.png"
      dot -Tpng "$DOT_PATH" -o "$PNG_PATH"
      if command -v open >/dev/null 2>&1; then
        open "$PNG_PATH"
      else
        echo "PNG gerado: $PNG_PATH"
      fi
    else
      echo "Graphviz (dot) não encontrado. Instale com: brew install graphviz" >&2
    fi
    ;;
  *)
    echo "Usage: $0 [build|run|all|plot] [--input <path> --output <path> --source <s> --dot <path.dot>]" >&2
    exit 2 ;;
esac
