# Especificación Técnica de API REST — UMG Express

**Versión:** 1.0.0  
**Base URL:** `http://127.0.0.1:18080/v1`  
**Formato de Intercambio:** `application/json`  
**Especificación OpenAPI 3.0:** [`openapi.yaml`](file:///Users/josue/Uni/umgExpress/openapi.yaml) / [`docs/openapi.yaml`](file:///Users/josue/Uni/umgExpress/docs/openapi.yaml)

---

## 1. Convenciones Generales de la API

### Formato Único de Respuesta
Todas las respuestas de la API devuelven una estructura JSON consistente:
- **Respuesta Exitosa (`200 OK`, `201 Created`):** Contiene `"success": true`, un mensaje informativo y el objeto o arreglo de datos solicitado.
- **Respuesta de Error (`400 Bad Request`, `401 Unauthorized`, `404 Not Found`, `409 Conflict`):** Contiene `"success": false` y la descripción del error en `"error"` o `"mensaje"`.

### Códigos de Estado HTTP Utilizados
* `200 OK`: Consulta o actualización realizada exitosamente.
* `201 Created`: Recurso o transacción creada exitosamente.
* `400 Bad Request`: Datos de solicitud faltantes, malformados o que violan reglas de negocio (e.g. peso <= 0).
* `401 Unauthorized`: Credenciales de autenticación inválidas o usuario inactivo.
* `404 Not Found`: Recurso no encontrado o inactivo por baja lógica.
* `409 Conflict`: Intento de duplicar un campo único (e.g. NIT de cliente, licencia de piloto, username).

---

## 2. Autenticación y Usuarios (`/v1/auth`)

### 2.1 Autenticación de Usuario (`POST /v1/auth/login`)
Valida las credenciales de un usuario y devuelve su información de perfil.

- **Request Body:**
  ```json
  {
    "username": "admin",
    "password": "admin123"
  }
  ```
- **Response `200 OK`:**
  ```json
  {
    "success": true,
    "mensaje": "Autenticación exitosa",
    "usuario": {
      "id": 1,
      "username": "admin",
      "nombre": "Administrador Principal",
      "rol": "Administrador"
    }
  }
  ```
- **Response `401 Unauthorized`:**
  ```json
  {
    "success": false,
    "error": "Credenciales inválidas o usuario inactivo"
  }
  ```

### 2.2 Registro de Usuario (`POST /v1/auth/usuarios`)
Crea una nueva cuenta de usuario en `data/usuarios.dat`.

- **Request Body:**
  ```json
  {
    "username": "operador_central",
    "password": "pass123Secure",
    "nombre": "Carlos Gómez",
    "rol": "Operador"
  }
  ```
- **Response `201 Created`:**
  ```json
  {
    "success": true,
    "mensaje": "Usuario creado exitosamente",
    "usuario": {
      "id": 2,
      "username": "operador_central",
      "nombre": "Carlos Gómez",
      "rol": "Operador"
    }
  }
  ```

---

## 3. Catálogo de Sucursales (`/v1/sucursales`)

### 3.1 Listar Sucursales (`GET /v1/sucursales`)
- **Response `200 OK`:**
  ```json
  {
    "success": true,
    "sucursales": [
      {
        "id": 1,
        "nombre": "Sucursal Central Guatemala",
        "direccion": "Zona 10, Ciudad de Guatemala",
        "telefono": "2222-0001"
      }
    ]
  }
  ```

### 3.2 Consultar Sucursal por ID (`GET /v1/sucursales/{id}`)
- **Response `200 OK`:** Regresa el objeto de la sucursal.
- **Response `404 Not Found`:** Si la sucursal no existe o está inactiva.

### 3.3 Crear Sucursal (`POST /v1/sucursales`)
- **Request Body:**
  ```json
  {
    "nombre": "Sucursal Quetzaltenango",
    "direccion": "Zona 3, Quetzaltenango",
    "telefono": "7777-1010"
  }
  ```
- **Response `201 Created`:** Retorna la sucursal creada con su ID auto-incrementable.

### 3.4 Actualizar Sucursal (`PUT /v1/sucursales/{id}`)
- **Request Body:** Objeto con campos a actualizar.

### 3.5 Baja Lógica de Sucursal (`DELETE /v1/sucursales/{id}`)
- **Response `200 OK`:** Regresa confirmación de eliminación lógica (`activo = false`).

---

## 4. Catálogo de Clientes (`/v1/clientes`)

### 4.1 Listar Clientes (`GET /v1/clientes`)
- **Response `200 OK`:** Lista de clientes activos.

### 4.2 Crear Cliente (`POST /v1/clientes`)
- **Request Body:**
  ```json
  {
    "nit": "1234567-8",
    "nombre": "Distribuidora El Sol, S.A.",
    "direccion": "Calzada Roosevelt 12-45, Zona 11",
    "telefono": "2440-5555"
  }
  ```
- **Response `201 Created` / `409 Conflict`:** Valida que el NIT no pertenezca a otro cliente activo.

---

## 5. Catálogo de Rutas (`/v1/rutas`)

### 5.1 Crear Ruta (`POST /v1/rutas`)
Valida la existencia de las sucursales de origen y destino.
- **Request Body:**
  ```json
  {
    "idSucursalOrigen": 1,
    "idSucursalDestino": 2,
    "distanciaKm": 205.5,
    "tarifaBase": 50.0
  }
  ```
- **Response `201 Created`:** Retorna objeto de la ruta.

---

## 6. Catálogo de Vehículos (`/v1/vehiculos`)

### 6.1 Crear Vehículo (`POST /v1/vehiculos`)
- **Request Body:**
  ```json
  {
    "placa": "C-123XYZ",
    "marcaModelo": "Isuzu NPR 2024",
    "capacidadPesoKg": 3500.0,
    "capacidadVolumenM3": 18.5,
    "estado": "Disponible"
  }
  ```
- **Response `201 Created`:** Objeto vehículo registrado.

---

## 7. Catálogo de Pilotos (`/v1/pilotos`)

### 7.1 Crear Piloto (`POST /v1/pilotos`)
- **Request Body:**
  ```json
  {
    "nombre": "Juan Pérez López",
    "licencia": "LIC-987654",
    "telefono": "5555-4433"
  }
  ```
- **Response `201 Created` / `409 Conflict`:** Valida que la licencia no esté duplicada.

---

## 8. Catálogo de Tarifas (`/v1/tarifas`)

### 8.1 Crear Tarifa (`POST /v1/tarifas`)
- **Request Body:**
  ```json
  {
    "tipoCarga": "Estándar",
    "precioPorKg": 5.0,
    "precioPorM3": 150.0,
    "recargoEspecial": 10.0
  }
  ```
- **Response `201 Created`:** Objeto tarifa registrado.

---

## 9. Transacciones: Guías de Envío (`/v1/transacciones/guias`)

### 9.1 Emisión de Guía (`POST /v1/transacciones/guias`)
Aplica automáticamente la fórmula de peso volumétrico $\frac{L \times W \times H}{5000}$, determina el peso facturable $\max(\text{pesoReal}, \text{pesoVolumetrico})$, calcula el costo total e incrementa la numeración correlativa (1001+).

- **Request Body:**
  ```json
  {
    "idClienteRemitente": 1,
    "idClienteDestinatario": 2,
    "idRuta": 1,
    "pesoRealKg": 12.5,
    "largoCm": 50.0,
    "anchoCm": 40.0,
    "altoCm": 30.0,
    "idTarifa": 1
  }
  ```
- **Response `201 Created`:**
  ```json
  {
    "success": true,
    "mensaje": "Guía emitida exitosamente",
    "guia": {
      "numeroGuia": 1001,
      "idClienteRemitente": 1,
      "idClienteDestinatario": 2,
      "idRuta": 1,
      "pesoRealKg": 12.5,
      "largoCm": 50.0,
      "anchoCm": 40.0,
      "altoCm": 30.0,
      "pesoVolumetricoKg": 12.0,
      "pesoFacturableKg": 12.5,
      "costoTotal": 112.5,
      "estado": "Creada"
    }
  }
  ```

---

## 10. Transacciones: Despacho de Carga (`/v1/transacciones/despacho`)

### 10.1 Procesar Despacho (`POST /v1/transacciones/despacho`)
Asigna una lista de números de guía a un vehículo y piloto. Valida que la carga acumulada no supere la capacidad de peso (kg) ni volumen (m3) del vehículo y que el vehículo no esté `"En taller"`.

- **Request Body:**
  ```json
  {
    "idVehiculo": 1,
    "idPiloto": 1,
    "guias": [1001, 1002]
  }
  ```
- **Response `200 OK`:**
  ```json
  {
    "success": true,
    "mensaje": "Despacho procesado exitosamente",
    "despacho": {
      "idVehiculo": 1,
      "idPiloto": 1,
      "totalGuias": 2,
      "pesoTotalKg": 42.5,
      "volumenTotalM3": 0.18,
      "estadoVehiculo": "En ruta"
    }
  }
  ```
- **Response `400 Bad Request`:** Si se excede la capacidad del vehículo o una guía ya fue despachada.

---

## 11. Módulo de Reportes

### 11.1 Rastreo de Guía (`GET /v1/reportes/rastreo/{numeroGuia}`)
- **Response `200 OK`:** Regresa información consolidada del remitente, destinatario, ruta, origen/destino, costos, estado y datos del despacho asignado.

### 11.2 Manifiesto por Vehículo (`GET /v1/reportes/manifiesto/{idVehiculo}`)
- **Response `200 OK`:** Regresa la información del vehículo, piloto asignado, ruta y el listado completo desduplicado de guías cargadas en el transporte con totales de peso y volumen.

### 11.3 Liquidación Financiera (`GET /v1/reportes/liquidacion`)
- **Response `200 OK`:** Consolidado financiero global con total facturado, desglose por estado de guías (`"Creada"`, `"Despachada"`), desglose por sucursal de origen y lista detallada de cobros registrados.
