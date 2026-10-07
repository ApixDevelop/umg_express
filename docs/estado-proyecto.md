# Estado del Proyecto - UMG Express

## Información General
- **Proyecto**: UMG Express (Sistema Logístico y de Paquetería)
- **Fase Actual**: Fase 1 — Base del proyecto
- **Estado de la Fase**: Completada
- **Próxima Fase**: Fase 2 — Estructuras y persistencia (Pendiente de autorización)

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
  - Prueba empírica de compilación y ejecución de un servidor mínimo C++17 con Crow.

### Fase 1 — Base del Proyecto
- **Estado**: COMPLETADA
- **Trabajo Realizado**:
  - Creación del script ejecutable `build.sh` para compilar con C++17 y Clang++ sin CMake.
  - Implementación del punto de entrada `main.cpp` inicializando la aplicación `crow::SimpleApp`.
  - Configuración del binding explícito a `127.0.0.1` en el puerto `18080`.
  - Implementación del endpoint raíz (`GET /`) devolviendo bienvenida en JSON.
  - Implementación del endpoint de salud (`GET /v1/health`) devolviendo estado `UP` en JSON.
  - Verificación de la estructura de directorios (`include/`, `data/`, `docs/`).
  - Actualización de la documentación (`docs/arquitectura.md` y `docs/estado-proyecto.md`).
- **Pruebas Realizadas**:
  - Compilación del proyecto ejecutando `./build.sh` (código de salida 0).
  - Arranque del servidor HTTP en puerto 18080 (servidor iniciado exitosamente).
  - Solicitud HTTP GET a `http://127.0.0.1:18080/` (respuesta `200 OK` con JSON `{"estado":"ok", "mensaje":"...", "version":"1.0.0"}`).
  - Solicitud HTTP GET a `http://127.0.0.1:18080/v1/health` (respuesta `200 OK` con JSON `{"servicio":"UMG Express API", "status":"UP"}`).
  - Recompilación limpia de verificación con `./build.sh` (código de salida 0).
- **Decisiones Importantes**:
  - Vincular explícitamente la dirección `127.0.0.1` en `main.cpp` para asegurar la creación del socket sin conflictos de permisos.
  - Mantener `main.cpp` conciso con únicamente las rutas base HTTP para la verificación del servidor en esta fase.
- **Problemas Pendientes**:
  - Ninguno. La base del servidor funciona correctamente y está lista para recibir las estructuras y capas de persistencia en las fases siguientes.

---

## 2. Mapa de Fases del Proyecto

| Fase | Descripción | Estado |
| :--- | :--- | :--- |
| **Fase 0** | Análisis y configuración | **Completada** |
| **Fase 1** | Base del proyecto (`main.cpp`, `build.sh`, verificación de servidor) | **Completada** |
| **Fase 2** | Estructuras y persistencia (`structs.h`, operaciones `.dat`) | Pendiente |
| **Fase 3** | Autenticación (`usuarios.dat`, login, roles) | Pendiente |
| **Fase 4** | CRUD de catálogos (Sucursales, Clientes, Rutas, Vehículos, Pilotos, Tarifas) | Pendiente |
| **Fase 5** | Transacciones (Guías, Despacho, validaciones de peso/volumen) | Pendiente |
| **Fase 6** | Reportes (Rastreo, Manifiestos, Liquidación) | Pendiente |
| **Fase 7** | API completa e integración con React | Pendiente |
| **Fase 8** | Pruebas y estabilización | Pendiente |
| **Fase 9** | Documentación y revisión final | Pendiente |
