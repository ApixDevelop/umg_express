# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 3 — Autenticación y usuarios
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 4 — CRUD de catálogos (Pendiente de autorización)

---

## 1. Bitácora de Fases

### Fase 0 — Análisis y Configuración
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Análisis del entorno operativo (macOS Darwin 25.6.0 x86_64, zsh).
  - Identificación del compilador C++ (`Apple clang version 17.0.0`).
  - Verificación del soporte C++17 (`-std=c++17`).
  - Identificación y verificación de dependencias de Crow (`crow` y `asio` en `/usr/local/include`).
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
  - Motor genérico de persistencia binaria en `include/persistence.h` usando C stdio (`fopen`, `fclose`, `fread`, `fwrite`, `fseek`, `ftell`).
  - Suite de pruebas unitarias en `tests/test_persistence.cpp`.

### Fase 3 — Autenticación y Usuarios
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Implementación del módulo de lógica de negocio para usuarios y autenticación en `include/auth.h`.
  - Definición y soporte para los 3 roles requeridos: `Administrador`, `Operador` y `Auditor`.
  - Implementación de validación de campos obligatorios, restricciones de longitud y prevención de usuarios duplicados por `username`.
  - Implementación de la función `inicializarUsuarioAdminPorDefecto` que crea la cuenta inicial `admin` / `admin123` si `data/usuarios.dat` no contiene usuarios activos.
  - Implementación de los endpoints REST en Crow (`main.cpp`):
    - `POST /v1/auth/login`: Autenticación con verificación de contraseña y comprobación de estado `activo = true`. Devuelve respuestas JSON estandarizadas (200 OK en éxito, 401 Unauthorized en credenciales/usuario inactivo).
    - `POST /v1/auth/usuarios`: Registro de nuevos usuarios con auto-incremento de ID y validaciones (201 Created en éxito, 409 Conflict en duplicados, 400 Bad Request en datos/roles inválidos).
  - Implementación de la suite de pruebas unitarias en `tests/test_auth.cpp`.
  - Creación de la documentación oficial en `docs/autenticacion.md`.
  - Actualización de `docs/arquitectura.md` y `docs/estado-proyecto.md`.
- **Pruebas Realizadas**:
  - Registro de usuarios con roles válidos (`Administrador`, `Operador`, `Auditor`).
  - Rechazo de nombres de usuario duplicados (`409 Conflict`).
  - Rechazo de roles no contemplados (`400 Bad Request`).
  - Autenticación exitosa (`200 OK`).
  - Rechazo por contraseña errónea (`401 Unauthorized`).
  - Rechazo por usuario inactivo (`activo = false`).
  - Persistencia de usuarios tras simular reinicio del sistema de archivos.
  - Peticiones HTTP reales ejecutadas mediante `curl` a `/v1/auth/login` y `/v1/auth/usuarios`.
  - Recompilación limpia verificada con `./build.sh`.
- **Decisiones Importantes**:
  - Mantener las contraseñas en plano/hash directo en la estructura de 64 bytes adecuada para un ámbito académico sin agregar librerías criptográficas externas innecesarias.
  - Crear automáticamente la cuenta `admin` / `admin123` si la base binaria está limpia al arrancar el servidor.
  - Mantener el control de roles como una propiedad del usuario sin agregar sistemas complejos de permisos.
- **Problemas Pendientes**:
  - Ninguno. El módulo de autenticación funciona correctamente.

---

## 2. Mapa de Fases del Proyecto

| Fase | Descripción | Estado |
| :--- | :--- | :--- |
| **Fase 0** | Análisis y configuración | **Completada** |
| **Fase 1** | Base del proyecto (`main.cpp`, `build.sh`, verificación de servidor) | **Completada** |
| **Fase 2** | Estructuras y persistencia (`structs.h`, `persistence.h`, `.dat`) | **Completada** |
| **Fase 3** | Autenticación y usuarios (`auth.h`, `POST /v1/auth/login`, `POST /v1/auth/usuarios`) | **Completada** |
| **Fase 4** | CRUD de catálogos (Sucursales, Clientes, Rutas, Vehículos, Pilotos, Tarifas) | Pendiente |
| **Fase 5** | Transacciones (Guías, Despacho, validaciones de peso/volumen) | Pendiente |
| **Fase 6** | Reportes (Rastreo, Manifiestos, Liquidación) | Pendiente |
| **Fase 7** | API completa e integración con React | Pendiente |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |
