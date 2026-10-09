# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 5C — Despacho y validación de capacidad
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 6 — Reportes (Rastreo, Manifiestos, Liquidación) (Pendiente de autorización)

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
  - Motor de cálculo matemático en `include/transacciones.h` (peso volumétrico, peso facturable, costo total).
  - Integración de los campos calculados (`pesoVolumetricoKg`, `pesoFacturableKg`, `costoTotal`) en `Guia` y `data/guias.dat`.
  - Suite de pruebas unitarias en `tests/test_calculos.cpp`.

### Fase 5C — Despacho y Validación de Capacidad
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Implementación de la entidad `Despacho` en `include/structs.h` y persistencia binaria en `data/despachos.dat`.
  - Desarrollo del módulo `Despachos` en `include/transacciones.h`:
    - Control acumulativo de peso: $\sum \text{pesoFacturableKg} \le \text{capacidadPesoKg}$.
    - Control acumulativo de volumen: $\sum \text{volumenM3} \le \text{capacidadVolumenM3}$.
    - Bloqueo automático cuando se exceden los límites físicos del vehículo.
    - Rechazo de vehículos inactivos o en estado `"En taller"`.
    - Rechazo de guías inactivas, ya despachadas o entregadas.
    - Actualización automática de `Guia.estado` a `"Despachada"` y `Vehiculo.estado` a `"En ruta"`.
  - Implementación de los endpoints HTTP REST en Crow (`main.cpp`):
    - `POST /v1/transacciones/despacho` (201 Created / 400 Bad Request / 404 Not Found)
    - `GET /v1/transacciones/despacho` (200 OK)
  - Suite de pruebas unitarias en `tests/test_despacho.cpp` (`test_despacho`).
  - Documentación técnica creada y actualizada en `docs/despacho.md`, `docs/rutas-api.md`, `docs/arquitectura.md` y `docs/estado-proyecto.md`.
- **Pruebas Realizadas**:
  - Suite unitaria ejecutada mediante `./test_despacho` (0 errores).
  - Pruebas de despacho exitoso, rechazo por exceso de peso, rechazo por exceso de volumen, rechazo por vehículo en taller, rechazo de guía ya despachada y verificación de persistencia binaria en disco.
  - Recompilación limpia verificada con `./build.sh`.
- **Decisiones Importantes**:
  - Se mantuvo fuera del alcance de la Fase 5C el módulo de reportes (rastreo, manifiesto, liquidación) según las instrucciones explícitas.
- **Problemas Pendientes**:
  - Ninguno. El módulo de despacho y control de capacidad está 100% probado y funcional.

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
| **Fase 5C** | Despacho y Operaciones (Asignación de vehículos, capacidad, estado) | **Completada** |
| **Fase 6** | Reportes (Rastreo, Manifiestos, Liquidación) | Pendiente |
| **Fase 7** | API completa e integración con React | Pendiente |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |



