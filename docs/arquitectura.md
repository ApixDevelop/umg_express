# Arquitectura - UMG Express

## 1. Estructura del Proyecto (Fase 3)

```text
umgExpress/
├── main.cpp            # Punto de entrada HTTP y servidor Crow (rutas base y /v1/auth/*)
├── build.sh            # Script de compilación C++17 (sin CMake)
├── server              # Ejecutable principal del servidor
├── test_persistence    # Suite de pruebas de persistencia .dat
├── test_auth           # Suite de pruebas de autenticación y usuarios
├── include/            # Encabezados modulares
│   ├── structs.h       # Estructuras de tamaño fijo para archivos binarios .dat
│   ├── persistence.h   # Motor genérico de persistencia binaria (stdio)
│   ├── auth.h          # Módulo de Autenticación y Usuarios
│   ├── sucursales.h    # Módulo de Sucursales (Próximas fases)
│   ├── clientes.h      # Módulo de Clientes (Próximas fases)
│   ├── rutas.h         # Módulo de Rutas (Próximas fases)
│   ├── vehiculos.h     # Módulo de Vehículos (Próximas fases)
│   ├── pilotos.h       # Módulo de Pilotos (Próximas fases)
│   ├── tarifas.h       # Módulo de Tarifas (Próximas fases)
│   ├── transacciones.h # Módulo de Transacciones (Próximas fases)
│   └── reportes.h      # Módulo de Reportes (Próximas fases)
├── tests/              # Pruebas unitarias e integración
│   ├── test_persistence.cpp # Pruebas de almacenamiento .dat
│   └── test_auth.cpp        # Pruebas de autenticación y usuarios
├── data/               # Directorio de persistencia binaria .dat
│   ├── usuarios.dat
│   ├── sucursales.dat
│   ├── clientes.dat
│   ├── rutas.dat
│   ├── vehiculos.dat
│   ├── pilotos.dat
│   ├── tarifas.dat
│   └── guias.dat
└── docs/               # Documentación viva del proyecto
    ├── arquitectura.md
    ├── autenticacion.md
    ├── estructura-dat.md
    ├── rutas-api.md
    ├── reglas-negocio.md
    └── estado-proyecto.md
```

---

## 2. Capa de Autenticación y Negocio (`include/auth.h`)

La lógica de negocio de usuarios está encapsulada en el namespace `Auth`:
- Validaciones de roles (`Administrador`, `Operador`, `Auditor`).
- Validación de duplicidad por `username`.
- Autenticación con verificación de contraseña y estado `activo`.
- Inicialización automática del usuario `admin` inicial si la base binaria está vacía.

---

## 3. Servidor HTTP y Endpoints API (`main.cpp`)

- Framework: **Crow HTTP C++ Framework**
- Binding: `127.0.0.1:18080`
- Endpoints activos:
  - `GET /` -> Estado del servidor.
  - `GET /v1/health` -> Estado del servicio y persistencia.
  - `POST /v1/auth/login` -> Validación de credenciales.
  - `POST /v1/auth/usuarios` -> Registro de nuevos usuarios.
