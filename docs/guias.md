# Módulo de Emisión y Gestión de Guías - UMG Express

## 1. Descripción General

El módulo de Guías de Envío (Fase 5A) gestiona la creación, numeración secuencial correlativa y consulta de paquetes de envío en UMG Express.
La información se almacena en el archivo binario `data/guias.dat` utilizando registros de tamaño constante (`struct Guia`).

---

## 2. Estructura de Datos (`include/structs.h`)

```cpp
struct Guia {
    int numeroGuia;           // Número único correlativo de guía (4 bytes, inicia en 1001)
    int idClienteRemitente;   // ID del cliente remitente (4 bytes)
    int idClienteDestinatario;// ID del cliente destinatario (4 bytes)
    int idRuta;               // ID de la ruta asignada (4 bytes)
    double pesoRealKg;        // Peso medido en báscula en kg (8 bytes)
    double largoCm;           // Largo del paquete en cm (8 bytes)
    double anchoCm;           // Ancho del paquete en cm (8 bytes)
    double altoCm;            // Alto del paquete en cm (8 bytes)
    double pesoVolumetricoKg; // Reservado para la Fase 5B (8 bytes)
    double pesoFacturableKg;  // Reservado para la Fase 5B (8 bytes)
    double costoTotal;        // Reservado para la Fase 5B (8 bytes)
    char estado[20];          // "Creada", "Despachada", "Entregada" (20 bytes)
    bool activo;              // Estado activo/inactivo para baja lógica (1 byte + padding)
};
```

---

## 3. Lógica de Negocio (`include/transacciones.h`)

- **Numeración Correlativa**: Los números de guía se generan automáticamente escaneando `data/guias.dat` y asignando `max(numeroGuia) + 1` (iniciando en 1001).
- **Validaciones Relacionales**:
  - `idClienteRemitente` debe existir y estar activo en `data/clientes.dat`.
  - `idClienteDestinatario` debe existir y estar activo en `data/clientes.dat`.
  - `idClienteRemitente` y `idClienteDestinatario` no pueden ser el mismo cliente.
  - `idRuta` debe existir y estar activa en `data/rutas.dat`.
  - `pesoRealKg` debe ser mayor a 0 kg.
  - Dimensiones (`largoCm`, `anchoCm`, `altoCm`) no pueden ser negativas.
- **Estado Inicial**: Cada nueva guía se registra automáticamente con el estado `"Creada"`.

---

## 4. Endpoints HTTP REST

### 4.1 POST `/v1/transacciones/guias`

Registra una nueva guía de envío.

- **Solicitud**:
  - `Content-Type: application/json`
  - Body:
    ```json
    {
      "idClienteRemitente": 2,
      "idClienteDestinatario": 3,
      "idRuta": 2,
      "pesoRealKg": 12.5,
      "largoCm": 40.0,
      "anchoCm": 30.0,
      "altoCm": 20.0
    }
    ```

- **Respuesta Exitosa (`201 Created`)**:
  ```json
  {
    "success": true,
    "mensaje": "Guía creada exitosamente",
    "guia": {
      "numeroGuia": 1001,
      "idClienteRemitente": 2,
      "idClienteDestinatario": 3,
      "idRuta": 2,
      "pesoRealKg": 12.5,
      "largoCm": 40.0,
      "anchoCm": 30.0,
      "altoCm": 20.0,
      "estado": "Creada"
    }
  }
  ```

- **Respuesta de Error - Referencia Inexistente (`404 Not Found`)**:
  ```json
  {
    "success": false,
    "mensaje": "El cliente remitente con ID 999 no existe o está inactivo"
  }
  ```

- **Respuesta de Error - Validación Inválida (`400 Bad Request`)**:
  ```json
  {
    "success": false,
    "mensaje": "El cliente remitente y destinatario no pueden ser el mismo"
  }
  ```

---

### 4.2 GET `/v1/transacciones/guias`

Obtiene la lista de todas las guías activas.

- **Respuesta Exitosa (`200 OK`)**:
  ```json
  {
    "success": true,
    "guias": [
      {
        "numeroGuia": 1001,
        "idClienteRemitente": 2,
        "idClienteDestinatario": 3,
        "idRuta": 2,
        "pesoRealKg": 12.5,
        "largoCm": 40.0,
        "anchoCm": 30.0,
        "altoCm": 20.0,
        "estado": "Creada"
      }
    ]
  }
  ```

---

### 4.3 GET `/v1/transacciones/guias/{numeroGuia}`

Obtiene la información de una guía por su número único.

- **Respuesta Exitosa (`200 OK`)**:
  ```json
  {
    "success": true,
    "guia": {
      "numeroGuia": 1001,
      "idClienteRemitente": 2,
      "idClienteDestinatario": 3,
      "idRuta": 2,
      "pesoRealKg": 12.5,
      "largoCm": 40.0,
      "anchoCm": 30.0,
      "altoCm": 20.0,
      "estado": "Creada"
    }
  }
  ```

- **Respuesta de Error (`404 Not Found`)**:
  ```json
  {
    "success": false,
    "mensaje": "Guía número 9999 no encontrada o inactiva"
  }
  ```
