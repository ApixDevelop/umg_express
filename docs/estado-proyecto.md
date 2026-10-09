# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 7 — Integración y revisión general de la API
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 8 — Pruebas y estabilización (Pendiente de autorización)

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
  - Desarrollo del módulo `Despachos` en `include/transacciones.h` para control de peso/volumen y despacho.
  - Endpoints HTTP REST `POST /v1/transacciones/despacho` y `GET /v1/transacciones/despacho`.
  - Suite de pruebas unitarias en `tests/test_despacho.cpp`.

### Fase 6A — Reporte de Rastreo de Guías
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Módulo `Reportes::obtenerRastreoGuia` en `include/reportes.h`.
  - Endpoint HTTP REST `GET /v1/reportes/rastreo/{numeroGuia}`.
  - Suite de pruebas unitarias en `tests/test_reporte_rastreo.cpp`.

### Fase 6B — Reporte de Manifiesto de Vehículo
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Módulo `Reportes::obtenerManifiestoVehiculo` en `include/reportes.h`.
  - Endpoint HTTP REST `GET /v1/reportes/manifiesto/{idVehiculo}`.
  - Suite de pruebas unitarias en `tests/test_reporte_manifiesto.cpp`.

### Fase 6C — Reporte de Liquidación Financiera
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Módulo `Reportes::obtenerReporteLiquidacion` en `include/reportes.h`.
  - Endpoint HTTP REST `GET /v1/reportes/liquidacion`.
  - Suite de pruebas unitarias en `tests/test_reporte_liquidacion.cpp`.

### Fase 7 — Integración y Revisión General de la API
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Revisión exhaustiva de los 42 endpoints REST y controladores HTTP en `main.cpp`.
  - Verificación del cumplimiento estricto de códigos de respuesta HTTP (`200 OK`, `201 Created`, `400 Bad Request`, `401 Unauthorized`, `404 Not Found`, `409 Conflict`).
  - Verificación del flujo de integración de extremo a extremo: Autenticación -> Catálogos -> Emisión de Guías -> Cálculos de Peso/Costo -> Despacho y Límites de Capacidad -> Reportes (Rastreo, Manifiesto, Liquidación) -> Persistencia en Disco.
  - Implementación de la suite de pruebas de integración e-2-e en `tests/test_integracion_api.cpp` (`test_integracion_api`).
  - Actualización de la documentación viva del proyecto en `docs/rutas-api.md`, `docs/arquitectura.md`, `docs/reglas-negocio.md` y `docs/estado-proyecto.md`.
- **Pruebas Realizadas**:
  - Ejecución limpia de todas las suites unitarias e integración en el sistema mediante `./build.sh && ./test_calculos && ./test_despacho && ./test_reporte_rastreo && ./test_reporte_manifiesto && ./test_reporte_liquidacion && ./test_integracion_api` (0 errores).
  - Verificación de persistencia de datos tras simulaciones de reinicio del servidor.
- **Decisiones Importantes**:
  - La API mantiene un contrato JSON unificado (`{ "success": bool, ... }`) en todos sus endpoints.
- **Problemas Pendientes**:
  - Ninguno. El backend está totalmente integrado y probado.

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
| **Fase 6A** | Reporte de Rastreo de Guías (`GET /v1/reportes/rastreo/{numeroGuia}`) | **Completada** |
| **Fase 6B** | Reporte de Manifiesto por Vehículo (`GET /v1/reportes/manifiesto/{idVehiculo}`) | **Completada** |
| **Fase 6C** | Reporte de Liquidación (`GET /v1/reportes/liquidacion`) | **Completada** |
| **Fase 7** | Integración y revisión general de la API | **Completada** |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |







