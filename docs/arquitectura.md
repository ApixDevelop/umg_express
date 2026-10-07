# Arquitectura - UMG Express

## 1. Estructura del Proyecto (Fase 1)

```text
umgExpress/
├── main.cpp            # Punto de entrada HTTP y configuración de servidor Crow
├── build.sh            # Script de compilación C++17 (sin CMake)
├── server              # Ejecutable binario generado por build.sh
├── include/            # Encabezados de módulos (.h)
│   ├── structs.h       # Estructuras de tamaño fijo para archivos binarios .dat
│   ├── auth.h          # Módulo de Autenticación
│   ├── sucursales.h    # Módulo de Sucursales
│   ├── clientes.h      # Módulo de Clientes
│   ├── rutas.h         # Módulo de Rutas
│   ├── vehiculos.h     # Módulo de Vehículos
│   ├── pilotos.h       # Módulo de Pilotos
│   ├── tarifas.h       # Módulo de Tarifas
│   ├── transacciones.h # Módulo de Transacciones
│   └── reportes.h      # Módulo de Reportes
├── data/               # Directorio para archivos binarios .dat
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

## 2. Punto de Entrada (`main.cpp`)

Es el archivo principal de la aplicación.

Responsabilidades en la Fase 1:
* Instanciar la aplicación HTTP utilizando el framework **Crow**.
* Enlazar el servidor a `127.0.0.1:18080`.
* Configurar las rutas HTTP base (`GET /` y `GET /v1/health`).
* Responder solicitudes con formato JSON (`crow::json::wvalue`).
* Iniciar el bucle multihilo del servidor web.

---

## 3. Sistema de Compilación (`build.sh`)

No se utiliza CMake ni `CMakeLists.txt`.

La compilación se realiza directamente invocando `clang++` con el estándar C++17:

```bash
clang++ -std=c++17 -I/usr/local/include -Iinclude main.cpp -o server
```

---

## 4. Integración de Dependencias

- **Crow HTTP Framework**: Cabecera principal instalada en `/usr/local/include/crow.h`.
- **Asio**: Cabeceras asíncronas standalone en `/usr/local/include/asio.hpp`.

---

## 5. Flujo de Comunicación HTTP

```text
Cliente HTTP / Browser / cURL / React
                 |
                 | HTTP GET / JSON
                 v
            Servidor Crow
            (main.cpp:18080)
                 |
                 v
          Respuesta JSON 200 OK
```
