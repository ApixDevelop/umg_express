# Arquitectura - UMG Express

## 1. Estructura actual

```text
umg_express/
├── crow.h
├── main.cpp
├── CMakeLists.txt
├── include/
│   ├── structs.h
│   ├── auth.h
│   ├── sucursales.h
│   ├── clientes.h
│   ├── rutas.h
│   ├── vehiculos.h
│   ├── pilotos.h
│   ├── tarifas.h
│   ├── transacciones.h
│   └── reportes.h
└── data/
    ├── usuarios.dat
    ├── sucursales.dat
    ├── clientes.dat
    ├── rutas.dat
    ├── vehiculos.dat
    ├── pilotos.dat
    ├── tarifas.dat
    └── guias.dat
```

---

## 2. `main.cpp`

Es el punto de entrada de la aplicación.

Responsabilidades:

* Crear la aplicación Crow.
* Configurar el servidor.
* Registrar endpoints.
* Iniciar el servidor.

No debe contener toda la lógica del negocio.

---

## 3. `crow.h`

Cabecera utilizada para integrar el framework Crow.

Permite:

* Crear el servidor HTTP.
* Definir rutas.
* Procesar solicitudes HTTP.
* Generar respuestas JSON.

---

## 4. `include/structs.h`

Contiene las estructuras utilizadas por el sistema.

Las estructuras que se almacenan en archivos `.dat` deben utilizar campos de tamaño fijo.

---

## 5. Módulos

### `auth.h`

Autenticación y usuarios.

Archivo:

```text
data/usuarios.dat
```

### `sucursales.h`

CRUD de sucursales.

Archivo:

```text
data/sucursales.dat
```

### `clientes.h`

CRUD de clientes.

Archivo:

```text
data/clientes.dat
```

### `rutas.h`

CRUD de rutas.

Archivo:

```text
data/rutas.dat
```

### `vehiculos.h`

CRUD de vehículos.

Archivo:

```text
data/vehiculos.dat
```

### `pilotos.h`

CRUD de pilotos.

Archivo:

```text
data/pilotos.dat
```

### `tarifas.h`

Gestión de tarifas.

Archivo:

```text
data/tarifas.dat
```

### `transacciones.h`

Operaciones transaccionales.

Incluye:

* Guías.
* Cálculo de peso volumétrico.
* Costos.
* Despacho.
* Capacidad vehicular.

Archivo principal:

```text
data/guias.dat
```

### `reportes.h`

Consultas y reportes:

* Rastreo.
* Manifiestos.
* Liquidaciones.

---

## 6. Flujo general

```text
React
  |
  | HTTP / JSON
  v
Crow
  |
  v
main.cpp / endpoints
  |
  v
Funciones del módulo correspondiente
  |
  v
Archivos .dat
```

---

## 7. Principio arquitectónico

La API debe encargarse de recibir y responder solicitudes.

Las funciones de los módulos deben encargarse de la lógica correspondiente.

La persistencia debe realizarse directamente mediante archivos binarios.

No introducir bases de datos externas.
