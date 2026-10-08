# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 5B — Cálculo de peso volumétrico y costos
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 5C — Despacho y Operaciones (Pendiente de autorización)

---

## 1. Bitácora de Fases

### Fase 0 — Análisis y Configuración
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Análisis del entorno operativo (macOS Darwin 25.6.0 x86_64, zsh).
  - Identificación del compilador C++ (`Apple clang version 17.0.0`).
  - Verificación del soporte C++17 (`-std=c++17`).
  - Definición del método de compilación sin CMake mediante `build.sh`.

### Fase 1 — Base del Proyecto
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Script ejecutable `build.sh` para compilar con C++17 sin CMake.
  - Punto de entrada `main.cpp` con servidor Crow en `127.0.0.1:18080`.
  - Rutas iniciales HTTP (`GET /` y `GET /v1/health`).

### Fase 2 — Estructuras y Persistencia
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Diseño de 8 estructuras de datos de tamaño fijo en `include/structs.h`.
  - Motor genérico de persistencia binaria en `include/persistence.h` usando C stdio.
  - Suite de pruebas unitarias en `tests/test_persistence.cpp`.

### Fase 3 — Autenticación y Usuarios
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Módulo `include/auth.h` con funciones de autenticación y roles.
  - Endpoints `POST /v1/auth/login` y `POST /v1/auth/usuarios`.
  - Suite de pruebas unitarias en `tests/test_auth.cpp`.

### Fase 4A — CRUD de Sucursales y Clientes
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Módulos `include/sucursales.h` e `include/clientes.h` con CRUD binario.
  - Endpoints REST bajo `/v1/sucursales` y `/v1/clientes`.
  - Suite de pruebas unitarias en `tests/test_sucursales_clientes.cpp`.

### Fase 4B — CRUD de Rutas y Vehículos
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Módulos `include/rutas.h` e `include/vehiculos.h` con CRUD binario y validaciones relacionales.
  - Endpoints REST bajo `/v1/rutas` y `/v1/vehiculos`.
  - Suite de pruebas unitarias en `tests/test_rutas_vehiculos.cpp`.

### Fase 4C — CRUD de Pilotos y Tarifas
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Implementación de la lógica de negocio y persistencia para Pilotos en `include/pilotos.h`.
  - Implementación de la lógica de negocio y persistencia para Tarifas en `include/tarifas.h`.
  - Implementación de endpoints REST en Crow (`main.cpp`).
  - Suite de pruebas unitarias en `tests/test_pilotos_tarifas.cpp`.

### Fase 5A — Emisión de Guías
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Implementación de la entidad `Guia` en `include/structs.h` y persistencia binaria en `data/guias.dat`.
  - Lógica de negocio en `include/transacciones.h` con numeración correlativa (1001+) y validaciones relacionales.
  - Endpoints REST HTTP bajo `/v1/transacciones/guias`.
  - Suite de pruebas unitarias en `tests/test_guias.cpp`.

### Fase 5B — Cálculo de Peso Volumétrico y Costos
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Implementación del motor de cálculo matemático en `include/transacciones.h`:
    - Peso volumétrico: `(largo * ancho * alto) / 5000.0`.
    - Peso facturable: `max(pesoReal, pesoVolumetrico)`.
    - Volumen en m3: `(largo * ancho * alto) / 1000000.0`.
    - Costo total: `tarifaBaseRuta + (pesoFacturable * precioPorKg) + (volumenM3 * precioPorM3) + recargoEspecial`.
  - Integración de los campos calculados (`pesoVolumetricoKg`, `pesoFacturableKg`, `costoTotal`) en `struct Guia` y su persistencia binaria en `data/guias.dat`.
  - Actualización de los endpoints REST en Crow (`main.cpp`):
    - `POST /v1/transacciones/guias` (retorna guía creada con campos calculados).
    - `GET /v1/transacciones/guias` y `GET /v1/transacciones/guias/{numeroGuia}` (retornan lista/guía con campos calculados).
  - Suite de pruebas unitarias en `tests/test_calculos.cpp` (`test_calculos`).
  - Documentación técnica creada y actualizada en `docs/calculos-guias.md`, `docs/guias.md`, `docs/rutas-api.md`, `docs/arquitectura.md` y `docs/estado-proyecto.md`.
- **Pruebas Realizadas**:
  - Pruebas unitarias ejecutadas con `./test_calculos` (0 errores).
  - Verificación de fórmulas matemáticas directas, comparaciones peso real vs volumétrico y cálculo de costo total con tarifa.
  - Recompilación limpia verificada con `./build.sh`.
- **Decisiones Importantes**:
  - Se mantuvo compatibilidad binaria estricta en `struct Guia` sin alterar el diseño de tamaño fijo.
  - Se mantuvo fuera del alcance de la Fase 5B el despacho, asignación de vehículos, validación de capacidad de transporte y reportes según las instrucciones explícitas.
- **Problemas Pendientes**:
  - Ninguno. El cálculo de peso volumétrico y costo total está 100% probado y funcional.

---

## 2. Mapa de Fases del Proyecto

| Fase | Descripción | Estado |
| :--- | :--- | :--- |
| **Fase 0** | Análisis y configuración | **Completada** |
| **Fase 1** | Base del proyecto (`main.cpp`, `build.sh`, verificación de servidor) | **Completada** |
| **Fase 2** | Estructuras y persistencia (`structs.h`, `persistence.h`, `.dat`) | **Completada** |
| **Fase 3** | Autenticación y usuarios (`auth.h`, `POST /v1/auth/login`, `POST /v1/auth/usuarios`) | **Completada** |
| **Fase 4A** | CRUD de Sucursales y Clientes (`include/sucursales.h`, `include/clientes.h`, API REST) | **Completada** |
| **Fase 4B** | CRUD de Rutas y Vehículos (`include/rutas.h`, `include/vehiculos.h`, API REST) | **Completada** |
| **Fase 4C** | CRUD de Pilotos y Tarifas (`include/pilotos.h`, `include/tarifas.h`, API REST) | **Completada** |
| **Fase 5A** | Emisión y consulta de Guías (`include/transacciones.h`, `guias.dat`, API REST) | **Completada** |
| **Fase 5B** | Cálculo de peso volumétrico y costos (fórmulas, peso facturable, costo total) | **Completada** |
| **Fase 5C** | Despacho y Operaciones (Asignación de vehículos, capacidad, estado) | Pendiente |
| **Fase 6** | Reportes (Rastreo, Manifiestos, Liquidación) | Pendiente |
| **Fase 7** | API completa e integración con React | Pendiente |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |


