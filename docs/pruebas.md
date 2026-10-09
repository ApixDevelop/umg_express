# Plan de Pruebas y Validación de Estabilidad — UMG Express (Fase 8)

## 1. Resumen Ejecutivo

La **Fase 8** de UMG Express estuvo dedicada exclusivamente a la validación exhaustiva, pruebas de regresión, detección y corrección de defectos reproducibles y estabilización global de la arquitectura del backend en C++17 con Crow y almacenamiento binario `.dat`.

Durante esta fase se verificó la compilación limpia del servidor y de los 6 binarios de suite de pruebas unitarias e integración end-to-end (E2E), logrando un **100% de tasa de éxito en la ejecución automatizada de pruebas**.

---

## 2. Entorno y Herramientas de Pruebas

* **Lenguaje:** C++17 (Apple Clang 16.0.0 / macOS arm64/x86_64)
* **Framework Web:** Crow HTTP Server (`127.0.0.1:18080`)
* **Persistencia:** Archivos binarios de registros de tamaño fijo (`.dat`) manipulados vía C/C++ `FILE*`, `fopen`, `fclose`, `fread`, `fwrite`, `fseek`
* **Sistema de Compilación:** Script Bash nativo `./build.sh` (sin CMake ni `CMakeLists.txt`)
* **Aislamiento de Datos:** Todos los conjuntos de pruebas utilizan archivos temporales aislados (`data/test_*.dat` o rutas de prueba específicas) y garantizan la eliminación limpia de archivos temporales al concluir, manteniendo intactos los datos de producción en `data/`.

---

## 3. Matriz de Pruebas Ejecutadas

| Módulo / Test Suite | Archivo de Prueba | Cobertura Funcional y Casos de Uso | Resultado |
| :--- | :--- | :--- | :--- |
| **Cálculos y Fórmulas** | `tests/test_calculos.cpp` | Fórmulas unitarias de peso volumétrico (`(L×W×H)/5000`), peso facturable (máx entre real y volumétrico), cálculo de costo según tarifa. Rechazo de dimensiones y pesos <= 0. | **PASÓ (100%)** |
| **Despacho y Capacidad** | `tests/test_despacho.cpp` | Asignación de guías a vehículos, control acumulativo de peso y volumen, rechazo de vehículos inactivos o en taller, rechazo por exceso de capacidad física, rechazo de re-despacho de guías ya despachadas. | **PASÓ (100%)** |
| **Reporte de Rastreo** | `tests/test_reporte_rastreo.cpp` | Consulta de guías en estado 'Creada' y 'Despachada', hidratación de relaciones (Cliente, Sucursal, Ruta, Vehículo, Piloto), rechazo de guías inexistentes o inactivas. | **PASÓ (100%)** |
| **Reporte de Manifiesto** | `tests/test_reporte_manifiesto.cpp` | Manifiesto de vehículos con múltiples guías asociadas, desduplicación de entradas, vehículos sin guías, vehículos inactivos o IDs inexistentes (-1, 9999). | **PASÓ (100%)** |
| **Reporte de Liquidación** | `tests/test_reporte_liquidacion.cpp` | Consolidación financiera por sucursales y estados ('Creada', 'Despachada'), reporte sin guías (valores en 0.0), tolerancia a punteros nulos de archivos. | **PASÓ (100%)** |
| **Integración End-to-End** | `tests/test_integracion_api.cpp` | Flujo E2E completo: Autenticación -> Catálogos (Sucursales, Clientes, Rutas, Vehículos, Pilotos, Tarifas) -> Emisión de Guías -> Despacho y Capacidad -> Módulos de Reportes -> Persistencia tras reinicio de servidor. | **PASÓ (100%)** |

---

## 4. Defectos Identificados y Corregidos en la Fase 8

### Defecto 1: Inclusión Duplicada e Incompatibilidad de Nombres en `include/reportes.h`
* **Comportamiento Incorrecto:** Las directivas `#include <set>` y `#include <algorithm>` estaban declaradas dos veces, incluyendo ocurrencias dentro de bloques parciales que provocaban advertencias en la resolución del espacio de nombres `Reportes`.
* **Causa Raíz:** Declaración redundante al final del bloque `#include` de cabeceras funcionales.
* **Corrección:** Se reordenaron las inclusiones estándar al inicio de `include/reportes.h` fuera de `namespace Reportes`.
* **Verificación:** Compilación limpia sin advertencias de colisión de directivas.

### Defecto 2: Llamada a Método Inexistente `guardarRegistro` en `test_reporte_manifiesto.cpp`
* **Comportamiento Incorrecto:** Error de compilación `no member named 'guardarRegistro' in namespace 'Persistence'`.
* **Causa Raíz:** En la prueba 6 de manifiestos se invocaba `Persistence::guardarRegistro<Despacho>()`, pero la plantilla de persistencia binaria expone `Persistence::insertar<T>()`.
* **Corrección:** Se sustituyó la llamada por `Persistence::insertar<Despacho>(FILE_DESPACHOS_TEST_6B, dDup)`.
* **Verificación:** Compilación limpia y ejecución exitosa de `test_reporte_manifiesto`.

### Defecto 3: Incompatibilidad de Comparación `std::strcmp` con `std::string` en `test_integracion_api.cpp`
* **Comportamiento Incorrecto:** Error de compilación `cannot convert 'std::string' to 'const char *'` en la aserción de `resDesp.estadoVehiculo`.
* **Causa Raíz:** `resDesp.estadoVehiculo` es un `std::string` en el DTO `ResultadoDespacho`, mientras `std::strcmp` requiere punteros `const char*`.
* **Corrección:** Se actualizó la aserción a la comparación directa `assert(resDesp.estadoVehiculo == "En ruta")`.
* **Verificación:** Compilación limpia y ejecución exitosa del módulo.

### Defecto 4: Descalce en Cadena de Validación de Mismo Cliente en `test_integracion_api.cpp`
* **Comportamiento Incorrecto:** Falla de aserción `assert(err.find("diferentes") != std::string::npos)` al intentar emitir una guía con remitente = destinatario.
* **Causa Raíz:** El mensaje de error estandarizado devuelto por `Guias::crearGuia` en `include/transacciones.h` es `"El cliente remitente y destinatario no pueden ser el mismo"`.
* **Corrección:** Se ajustó la aserción en `test_integracion_api.cpp` a `assert(err.find("mismo") != std::string::npos)`.
* **Verificación:** Ejecución exitosa de la prueba de integración.

---

## 5. Validación de Aislamiento de Datos

Durante toda la ejecución de pruebas de la Fase 8:
1. No se modificó ni eliminó ningún archivo activo en la carpeta `data/` de producción.
2. Cada archivo de prueba creó sus propios archivos `.dat` de prueba en rutas temporales aisladas.
3. Todas las funciones de cleanup (`limpiarArchivosTest...()`) se ejecutaron al finalizar las pruebas para dejar el repositorio en estado limpio.

---

## 6. Conclusiones y Estado del Sistema

El backend de UMG Express ha alcanzado el nivel deseado de **estabilidad y robustez funcional**. Todos los módulos (Autenticación, Catálogos, Transacciones, Despacho y Reportes) funcionan de forma integrada y consistente con el almacenamiento binario en disco.
