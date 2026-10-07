#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_DIR"

echo "=== Compilando UMG Express (C++17 / Crow) ==="
clang++ -std=c++17 -I/usr/local/include -Iinclude main.cpp -o server

echo "=== Compilación exitosa. Ejecutable creado: ./server ==="
