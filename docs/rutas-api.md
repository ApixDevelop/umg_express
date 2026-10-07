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
- **Request Body**:
  ```json
  { "username": "admin", "password": "admin123" }
  ```
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "mensaje": "Autenticación exitosa",
    "usuario": { "id": 1, "username": "admin", "nombre": "Administrador Inicial", "rol": "Administrador" }
  }
  ```

### POST `/v1/auth/usuarios`
Registra un nuevo usuario en `data/usuarios.dat`.
- **Request Body**:
  ```json
  { "username": "operador1", "password": "pass", "nombre": "Carlos Gómez", "rol": "Operador" }
  ```
- **Response `201 Created`**:
  ```json
  {
    "success": true,
    "mensaje": "Usuario creado exitosamente",
    "usuario": { "id": 2, "username": "operador1", "nombre": "Carlos Gómez", "rol": "Operador" }
  }
  ```

---

## 3. Sucursales (`/v1/sucursales`) - Implementado en Fase 4A

### GET `/v1/sucursales`
Obtiene la lista de todas las sucursales activas.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "sucursales": [
      { "id": 1, "nombre": "Sucursal Central", "direccion": "Zona 1, Guatemala", "telefono": "2222-1000" }
    ]
  }
  ```

### GET `/v1/sucursales/{id}`
Obtiene una sucursal específica por su ID.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "sucursal": { "id": 1, "nombre": "Sucursal Central", "direccion": "Zona 1, Guatemala", "telefono": "2222-1000" }
  }
  ```
- **Response `404 Not Found`**:
  ```json
  { "success": false, "mensaje": "Sucursal con ID 1 no encontrada o inactiva" }
  ```

### POST `/v1/sucursales`
Crea una nueva sucursal con validación de nombre único.
- **Request Body**:
  ```json
  { "nombre": "Sucursal Norte", "direccion": "Zona 18, Guatemala", "telefono": "2222-2000" }
  ```
- **Response `201 Created`**:
  ```json
  {
    "success": true,
    "mensaje": "Sucursal creada exitosamente",
    "sucursal": { "id": 2, "nombre": "Sucursal Norte", "direccion": "Zona 18, Guatemala", "telefono": "2222-2000" }
  }
  ```
- **Response `409 Conflict`**: Si ya existe una sucursal con el mismo nombre.

### PUT `/v1/sucursales/{id}`
Actualiza los datos de una sucursal existente.
- **Request Body**:
  ```json
  { "nombre": "Sucursal Central Modificada", "direccion": "Zona 10, Guatemala", "telefono": "2333-9999" }
  ```
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "mensaje": "Sucursal actualizada exitosamente",
    "sucursal": { "id": 1, "nombre": "Sucursal Central Modificada", "direccion": "Zona 10, Guatemala", "telefono": "2333-9999" }
  }
  ```

### DELETE `/v1/sucursales/{id}`
Realiza la baja lógica (`activo = false`) de una sucursal.
- **Response `200 OK`**:
  ```json
  { "success": true, "mensaje": "Sucursal eliminada exitosamente (baja lógica)" }
  ```

---

## 4. Clientes (`/v1/clientes`) - Implementado en Fase 4A

### GET `/v1/clientes`
Obtiene la lista de todos los clientes activos.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "clientes": [
      { "id": 1, "nit": "123456-7", "nombre": "Corporación Alfa", "direccion": "Roosevelt Km 14", "telefono": "2444-1111" }
    ]
  }
  ```

### GET `/v1/clientes/{id}`
Obtiene un cliente por su ID.
- **Response `200 OK`**:
  ```json
  {
    "success": true,
    "cliente": { "id": 1, "nit": "123456-7", "nombre": "Corporación Alfa", "direccion": "Roosevelt Km 14", "telefono": "2444-1111" }
  }
  ```

### POST `/v1/clientes`
Crea un nuevo cliente con validación de NIT único.
- **Request Body**:
  ```json
  { "nit": "123456-7", "nombre": "Corporación Alfa", "direccion": "Roosevelt Km 14", "telefono": "2444-1111" }
  ```
- **Response `201 Created`**:
  ```json
  {
    "success": true,
    "mensaje": "Cliente creado exitosamente",
    "cliente": { "id": 1, "nit": "123456-7", "nombre": "Corporación Alfa", "direccion": "Roosevelt Km 14", "telefono": "2444-1111" }
  }
  ```
- **Response `409 Conflict`**: Si ya existe un cliente con el mismo NIT.

### PUT `/v1/clientes/{id}`
Actualiza los datos de un cliente existente.
- **Request Body**:
  ```json
  { "nit": "123456-7", "nombre": "Corporación Alfa Modificada", "direccion": "Roosevelt Km 15", "telefono": "2444-9999" }
  ```
- **Response `200 OK`**:
  ```json
  { "success": true, "mensaje": "Cliente actualizado exitosamente", "cliente": { ... } }
  ```

### DELETE `/v1/clientes/{id}`
Realiza la baja lógica (`activo = false`) de un cliente.
- **Response `200 OK`**:
  ```json
  { "success": true, "mensaje": "Cliente eliminado exitosamente (baja lógica)" }
  ```

---

## 5. Endpoints Pendientes (Próximas Fases)

- `/v1/rutas` (Fase 4B)
- `/v1/vehiculos` (Fase 4B)
- `/v1/pilotos` (Fase 4B)
- `/v1/tarifas` (Fase 4B)
- `/v1/transacciones/guias` (Fase 5)
- `/v1/transacciones/despacho` (Fase 5)
- `/v1/reportes/*` (Fase 6)
