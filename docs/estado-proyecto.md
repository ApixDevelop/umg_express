# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 0 — Análisis y configuración
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 1 — Base del proyecto (Pendiente de autorización)

---

## 1. Bitácora de Fases

### Fase 0 — Análisis y Configuración
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Análisis del entorno operativo (macOS Darwin 25.6.0 x86_64, zsh).
  - Identificación del compilador C++ (`Apple clang version 17.0.0`).
  - Verificación del soporte C++17 (`-std=c++17`).
  - Identificación y verificación de dependencias de Crow (`crow` v1.2.0 y `asio` ubicados en `/usr/local/include`).
  - Confirmación de la regla de NO usar CMake. Definición del método de compilación mediante script `build.sh`.
  - Prueba empírica de compilación y ejecución de un servidor mínimo C++17 con Crow sin dependencias externas complejas.
  - Corrección de referencias desactualizadas a CMake en la documentación (`docs/arquitectura.md`).
  - Propuesta de estructura modular inicial del proyecto (`main.cpp`, `build.sh`, `include/`, `data/`, `docs/`).
- **Pruebas Realizadas**:
  - Detección de versión del compilador: `clang++ --version` (exitoso).
  - Verificación de rutas de encabezados: `/usr/local/include/crow.h` y `/usr/local/include/asio.hpp` (exitoso).
  - Compilación de prueba C++17 con Crow: `clang++ -std=c++17 -I/usr/local/include ...` (código de salida 0).
  - Ejecución de ejecutable de prueba: salida `"Crow initialized successfully"` (exitoso).
- **Decisiones Importantes**:
  - Usar `clang++` con `-std=c++17` e `-I/usr/local/include`.
  - Crear un script `build.sh` en lugar de utilizar CMake o Makefiles complejos.
  - Utilizar directamente las librerías del sistema `/usr/local/include/crow.h` y `/usr/local/include/asio.hpp`.
  - Organizar la persistencia binaria con archivos `.dat` de tamaño fijo en el directorio `data/`.
- **Problemas Conocidos**:
  - El archivo `crow.h` en la raíz estaba vacío (0 bytes); se utilizará la cabecera instalada por Homebrew (`/usr/local/include/crow.h`).

---

## 2. Mapa de Fases del Proyecto

| Fase | Descripción | Estado |
| :--- | :--- | :--- |
| **Fase 0** | Análisis y configuración | **Completada** |
| **Fase 1** | Base del proyecto (`main.cpp`, `build.sh`, verificación de servidor) | Pendiente |
| **Fase 2** | Estructuras y persistencia (`structs.h`, operaciones `.dat`) | Pendiente |
| **Fase 3** | Autenticación (`usuarios.dat`, login, roles) | Pendiente |
| **Fase 4** | CRUD de catálogos (Sucursales, Clientes, Rutas, Vehículos, Pilotos, Tarifas) | Pendiente |
| **Fase 5** | Transacciones (Guías, Despacho, validaciones de peso/volumen) | Pendiente |
| **Fase 6** | Reportes (Rastreo, Manifiestos, Liquidación) | Pendiente |
| **Fase 7** | API completa e integración con React | Pendiente |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |
