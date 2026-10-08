# API REST - UMG Express

## 1. Información General

La API utiliza Crow como framework HTTP en C++17.
Todas las respuestas utilizan el formato JSON y se comunican a través del puerto `18080` (enlace local `127.0.0.1`).

Prefijo global de API:
```text
/v1/
```

---

## 2. Autenticación (`/v1/auth`)

### POST `/v1/auth/login`
Valida las credenciales de usuario.
- **Request Body**: `{ "username": "admin", "password": "admin123" }`
- **Response `200 OK`**: `{ "success": true, "mensaje": "Autenticación exitosa", "usuario": { ... } }`

### POST `/v1/auth/usuarios`
Registra un nuevo usuario en `data/usuarios.dat`.
- **Request Body**: `{ "username": "operador1", "password": "pass", "nombre": "Carlos Gómez", "rol": "Operador" }`
- **Response `201 Created`**: `{ "success": true, "mensaje": "Usuario creado exitosamente", ... }`

---

## 3. Sucursales (`/v1/sucursales`) - Implementado en Fase 4A

### GET `/v1/sucursales` | GET `/v1/sucursales/{id}`
### POST `/v1/sucursales` | PUT `/v1/sucursales/{id}` | DELETE `/v1/sucursales/{id}`

---

## 4. Clientes (`/v1/clientes`) - Implementado en Fase 4A

### GET `/v1/clientes` | GET `/v1/clientes/{id}`
### POST `/v1/clientes` | PUT `/v1/clientes/{id}` | DELETE `/v1/clientes/{id}`

---

## 5. Rutas (`/v1/rutas`) - Implementado en Fase 4B

### GET `/v1/rutas` | GET `/v1/rutas/{id}`
### POST `/v1/rutas` | PUT `/v1/rutas/{id}` | DELETE `/v1/rutas/{id}`

---

## 6. Vehículos (`/v1/vehiculos`) - Implementado en Fase 4B

### GET `/v1/vehiculos` | GET `/v1/vehiculos/{id}`
### POST `/v1/vehiculos` | PUT `/v1/vehiculos/{id}` | DELETE `/v1/vehiculos/{id}`

---

## 7. Pilotos (`/v1/pilotos`) - Implementado en Fase 4C

### GET `/v1/pilotos`
Obtiene la lista de pilotos activos.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "pilotos": [
      {
        "id": 1,
        "nombre": "Mario Estrada",
        "licencia": "LIC-12345",
        "telefono": "5555-1111"
      }
    ]
  }
  ```

### GET `/v1/pilotos/{id}`
Obtiene un piloto por su ID.
- **Response `200 OK`**: Devuelve objeto `piloto`.
- **Response `404 Not Found`**: Si no existe o está inactivo.

### POST `/v1/pilotos`
Registra un nuevo piloto con validación de número de licencia único.
- **Request Body**:
  ```json
  {
    "nombre": "Mario Estrada",
    "licencia": "LIC-12345",
    "telefono": "5555-1111"
  }
  ```
- **Response `201 Created`**: Piloto creado exitosamente.
- **Response `409 Conflict`**: Si el número de licencia ya existe.
- **Response `400 Bad Request`**: Si faltan campos obligatorios.

### PUT `/v1/pilotos/{id}`
Actualiza los datos de un piloto existente.
- **Request Body**:
  ```json
  {
    "nombre": "Mario Estrada Pérez",
    "licencia": "LIC-12345",
    "telefono": "5555-9999"
  }
  ```
- **Response `200 OK`**: Piloto actualizado exitosamente.

### DELETE `/v1/pilotos/{id}`
Realiza la baja lógica (`activo = false`) del piloto.
- **Response `200 OK`**: `{ "success": true, "mensaje": "Piloto eliminado exitosamente (baja lógica)" }`

---

## 8. Tarifas (`/v1/tarifas`) - Implementado en Fase 4C

### GET `/v1/tarifas`
Obtiene la lista de tarifas activas por tipo de carga.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "tarifas": [
      {
        "id": 1,
        "tipoCarga": "Estándar",
        "precioPorKg": 10.5,
        "precioPorM3": 15.0,
        "recargoEspecial": 0.0
      }
    ]
  }
  ```

### GET `/v1/tarifas/{id}`
Obtiene una tarifa por su ID.
- **Response `200 OK`**: Devuelve objeto `tarifa`.
- **Response `404 Not Found`**: Si no existe o está inactiva.

