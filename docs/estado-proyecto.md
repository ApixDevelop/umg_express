# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 4C — CRUD de Pilotos y Tarifas (Mantenimientos completados)
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 5 — Transacciones (Guías y Despacho) (Pendiente de autorización)

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
    - Operaciones: Crear, Listar activos, Consultar por ID, Actualizar por ID y Baja lógica (`activo = false`).
    - Validaciones: Campos requeridos, límites de longitud y número de licencia único.
  - Implementación de la lógica de negocio y persistencia para Tarifas en `include/tarifas.h`.
    - Operaciones: Crear, Listar activas, Consultar por ID, Actualizar por ID y Baja lógica (`activo = false`).
    - Validaciones: Tipo de carga único, precios por kg y por m3 no negativos, y recargos especiales no negativos.
  - Implementación de endpoints REST en Crow (`main.cpp`):
    - `GET /v1/pilotos` (200 OK)
    - `GET /v1/pilotos/{id}` (200 OK / 404 Not Found)
    - `POST /v1/pilotos` (201 Created / 409 Conflict / 400 Bad Request)
    - `PUT /v1/pilotos/{id}` (200 OK / 404 Not Found / 409 Conflict)
    - `DELETE /v1/pilotos/{id}` (200 OK / 404 Not Found)
    - `GET /v1/tarifas` (200 OK)
    - `GET /v1/tarifas/{id}` (200 OK / 404 Not Found)
    - `POST /v1/tarifas` (201 Created / 409 Conflict / 400 Bad Request)
    - `PUT /v1/tarifas/{id}` (200 OK / 404 Not Found / 409 Conflict)
    - `DELETE /v1/tarifas/{id}` (200 OK / 404 Not Found)
  - Suite de pruebas unitarias en `tests/test_pilotos_tarifas.cpp`.
  - Documentación técnica actualizada en `docs/rutas-api.md`, `docs/arquitectura.md` y `docs/estado-proyecto.md`.
- **Pruebas Realizadas**:
  - Pruebas unitarias completas ejecutando `./test_pilotos_tarifas` (0 errores).
  - Peticiones HTTP reales ejecutadas mediante `curl` a todos los verbos HTTP (`GET`, `POST`, `PUT`, `DELETE`) para `/v1/pilotos` y `/v1/tarifas`.
  - Recompilación limpia verificada con `./build.sh`.
- **Decisiones Importantes**:
  - Tarifas parametrizadas por `tipoCarga` con campos explícitos para precio por peso (`precioPorKg`), precio por volumen (`precioPorM3`) y `recargoEspecial` preparadas para el consumo inmediato en el cálculo de costos de guías de la Fase 5.
- **Problemas Pendientes**:
  - Ninguno. Todos los mantenimientos de catálogos (Fase 4 completa) están 100% probados y funcionales.

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
| **Fase 5** | Transacciones (Guías, Despacho, validaciones de peso/volumen) | Pendiente |
| **Fase 6** | Reportes (Rastreo, Manifiestos, Liquidación) | Pendiente |
| **Fase 7** | API completa e integración con React | Pendiente |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |
