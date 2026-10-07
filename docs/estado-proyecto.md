# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 2 — Estructuras y persistencia
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 3 — Autenticación (Pendiente de autorización)
- **Rama Git**: `feature/structure-and-persistence`

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

### Fase 1 — Base del Proyecto
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Script ejecutable `build.sh` para compilar con C++17 sin CMake.
  - Punto de entrada `main.cpp` con servidor Crow en `127.0.0.1:18080`.
  - Rutas iniciales HTTP (`GET /` y `GET /v1/health`).

### Fase 2 — Estructuras y Persistencia
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Diseño de 8 estructuras de datos de tamaño fijo en `include/structs.h` (`Usuario`, `Sucursal`, `Cliente`, `Ruta`, `Vehiculo`, `Piloto`, `Tarifa`, `Guia`).
  - Implementación del motor genérico de persistencia binaria en `include/persistence.h` utilizando únicamente funciones de C stdio (`fopen`, `fclose`, `fread`, `fwrite`, `fseek`, `ftell`).
  - Implementación de operaciones CRUD en archivos `.dat`: creación, lectura secuencial, búsqueda por ID, actualización por desplazamiento directo con `fseek`, eliminación lógica (`activo = false`) y auto-incremento de IDs.
  - Creación de la suite de pruebas unitarias en `tests/test_persistence.cpp`.
  - Integración de la verificación e inicialización automática de los 8 archivos `.dat` al arrancar `main.cpp`.
  - Documentación completa en `docs/estructura-dat.md`, `docs/arquitectura.md` y `docs/estado-proyecto.md`.
- **Pruebas Realizadas**:
  - `sizeof()` constante verificado para las 8 estructuras.
  - Prueba de inserción y lectura binaria en disco.
  - Prueba de búsqueda por ID y posicionamiento exacto con `fseek`.
  - Prueba de actualización en disco y verificación de integridad.
  - Prueba de baja lógica (`activo = false`) y filtrado en consultas.
  - Prueba de persistencia tras cerrar y volver a abrir archivos `.dat`.
  - Prueba de integración del servidor HTTP iniciado con inicialización de persistencia.
  - Recompilación limpia verificada con `./build.sh`.
- **Decisiones Importantes**:
  - Usar cadenas de caracteres de tamaño fijo (`char[]`) en lugar de `std::string` para garantizar `sizeof()` constante y operaciones binarias inmediatas.
  - Emplear plantillas de C++ en `namespace Persistence` para ofrecer operaciones de archivo reutilizables sin duplicar código procedural.
  - Retener registros inactivos en el archivo binario (`activo = false`) para posibilitar auditorías y mantener consistencia de desplazamientos.
- **Problemas Pendientes**:
  - Ninguno. La capa de persistencia binaria está 100% probada y funcional.

---

## 2. Mapa de Fases del Proyecto

| Fase | Descripción | Estado |
| :--- | :--- | :--- |
| **Fase 0** | Análisis y configuración | **Completada** |
| **Fase 1** | Base del proyecto (`main.cpp`, `build.sh`, verificación de servidor) | **Completada** |
| **Fase 2** | Estructuras y persistencia (`structs.h`, `persistence.h`, `.dat`) | **Completada** |
| **Fase 3** | Autenticación (`usuarios.dat`, login, roles) | Pendiente |
| **Fase 4** | CRUD de catálogos (Sucursales, Clientes, Rutas, Vehículos, Pilotos, Tarifas) | Pendiente |
| **Fase 5** | Transacciones (Guías, Despacho, validaciones de peso/volumen) | Pendiente |
| **Fase 6** | Reportes (Rastreo, Manifiestos, Liquidación) | Pendiente |
| **Fase 7** | API completa e integración con React | Pendiente |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |
