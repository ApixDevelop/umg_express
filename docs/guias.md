# Módulo de Emisión y Gestión de Guías - UMG Express

## 1. Descripción General

El módulo de Guías de Envío (Fases 5A y 5B) gestiona la creación, numeración secuencial correlativa, cálculo automático de peso volumétrico, peso facturable, costo total estimado y consulta de paquetes de envío en UMG Express.
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
    double pesoVolumetricoKg; // Peso volumétrico calculado: (largo*ancho*alto)/5000 (8 bytes)
    double pesoFacturableKg;  // Peso facturable: max(pesoReal, pesoVolumetrico) (8 bytes)
    double costoTotal;        // Costo estimado total de envío en Q (8 bytes)
    char estado[20];          // "Creada", "Despachada", "Entregada" (20 bytes)
    bool activo;              // Estado activo/inactivo para baja lógica (1 byte + padding)
};
```

---

## 3. Lógica de Negocio y Cálculo (`include/transacciones.h`)

- **Numeración Correlativa**: Los números de guía se generan automáticamente escaneando `data/guias.dat` y asignando `max(numeroGuia) + 1` (iniciando en 1001).
- **Cálculo de Peso Volumétrico**:
  $$\text{pesoVolumetricoKg} = \frac{\text{largoCm} \times \text{anchoCm} \times \text{altoCm}}{5000}$$
- **Cálculo de Peso Facturable**:
  $$\text{pesoFacturableKg} = \max(\text{pesoRealKg}, \text{pesoVolumetricoKg})$$
- **Cálculo del Costo Total**:
  $$\text{costoTotal} = \text{tarifaBaseRuta} + (\text{pesoFacturableKg} \times \text{precioPorKg}) + (\text{volumenM3} \times \text{precioPorM3}) + \text{recargoEspecial}$$
- **Validaciones Relacionales y de Formato**:
  - `idClienteRemitente` debe existir y estar activo en `data/clientes.dat`.
  - `idClienteDestinatario` debe existir y estar activo en `data/clientes.dat`.
  - `idClienteRemitente` y `idClienteDestinatario` no pueden ser el mismo cliente.
  - `idRuta` debe existir y estar activa en `data/rutas.dat`.
  - `pesoRealKg` debe ser mayor a 0 kg.
  - Dimensiones (`largoCm`, `anchoCm`, `altoCm`) deben ser mayores a 0 cm.
- **Estado Inicial**: Cada nueva guía se registra automáticamente con el estado `"Creada"`.

---

## 4. Endpoints HTTP REST

### 4.1 POST `/v1/transacciones/guias`

Registra una nueva guía de envío y calcula automáticamente peso volumétrico, peso facturable y costo total.

- **Solicitud**:
  - `Content-Type: application/json`
  - Body:
    ```json
    {
      "idClienteRemitente": 2,
      "idClienteDestinatario": 3,
      "idRuta": 2,
      "pesoRealKg": 5.0,
      "largoCm": 60.0,
      "anchoCm": 50.0,
      "altoCm": 40.0,
      "idTarifa": 1
    }
    ```

- **Respuesta Exitosa (`201 Created`)**:
  ```json
  {
    "success": true,
    "mensaje": "Guía creada exitosamente con peso volumétrico y costo calculados",
    "guia": {
      "numeroGuia": 1001,
      "idClienteRemitente": 2,
      "idClienteDestinatario": 3,
      "idRuta": 2,
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
    "mensaje": "Las dimensiones del paquete (largo, ancho, alto) deben ser mayores a 0 cm"
  }
  ```

---

### 4.2 GET `/v1/transacciones/guias`

Obtiene la lista de todas las guías activas con sus campos calculados.

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

