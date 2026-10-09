# Arquitectura - UMG Express

## 1. Estructura del Proyecto (Fase 6A)

```text
umgExpress/
├── main.cpp                 # Punto de entrada HTTP y servidor Crow (rutas base, auth, catálogos, guías, despacho y reportes)
├── build.sh                 # Script de compilación C++17 (sin CMake)
├── server                   # Ejecutable principal del servidor
├── test_persistence        # Pruebas unitarias de persistencia .dat
├── test_auth               # Pruebas unitarias de autenticación
├── test_sucursales_clientes # Pruebas unitarias de sucursales y clientes (Fase 4A)
├── test_rutas_vehiculos     # Pruebas unitarias de rutas y vehículos (Fase 4B)
├── test_pilotos_tarifas     # Pruebas unitarias de pilotos y tarifas (Fase 4C)
├── test_guias               # Pruebas unitarias de emisión y consulta de guías (Fase 5A)
├── test_calculos            # Pruebas unitarias de peso volumétrico y costos (Fase 5B)
├── test_despacho            # Pruebas unitarias de despacho y control de capacidad (Fase 5C)
├── test_reporte_rastreo    # Pruebas unitarias de reporte de rastreo de guías (Fase 6A)
├── include/                 # Encabezados modulares
│   ├── structs.h            # Estructuras de tamaño fijo para archivos binarios .dat
│   ├── persistence.h        # Motor genérico de persistencia binaria (stdio)
│   ├── auth.h               # Módulo de Autenticación y Usuarios
│   ├── sucursales.h         # Módulo CRUD de Sucursales (Fase 4A)
│   ├── clientes.h           # Módulo CRUD de Clientes (Fase 4A)
│   ├── rutas.h              # Módulo CRUD de Rutas (Fase 4B)
│   ├── vehiculos.h          # Módulo CRUD de Vehículos (Fase 4B)
│   ├── pilotos.h            # Módulo CRUD de Pilotos (Fase 4C)
│   ├── tarifas.h            # Módulo CRUD de Tarifas (Fase 4C)
│   ├── transacciones.h      # Módulo de Transacciones - Guías, Cálculos y Despacho (Fases 5A/5B/5C)
│   └── reportes.h           # Módulo de Reportes - Rastreo de Guías (Fase 6A)
├── tests/                   # Pruebas unitarias e integración
│   ├── test_persistence.cpp  # Pruebas de almacenamiento .dat
│   ├── test_auth.cpp        # Pruebas de autenticación
│   ├── test_sucursales_clientes.cpp # Pruebas de sucursales y clientes
│   ├── test_rutas_vehiculos.cpp     # Pruebas de rutas y vehículos
│   ├── test_pilotos_tarifas.cpp     # Pruebas de pilotos y tarifas
│   ├── test_guias.cpp               # Pruebas de emisión y consulta de guías (Fase 5A)
│   ├── test_calculos.cpp            # Pruebas de peso volumétrico y costo total (Fase 5B)
│   ├── test_despacho.cpp            # Pruebas de despacho y límites de capacidad (Fase 5C)
│   └── test_reporte_rastreo.cpp     # Pruebas de reporte de rastreo (Fase 6A)
├── data/                    # Directorio de persistencia binaria .dat
│   ├── usuarios.dat
│   ├── sucursales.dat
│   ├── clientes.dat
│   ├── rutas.dat
│   ├── vehiculos.dat
│   ├── pilotos.dat
│   ├── tarifas.dat
│   ├── guias.dat
│   └── despachos.dat
└── docs/                    # Documentación viva del proyecto
    ├── arquitectura.md
    ├── autenticacion.md
    ├── estructura-dat.md
    ├── guias.md
    ├── calculos-guias.md
    ├── despacho.md
    ├── reporte-rastreo.md
    ├── rutas-api.md
    ├── reglas-negocio.md
    └── estado-proyecto.md
```

---

## 2. Capa de Negocio y Módulos Implementados

- **Autenticación (`include/auth.h`)**: Gestión de cuentas, roles (`Administrador`, `Operador`, `Auditor`), autenticación y creación de admin inicial.
- **Sucursales (`include/sucursales.h`)**: Gestión CRUD de sucursales, validaciones de nombres duplicados, auto-incremento de IDs y baja lógica.
- **Clientes (`include/clientes.h`)**: Gestión CRUD de clientes, validaciones de NIT único, auto-incremento de IDs y baja lógica.
- **Rutas (`include/rutas.h`)**: Gestión CRUD de rutas entre sucursales activas.
- **Vehículos (`include/vehiculos.h`)**: Gestión CRUD de vehículos con capacidades de peso (kg), volumen (m3) y estados operacionales.
- **Pilotos (`include/pilotos.h`)**: Gestión CRUD de pilotos con número de licencia único y datos de contacto para asignación de transportes.
- **Tarifas (`include/tarifas.h`)**: Gestión CRUD de tarifas por tipo de carga (precios por kg, por m3 y recargos especiales).
- **Transacciones - Guías y Cálculos (`include/transacciones.h`)**: Emisión de guías de envío con numeración correlativa automática (1001+), cálculo automático de peso volumétrico: `(largo*ancho*alto)/5000`, peso facturable: `max(pesoReal, pesoVolumetrico)`, y costo total integrado.
- **Transacciones - Despacho (`include/transacciones.h`)**: Asignación de guías a vehículos de transporte, control de capacidad acumulada de peso y volumen, rechazo automático si se exceden los límites o si el vehículo está en taller, actualización de guía a `"Despachada"` y de vehículo a `"En ruta"`.
- **Reportes - Rastreo (`include/reportes.h`)**: Consulta integral de rastreo por `numeroGuia` consolidando datos de cliente remitente/destinatario, ruta, sucursales de origen/destino, peso, costos, y datos de despacho (vehículo y piloto) si la guía ha sido despachada.

---

## 3. Servidor HTTP y API (`main.cpp`)

- Framework: **Crow HTTP C++ Framework**
- Enlace: `127.0.0.1:18080`
- Endpoints HTTP REST:
  - `/v1/auth/login` (`POST`) | `/v1/auth/usuarios` (`POST`)
  - `/v1/sucursales` (`GET`, `POST`) | `/v1/sucursales/<int>` (`GET`, `PUT`, `DELETE`)
  - `/v1/clientes` (`GET`, `POST`) | `/v1/clientes/<int>` (`GET`, `PUT`, `DELETE`)
  - `/v1/rutas` (`GET`, `POST`) | `/v1/rutas/<int>` (`GET`, `PUT`, `DELETE`)
  - `/v1/vehiculos` (`GET`, `POST`) | `/v1/vehiculos/<int>` (`GET`, `PUT`, `DELETE`)
  - `/v1/pilotos` (`GET`, `POST`) | `/v1/pilotos/<int>` (`GET`, `PUT`, `DELETE`)
  - `/v1/tarifas` (`GET`, `POST`) | `/v1/tarifas/<int>` (`GET`, `PUT`, `DELETE`)
  - `/v1/transacciones/guias` (`GET`, `POST`) | `/v1/transacciones/guias/<int>` (`GET`)
  - `/v1/transacciones/despacho` (`GET`, `POST`)
  - `/v1/reportes/rastreo/<int>` (`GET`)




