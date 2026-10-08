# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 4B — CRUD de Rutas y Vehículos
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 4C — CRUD de Pilotos y Tarifas (Pendiente de autorización)
- **Rama Git**: `feature/routes-and-vehicles`

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
  - Documentación en `docs/autenticacion.md`.

### Fase 4A — CRUD de Sucursales y Clientes
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Módulos `include/sucursales.h` e `include/clientes.h` con CRUD binario y validaciones.
  - Endpoints REST bajo `/v1/sucursales` y `/v1/clientes`.
  - Suite de pruebas unitarias en `tests/test_sucursales_clientes.cpp`.

### Fase 4B — CRUD de Rutas y Vehículos
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Implementación de la lógica de negocio y persistencia para Rutas en `include/rutas.h`.
    - Operaciones: Crear, Listar activas, Consultar por ID, Actualizar por ID y Baja lógica (`activo = false`).
    - Validaciones: Verificación de que las sucursales de origen y destino existan y estén activas en `data/sucursales.dat`, origen != destino, distanciaKm > 0, tarifaBase >= 0 y no duplicidad de trayecto.
  - Implementación de la lógica de negocio y persistencia para Vehículos en `include/vehiculos.h`.
    - Operaciones: Crear, Listar activos, Consultar por ID, Actualizar por ID y Baja lógica (`activo = false`).
    - Validaciones: Placa única, capacidad de peso > 0 kg, capacidad de volumen > 0 m3, y estado válido (`Disponible`, `En ruta`, `En taller`).
  - Implementación de endpoints REST en Crow (`main.cpp`):
    - `GET /v1/rutas` (200 OK)
    - `GET /v1/rutas/{id}` (200 OK / 404 Not Found)
    - `POST /v1/rutas` (201 Created / 404 Not Found / 409 Conflict / 400 Bad Request)
    - `PUT /v1/rutas/{id}` (200 OK / 404 Not Found / 409 Conflict / 400 Bad Request)
    - `DELETE /v1/rutas/{id}` (200 OK / 404 Not Found)
    - `GET /v1/vehiculos` (200 OK)
    - `GET /v1/vehiculos/{id}` (200 OK / 404 Not Found)
    - `POST /v1/vehiculos` (201 Created / 409 Conflict / 400 Bad Request)
    - `PUT /v1/vehiculos/{id}` (200 OK / 404 Not Found / 409 Conflict / 400 Bad Request)
    - `DELETE /v1/vehiculos/{id}` (200 OK / 404 Not Found)
  - Suite de pruebas unitarias en `tests/test_rutas_vehiculos.cpp`.
  - Documentación técnica actualizada en `docs/rutas-api.md`, `docs/arquitectura.md` y `docs/estado-proyecto.md`.
- **Pruebas Realizadas**:
  - Pruebas unitarias completas ejecutando `./test_rutas_vehiculos` (0 errores).
  - Peticiones HTTP reales ejecutadas mediante `curl` a todos los verbos HTTP (`GET`, `POST`, `PUT`, `DELETE`) para `/v1/rutas` y `/v1/vehiculos`.
  - Recompilación limpia verificada con `./build.sh`.
- **Decisiones Importantes**:
  - Las rutas mantienen una referencia relacional por ID con las sucursales y validan activamente que ambas sucursales existan antes de la creación o actualización.
  - Los datos de vehículo capturan capacidades físicas de peso y volumen que posteriormente serán consumidas por las operaciones de despacho en la Fase 5.
- **Problemas Pendientes**:
  - Ninguno. La Fase 4B está 100% probada y funcional.

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
| **Fase 4C** | CRUD de Pilotos y Tarifas | Pendiente |
| **Fase 5** | Transacciones (Guías, Despacho, validaciones de peso/volumen) | Pendiente |
| **Fase 6** | Reportes (Rastreo, Manifiestos, Liquidación) | Pendiente |
| **Fase 7** | API completa e integración con React | Pendiente |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |
