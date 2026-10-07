# Arquitectura - UMG Express

## 1. Estructura del Proyecto (Fase 2)

```text
umgExpress/
├── main.cpp            # Punto de entrada HTTP y servidor Crow
├── build.sh            # Script de compilación C++17 (sin CMake)
├── server              # Ejecutable principal del servidor
├── test_persistence    # Ejecutable de la suite de pruebas de persistencia
├── include/            # Encabezados modulares
│   ├── structs.h       # Estructuras de tamaño fijo para archivos binarios .dat
│   ├── persistence.h   # Motor genérico de persistencia binaria (stdio)
│   ├── auth.h          # Módulo de Autenticación (Próximas fases)
│   ├── sucursales.h    # Módulo de Sucursales (Próximas fases)
│   ├── clientes.h      # Módulo de Clientes (Próximas fases)
│   ├── rutas.h         # Módulo de Rutas (Próximas fases)
│   ├── vehiculos.h     # Módulo de Vehículos (Próximas fases)
│   ├── pilotos.h       # Módulo de Pilotos (Próximas fases)
│   ├── tarifas.h       # Módulo de Tarifas (Próximas fases)
│   ├── transacciones.h # Módulo de Transacciones (Próximas fases)
│   └── reportes.h      # Módulo de Reportes (Próximas fases)
├── tests/              # Pruebas unitarias e integración
│   └── test_persistence.cpp # Suite de pruebas de persistencia .dat
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
    ├── estructura-dat.md
    ├── rutas-api.md
    ├── reglas-negocio.md
    └── estado-proyecto.md
```

---

## 2. Capa de Persistencia Binaria (`include/persistence.h`)

La persistencia del proyecto no utiliza bases de datos relacionales ni NoSQL (sin SQL, SQLite, MongoDB ni ORMs).

Implementa acceso directo mediante stdio de C/C++:
- `fopen()` / `fclose()`
- `fread()` / `fwrite()`
- `fseek()` / `ftell()`

Cada entidad se almacena en su propio archivo `.dat` con registros de tamaño constante definidos en `include/structs.h`.

---

## 3. Servidor HTTP y API Base (`main.cpp`)

- Framework: **Crow HTTP C++ Framework**
- Binding: `127.0.0.1:18080`
- Al iniciar, el servidor verifica e inicializa automáticamente todos los archivos `.dat` en la carpeta `data/`.
- Rutas actuales:
  - `GET /` -> Estado de bienvenida y número de fase activa.
  - `GET /v1/health` -> Verificación de salud y estado de inicialización de archivos `.dat`.
