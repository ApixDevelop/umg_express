#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_DIR"

echo "=== Compilando UMG Express (C++17 / Crow) ==="
clang++ -std=c++17 -I/usr/local/include -Iinclude main.cpp -o server
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_calculos.cpp -o test_calculos
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_despacho.cpp -o test_despacho
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_reporte_rastreo.cpp -o test_reporte_rastreo
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_reporte_manifiesto.cpp -o test_reporte_manifiesto

echo "=== Compilación exitosa. Ejecutables creados: ./server, ./test_calculos, ./test_despacho, ./test_reporte_rastreo, ./test_reporte_manifiesto ==="



