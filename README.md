# UMG Express — Sistema de Gestión Logística y Paquetería (Backend API)

**Universidad Mariano Gálvez de Guatemala (UMG)**  
**Proyecto Académico — Ingeniería en Sistemas de Información**

---

## 1. Descripción del Proyecto

**UMG Express** es un sistema backend de gestión logística y paquetería desarrollado en **C++17** que expone una **API REST** a través del framework **Crow**. El sistema administra el ciclo de vida completo de un envío exprés: desde la autenticación de usuarios por roles, la administración de catálogos clave (sucursales, clientes, rutas, vehículos, pilotos y tarifas), la emisión automatizada de guías con cálculo de peso volumétrico y costos, el despacho de carga con control estricto de capacidad física vehicular, hasta la generación de reportes operativos y financieros.

El proyecto implementa un esquema de **persistencias mediante archivos binarios de tamaño fijo (`.dat`)** utilizando la biblioteca estándar de C (`FILE*`, `fopen`, `fread`, `fwrite`, `fseek`, `ftell`, `fclose`), siguiendo un enfoque de programación estructurada, robusto y eficiente sin dependencia de bases de datos externas ni ORMs.

---

## 2. Tecnologías y Restricciones Técnicas

* **Lenguaje:** C++17
* **Framework HTTP:** Crow (REST API en `http://127.0.0.1:18080`)
* **Formato de Comunicación:** JSON (`application/json`)
* **Persistencia:** Archivos binarios `.dat` con registros de tamaño fijo
* **Sistema de Compilación:** Script Bash nativo `./build.sh` (Sin CMake / `CMakeLists.txt`)
* **Estilo de Programación:** Enfoque de programación estructurada, modular y limpia sin sobreingeniería

---

## 3. Estructura del Repositorio

```text
umgExpress/
├── main.cpp                     # Punto de entrada HTTP y controlador REST (Crow Server)
├── build.sh                     # Script ejecutable de compilación (C++17)
├── server                       # Binario ejecutable del servidor HTTP
├── include/                     # Módulos de lógica de negocio y persistencia
│   ├── structs.h                # Estructuras binarias de tamaño fijo (data models)
│   ├── persistence.h            # Motor genérico de persistencia binaria stdio
│   ├── auth.h                   # Autenticación y gestión de usuarios
│   ├── sucursales.h             # CRUD binario de sucursales
│   ├── clientes.h               # CRUD binario de clientes
│   ├── rutas.h                  # CRUD binario de rutas
│   ├── vehiculos.h              # CRUD binario de vehículos y capacidades
│   ├── pilotos.h                # CRUD binario de pilotos y licencias
│   ├── tarifas.h                # CRUD binario de tarifas por tipo de carga
│   ├── transacciones.h          # Guías, cálculo volumétrico, costos y despacho
│   └── reportes.h               # Reportes de rastreo, manifiesto y liquidación
├── tests/                       # Suites de pruebas unitarias e integración E2E
│   ├── test_persistence.cpp      # Pruebas de persistencia binaria .dat
│   ├── test_auth.cpp            # Pruebas de autenticación y roles
│   ├── test_sucursales_clientes.cpp # Pruebas CRUD sucursales y clientes
│   ├── test_rutas_vehiculos.cpp # Pruebas CRUD rutas y vehículos
│   ├── test_pilotos_tarifas.cpp # Pruebas CRUD pilotos y tarifas
│   ├── test_guias.cpp           # Pruebas de emisión y correlativo de guías
│   ├── test_calculos.cpp        # Pruebas de peso volumétrico y costos
│   ├── test_despacho.cpp        # Pruebas de despacho y capacidad física
│   ├── test_reporte_rastreo.cpp # Pruebas de reporte de rastreo
│   ├── test_reporte_manifiesto.cpp # Pruebas de manifiesto de vehículos
│   ├── test_reporte_liquidacion.cpp # Pruebas de reporte de liquidación
│   └── test_integracion_api.cpp # Prueba de integración End-to-End (E2E)
├── data/                        # Archivos binarios de almacenamiento (.dat)
│   ├── usuarios.dat
│   ├── sucursales.dat
│   ├── clientes.dat
│   ├── rutas.dat
│   ├── vehiculos.dat
│   ├── pilotos.dat
│   ├── tarifas.dat
│   ├── guias.dat
│   └── despachos.dat
└── docs/                        # Documentación técnica del proyecto
    ├── arquitectura.md          # Arquitectura de software y flujo modular
    ├── estructura-dat.md        # Especificación de estructuras binarias .dat
    ├── api.md                   # Documentación completa de endpoints REST
    ├── rutas-api.md             # Referencia rápida de rutas y contratos
    ├── reglas-negocio.md        # Reglas de negocio del dominio logístico
    ├── pruebas.md               # Informe de pruebas y estabilización
    └── estado-proyecto.md       # Estado actual y bitácora de fases
```

