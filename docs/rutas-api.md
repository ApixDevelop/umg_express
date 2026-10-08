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

### GET `/v1/rutas`
Obtiene la lista de todas las rutas activas.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "rutas": [
      {
        "id": 1,
        "idSucursalOrigen": 1,
        "idSucursalDestino": 2,
        "distanciaKm": 205.5,
        "tarifaBase": 75.0
      }
    ]
  }
  ```

### GET `/v1/rutas/{id}`
Obtiene una ruta por su ID.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "ruta": {
      "id": 1,
      "idSucursalOrigen": 1,
      "idSucursalDestino": 2,
      "distanciaKm": 205.5,
      "tarifaBase": 75.0
    }
  }
  ```
- **Response `404 Not Found`**: Si la ruta no existe o está inactiva.

### POST `/v1/rutas`
Crea una nueva ruta. Valida que las sucursales de origen y destino existan y estén activas en `data/sucursales.dat`, que el origen sea distinto al destino, y que no exista ruta duplicada.
- **Request Body**:
  ```json
  {
    "idSucursalOrigen": 1,
    "idSucursalDestino": 2,
    "distanciaKm": 205.5,
    "tarifaBase": 75.0
  }
  ```
- **Response `201 Created`**: Devuelve el objeto `ruta` creado con su ID auto-incrementado.
- **Response `404 Not Found`**: Si la sucursal de origen o destino no existe o está inactiva.
- **Response `409 Conflict`**: Si ya existe una ruta activa entre el mismo origen y destino.
- **Response `400 Bad Request`**: Si origen == destino o distanciaKm <= 0.

### PUT `/v1/rutas/{id}`
Actualiza los datos de una ruta existente.
- **Request Body**:
  ```json
  {
    "idSucursalOrigen": 1,
    "idSucursalDestino": 2,
    "distanciaKm": 210.0,
    "tarifaBase": 85.0
  }
  ```
- **Response `200 OK`**: Ruta actualizada exitosamente.

### DELETE `/v1/rutas/{id}`
Realiza la baja lógica (`activo = false`) de la ruta.
- **Response `200 OK`**: `{ "success": true, "mensaje": "Ruta eliminada exitosamente (baja lógica)" }`

---

## 6. Vehículos (`/v1/vehiculos`) - Implementado en Fase 4B

### GET `/v1/vehiculos`
Obtiene la lista de vehículos activos.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "vehiculos": [
      {
        "id": 1,
        "placa": "P-123XYZ",
        "marcaModelo": "Isuzu NPR 2024",
        "capacidadPesoKg": 3500.0,
        "capacidadVolumenM3": 18.5,
        "estado": "Disponible"
      }
    ]
  }
  ```

### GET `/v1/vehiculos/{id}`
Obtiene un vehículo por su ID.
- **Response `200 OK`**: Retorna el objeto `vehiculo`.
- **Response `404 Not Found`**: Si no existe o está inactivo.

### POST `/v1/vehiculos`
Registra un nuevo vehículo con validación de placa única y estado permitido (`"Disponible"`, `"En ruta"`, `"En taller"`).
- **Request Body**:
  ```json
  {
    "placa": "P-123XYZ",
    "marcaModelo": "Isuzu NPR 2024",
    "capacidadPesoKg": 3500.0,
    "capacidadVolumenM3": 18.5,
    "estado": "Disponible"
  }
  ```
- **Response `201 Created`**: Vehículo creado exitosamente.
- **Response `409 Conflict`**: Si la placa ya está registrada.
- **Response `400 Bad Request`**: Si el estado o las capacidades son inválidas.

### PUT `/v1/vehiculos/{id}`
Actualiza los datos o el estado de un vehículo existente.
- **Request Body**:
  ```json
  {
    "placa": "P-123XYZ",
    "marcaModelo": "Isuzu NPR 2024",
    "capacidadPesoKg": 3500.0,
    "capacidadVolumenM3": 18.5,
    "estado": "En ruta"
  }
  ```
- **Response `200 OK`**: Vehículo actualizado exitosamente.

### DELETE `/v1/vehiculos/{id}`
Realiza la baja lógica (`activo = false`) del vehículo.
- **Response `200 OK`**: `{ "success": true, "mensaje": "Vehículo eliminado exitosamente (baja lógica)" }`

---

## 7. Endpoints Pendientes (Próximas Fases)

- `/v1/pilotos` (Fase 4C)
- `/v1/tarifas` (Fase 4C)
- `/v1/transacciones/guias` (Fase 5)
- `/v1/transacciones/despacho` (Fase 5)
- `/v1/reportes/*` (Fase 6)