### POST `/v1/tarifas`
Registra una nueva tarifa con validación de tipo de carga único y precios/recargos >= 0.
- **Request Body**:
  ```json
  {
    "tipoCarga": "Estándar",
    "precioPorKg": 10.5,
    "precioPorM3": 15.0,
    "recargoEspecial": 0.0
  }
  ```
- **Response `201 Created`**: Tarifa creada exitosamente.
- **Response `409 Conflict`**: Si ya existe una tarifa para el mismo tipo de carga.
- **Response `400 Bad Request`**: Si los precios son negativos o falta el tipo de carga.

### PUT `/v1/tarifas/{id}`
Actualiza los precios o recargos de una tarifa existente.
- **Request Body**:
  ```json
  {
    "tipoCarga": "Estándar",
    "precioPorKg": 11.5,
    "precioPorM3": 16.0,
    "recargoEspecial": 2.0
  }
  ```
- **Response `200 OK`**: Tarifa actualizada exitosamente.

### DELETE `/v1/tarifas/{id}`
Realiza la baja lógica (`activo = false`) de la tarifa.
- **Response `200 OK`**: `{ "success": true, "mensaje": "Tarifa eliminada exitosamente (baja lógica)" }`

---

---

## 9. Transacciones - Guías (`/v1/transacciones/guias`) - Implementado en Fases 5A y 5B

### POST `/v1/transacciones/guias`
Emite una nueva guía de envío con numeración correlativa automática (iniciando en 1001) y calcula peso volumétrico, peso facturable y costo total.

- **Validaciones**:
  - `idClienteRemitente` debe ser un cliente activo y existente.
  - `idClienteDestinatario` debe ser un cliente activo y existente.
  - `idClienteRemitente` y `idClienteDestinatario` deben ser diferentes.
  - `idRuta` debe ser una ruta activa y existente.
  - `pesoRealKg` (kg), `altoCm`, `anchoCm` y `largoCm` (cm) deben ser estrictamente mayores a 0.
  - `idTarifa` o `tipoCarga` opcional para cálculo tarifa de carga.

- **Request Body**:
  ```json
  {
    "idClienteRemitente": 1,
    "idClienteDestinatario": 2,
    "idRuta": 1,
    "pesoRealKg": 5.0,
    "largoCm": 60.0,
    "anchoCm": 50.0,
    "altoCm": 40.0,
    "idTarifa": 1
  }
  ```
- **Response `201 Created`**:
  ```json
  {
    "success": true,
    "mensaje": "Guía creada exitosamente con peso volumétrico y costo calculados",
    "guia": {
      "numeroGuia": 1001,
      "idClienteRemitente": 1,
      "idClienteDestinatario": 2,
      "idRuta": 1,
      "pesoRealKg": 5.0,
      "largoCm": 60.0,
      "anchoCm": 50.0,
      "altoCm": 40.0,
      "pesoVolumetricoKg": 24.0,
      "pesoFacturableKg": 24.0,
      "costoTotal": 285.6,
      "estado": "Creada"
    }
  }
  ```
- **Response `400 Bad Request`**: Si faltan campos obligatorios, las dimensiones/peso son <= 0, o los clientes remitente y destinatario son idénticos.
- **Response `404 Not Found`**: Si el cliente remitente, cliente destinatario o ruta no existen o están inactivos.

### GET `/v1/transacciones/guias`
Obtiene el listado completo de guías activas emitidas en el sistema con todos sus campos calculados.

- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "guias": [
      {
        "numeroGuia": 1001,
        "idClienteRemitente": 1,
        "idClienteDestinatario": 2,
        "idRuta": 1,
        "pesoRealKg": 5.0,
        "largoCm": 60.0,
        "anchoCm": 50.0,
        "altoCm": 40.0,
        "pesoVolumetricoKg": 24.0,
        "pesoFacturableKg": 24.0,
        "costoTotal": 285.6,
        "estado": "Creada"
      }
    ]
  }
  ```

### GET `/v1/transacciones/guias/{numeroGuia}`
Busca una guía específica por su número correlativo de guía (`numeroGuia`).

- **Response `200 OK`**: Devuelve el objeto `guia` completo con pesos y costos calculados.
- **Response `404 Not Found`**: Si no existe la guía o se encuentra inactiva.

---

## 10. Endpoints Pendientes (Próximas Fases)

- `/v1/transacciones/despacho` (Fase 5C)
- `/v1/reportes/rastreo/{numeroGuia}` (Fase 6)
- `/v1/reportes/manifiesto/{idVehiculo}` (Fase 6)
- `/v1/reportes/liquidacion` (Fase 6)