---

## 4. Requisitos del Entorno de Desarrollo

Para compilar y ejecutar UMG Express se requiere:

1. **Sistema Operativo:** macOS Darwin (arm64 / x86_64) o Linux.
2. **Compilador C++17:** `Apple Clang` (v12.0+) o `GCC` (v9.0+).
3. **Encabezados de Crow y Asio:** Crow HTTP Framework instalado en la ruta del compilador (por ejemplo, `/usr/local/include` o `/opt/homebrew/include`).

---

## 5. Instrucciones de Compilación y Ejecución

### 5.1 Compilación del Proyecto
Ejecute el script de compilación nativo disponible en la raíz del proyecto:

```bash
chmod +x build.sh
./build.sh
```

El script compilara `main.cpp` generando el binario `./server` y compilará automáticamente los executables de prueba (`./test_calculos`, `./test_despacho`, `./test_reporte_rastreo`, `./test_reporte_manifiesto`, `./test_reporte_liquidacion`, `./test_integracion_api`).

### 5.2 Iniciar el Servidor HTTP
Para ejecutar el backend de UMG Express:

```bash
./server
```

Salida esperada por consola:
```text
=== UMG Express Server (Crow HTTP API REST / C++17) ===
Servidor escuchando en http://127.0.0.1:18080
Prefijo base de API: /v1/
```

### 5.3 Detener el Servidor
Presione `Ctrl + C` en la terminal donde se ejecuta el servidor.

---

## 6. Ejecución de la Suite de Pruebas Automatizadas

El proyecto incluye 6 binarios de pruebas automatizadas que validan cada módulo y el flujo End-to-End:

```bash
# Ejecutar pruebas individuales
./test_calculos
./test_despacho
./test_reporte_rastreo
./test_reporte_manifiesto
./test_reporte_liquidacion
./test_integracion_api

# O ejecutar toda la suite en secuencia
./build.sh && ./test_calculos && ./test_despacho && ./test_reporte_rastreo && ./test_reporte_manifiesto && ./test_reporte_liquidacion && ./test_integracion_api
```

> **Nota sobre aislamiento de datos:** Las pruebas utilizan archivos temporales aislados (`data/test_*.dat`) que se crean y eliminan automáticamente durante cada ejecución, protegiendo en todo momento los archivos de producción en `data/`.

---

## 7. Resumen de la API REST (`/v1/`)

| Módulo | Método | Endpoint | Descripción |
| :--- | :--- | :--- | :--- |
| **Auth** | `POST` | `/v1/auth/login` | Autentica un usuario y devuelve sus datos y rol |
| **Auth** | `POST` | `/v1/auth/usuarios` | Registra un nuevo usuario (`Administrador`, `Operador`, `Auditor`) |
| **Sucursales** | `GET` / `POST` | `/v1/sucursales` | Listar / Crear sucursal |
| **Sucursales** | `GET` / `PUT` / `DELETE` | `/v1/sucursales/{id}` | Obtener / Actualizar / Baja lógica de sucursal |
| **Clientes** | `GET` / `POST` | `/v1/clientes` | Listar / Crear cliente (con NIT único) |
| **Clientes** | `GET` / `PUT` / `DELETE` | `/v1/clientes/{id}` | Obtener / Actualizar / Baja lógica de cliente |
| **Rutas** | `GET` / `POST` | `/v1/rutas` | Listar / Crear ruta entre sucursales activas |
| **Rutas** | `GET` / `PUT` / `DELETE` | `/v1/rutas/{id}` | Obtener / Actualizar / Baja lógica de ruta |
| **Vehículos** | `GET` / `POST` | `/v1/vehiculos` | Listar / Crear vehículo con límites de peso/volumen |
| **Vehículos** | `GET` / `PUT` / `DELETE` | `/v1/vehiculos/{id}` | Obtener / Actualizar / Baja lógica de vehículo |
| **Pilotos** | `GET` / `POST` | `/v1/pilotos` | Listar / Crear piloto con número de licencia único |
| **Pilotos** | `GET` / `PUT` / `DELETE` | `/v1/pilotos/{id}` | Obtener / Actualizar / Baja lógica de piloto |
| **Tarifas** | `GET` / `POST` | `/v1/tarifas` | Listar / Crear tarifa por categoría de carga |
| **Tarifas** | `GET` / `PUT` / `DELETE` | `/v1/tarifas/{id}` | Obtener / Actualizar / Baja lógica de tarifa |
| **Guías** | `POST` | `/v1/transacciones/guias` | Crear guía con cálculo de peso volumétrico y costo |
| **Guías** | `GET` | `/v1/transacciones/guias` | Listar todas las guías registradas |
| **Guías** | `GET` | `/v1/transacciones/guias/{numeroGuia}` | Consultar datos de una guía por su número único |
| **Despacho** | `POST` | `/v1/transacciones/despacho` | Despachar guías a vehículo validando peso/volumen |
| **Despacho** | `GET` | `/v1/transacciones/despacho` | Listar historial de despachos de carga |
| **Reportes** | `GET` | `/v1/reportes/rastreo/{numeroGuia}`| Reporte detallado de rastreo de guía |
| **Reportes** | `GET` | `/v1/reportes/manifiesto/{idVehiculo}`| Reporte de manifiesto de carga por vehículo |
| **Reportes** | `GET` | `/v1/reportes/liquidacion` | Reporte consolidado de liquidación financiera |

