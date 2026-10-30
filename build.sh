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
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_reporte_liquidacion.cpp -o test_reporte_liquidacion
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_auth.cpp -o test_auth
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_fase1_seguridad.cpp -o test_fase1_seguridad
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_integracion_api.cpp -o test_integracion_api
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_algebra.cpp -o test_algebra
clang++ -std=c++17 -I/usr/local/include -Iinclude tests/test_arbol_bst.cpp -o test_arbol_bst

echo "=== Compilación exitosa. Ejecutables creados: ./server, ./test_auth, ./test_fase1_seguridad, ./test_calculos, ./test_despacho, ./test_reporte_rastreo, ./test_reporte_manifiesto, ./test_reporte_liquidacion, ./test_integracion_api, ./test_algebra, ./test_arbol_bst ==="



