# Arquitectura - UMG Express

## 1. Estructura del Proyecto (Fase 4A)

```text
umgExpress/
├── main.cpp                 # Punto de entrada HTTP y servidor Crow (rutas base, auth, sucursales, clientes)
├── build.sh                 # Script de compilación C++17 (sin CMake)
├── server                   # Ejecutable principal del servidor
├── test_persistence        # Pruebas unitarias de persistencia .dat
├── test_auth               # Pruebas unitarias de autenticación
├── test_sucursales_clientes # Pruebas unitarias de sucursales y clientes (Fase 4A)
├── include/                 # Encabezados modulares
│   ├── structs.h            # Estructuras de tamaño fijo para archivos binarios .dat
│   ├── persistence.h        # Motor genérico de persistencia binaria (stdio)
│   ├── auth.h               # Módulo de Autenticación y Usuarios
│   ├── sucursales.h         # Módulo CRUD de Sucursales (Fase 4A)
│   ├── clientes.h           # Módulo CRUD de Clientes (Fase 4A)
│   ├── rutas.h              # Módulo de Rutas (Próximas fases)
│   ├── vehiculos.h          # Módulo de Vehículos (Próximas fases)
│   ├── pilotos.h            # Módulo de Pilotos (Próximas fases)
│   ├── tarifas.h            # Módulo de Tarifas (Próximas fases)
│   ├── transacciones.h      # Módulo de Transacciones (Próximas fases)
│   └── reportes.h           # Módulo de Reportes (Próximas fases)
├── tests/                   # Pruebas unitarias e integración
│   ├── test_persistence.cpp  # Pruebas de almacenamiento .dat
│   ├── test_auth.cpp        # Pruebas de autenticación
│   └── test_sucursales_clientes.cpp # Pruebas de sucursales y clientes
├── data/                    # Directorio de persistencia binaria .dat
│   ├── usuarios.dat
│   ├── sucursales.dat
│   ├── clientes.dat
│   ├── rutas.dat
│   ├── vehiculos.dat
│   ├── pilotos.dat
│   ├── tarifas.dat
│   └── guias.dat
└── docs/                    # Documentación viva del proyecto
    ├── arquitectura.md
    ├── autenticacion.md
    ├── estructura-dat.md
    ├── rutas-api.md
    ├── reglas-negocio.md
    └── estado-proyecto.md
```

---

## 2. Capa de Negocio e Módulos Implementados

- **Autenticación (`include/auth.h`)**: Gestión de cuentas, roles (`Administrador`, `Operador`, `Auditor`), autenticación y creación de admin inicial.
- **Sucursales (`include/sucursales.h`)**: Gestión CRUD de sucursales, validaciones de nombres duplicados, auto-incremento de IDs y baja lógica en `data/sucursales.dat`.
- **Clientes (`include/clientes.h`)**: Gestión CRUD de clientes, validaciones de NIT único, auto-incremento de IDs y baja lógica en `data/clientes.dat`.

---

## 3. Servidor HTTP y API (`main.cpp`)

- Framework: **Crow HTTP C++ Framework**
- Enlace: `127.0.0.1:18080`
- Endpoints HTTP REST:
  - `/v1/auth/login` (`POST`)
  - `/v1/auth/usuarios` (`POST`)
  - `/v1/sucursales` (`GET`, `POST`)
  - `/v1/sucursales/<int>` (`GET`, `PUT`, `DELETE`)
  - `/v1/clientes` (`GET`, `POST`)
  - `/v1/clientes/<int>` (`GET`, `PUT`, `DELETE`)