Consulte la especificación detallada con ejemplos JSON de solicitud y respuesta en [docs/api.md](docs/api.md).

---

## 8. Persistencia y Eliminación Lógica

1. **Tamaño Fijo:** Los datos persistentes no emplean punteros dinámicos ni objetos de longitud variable directamente en el archivo binario. Se emplean estructuras con buffers estilo C (`char[N]`).
2. **Acceso Directo via `fseek`:** La modificación de registros se realiza mediante posicionamiento directo en el archivo binario con `fseek(file, index * sizeof(T), SEEK_SET)` y sobreescritura con `fwrite`.
3. **Baja Lógica (`activo = false`):** Las operaciones de borrado no eliminan el registro físicamente del archivo `.dat`. Cambian la bandera `activo` a `false`. Las lecturas estándar filtran únicamente los registros donde `activo == true`.

---

## 9. Guía de Preparación Académica y Defensa del Sistema

### 9.1 Problema que Resuelve
UMG Express resuelve la necesidad de una plataforma de gestión logística eficiente que automatice el control operacional de paquetería express. Centraliza la emisión de guías de envío, calcula costos justos evaluando dimensiones físicas (peso volumétrico), optimiza el despacho en vehículos previniendo sobrecargas físicas (peso/volumen) y ofrece trazabilidad y liquidación financiera transparente.

### 9.2 Decisiones de Arquitectura Técnica
- **¿Por qué C++17 y Crow?** Proporciona un rendimiento óptimo de procesamiento en memoria y baja latencia HTTP, ideal para microservicios ligeros sin la sobrecarga de máquinas virtuales o intérpretes pesados.
- **¿Por qué almacenamiento binario `.dat` con C stdio?** Cumple estrictamente el requerimiento académico de manipular archivos binarios directamente a nivel de bytes con estructuras de tamaño fijo, demostrando dominio en punteros de archivo (`FILE*`), desplazamientos directos (`fseek`) y serialización nativa de estructuras de C/C++.
- **¿Por qué programación estructurada y modular?** Facilita la comprensión, mantenimiento y defensa del código en un ámbito académico, separando claramente definiciones de datos (`structs.h`), persistencia (`persistence.h`), lógica de dominio (`transacciones.h`, `reportes.h`) y entrega web (`main.cpp`).

### 9.3 Reglas de Negocio Clave
1. **Peso Volumétrico:** $\text{Peso Volumétrico (kg)} = \frac{\text{largo (cm)} \times \text{ancho (cm)} \times \text{alto (cm)}}{5000}$.
2. **Peso Facturable:** $\text{Peso Facturable} = \max(\text{Peso Real}, \text{Peso Volumétrico})$.
3. **Control Físico de Despacho:** Un despacho es rechazado si $\sum \text{Peso} > \text{Capacidad Peso Vehículo}$ o $\sum \text{Volumen} > \text{Capacidad Volumen Vehículo}$, o si el vehículo está en estado `"En taller"`.

---

## 10. Solución de Problemas Comunes

* **Error: `Puerto 18080 en uso`:**  
  Cierre cualquier instancia previa del servidor ejecutando en terminal:
  ```bash
  killall server
  ```
* **Error de inclusión de Crow / Asio:**  
  Asegúrese de contar con Crow en `/usr/local/include`. Si sus encabezados están en otra ruta (por ejemplo Homebrew), modifique `build.sh` agregando `-I/ruta/a/sus/headers`.

---

## 11. Licencia y Créditos

Proyecto desarrollado como trabajo práctico académico para la **Universidad Mariano Gálvez de Guatemala**. Todos los derechos reservados.
