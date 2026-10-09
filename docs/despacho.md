# Módulo de Despacho y Validación de Capacidad - UMG Express

## 1. Descripción General

El módulo de Despacho (Fase 5C) coordina la asignación de guías de envío registradas (`"Creada"`) a vehículos de transporte disponibles, realizando un control estricto del peso y volumen acumulados frente a las capacidades físicas del vehículo asignado.

La información de las asignaciones de despacho se almacena binariamente en `data/despachos.dat` mediante registros de tamaño constante (`struct Despacho`).

---

## 2. Estructura de Datos (`include/structs.h`)

```cpp
struct Despacho {
    int id;                // Identificador único de registro de despacho (4 bytes)
    int idVehiculo;        // ID del vehículo asignado (4 bytes)
    int idPiloto;          // ID del piloto asignado (4 bytes, opcional)
    int numeroGuia;        // Número correlativo de la guía despachada (4 bytes)
    double pesoKg;         // Peso facturable de la guía (8 bytes)
    double volumenM3;      // Volumen en metros cúbicos de la guía (8 bytes)
    char fechaDespacho[30];// Cadena con fecha/hora del despacho (30 bytes)
    bool activo;           // Estado activo/inactivo para baja lógica (1 byte + padding)
};
```

---

## 3. Reglas de Negocio y Validaciones (`include/transacciones.h`)

1. **Validación del Vehículo**:
   - El vehículo debe existir y estar activo (`activo == true`) en `data/vehiculos.dat`.
   - El vehículo no puede estar en estado `"En taller"`.

2. **Validación del Piloto**:
   - Si se especifica `idPiloto > 0`, el piloto debe existir y estar activo (`activo == true`) en `data/pilotos.dat`.

3. **Validación de la Carga (Guías)**:
   - Cada número de guía debe existir y estar activo (`activo == true`).
   - La guía debe encontrarse en estado `"Creada"`. Se rechaza si la guía ya fue `"Despachada"` o `"Entregada"`.
   - No se permiten números de guía duplicados en una misma solicitud.

4. **Control Acumulado de Capacidad Vehicular**:
   - **Peso Acumulado**: Suma de `pesoFacturableKg` de las guías del lote.
     $$\text{pesoAcumulado} \le \text{vehiculo.capacidadPesoKg}$$
   - **Volumen Acumulado**: Suma de `(largo * ancho * alto) / 1000000.0` en $m^3$ de las guías del lote.
     $$\text{volumenAcumulado} \le \text{vehiculo.capacidadVolumenM3}$$
   - **Bloqueo Automático**: Si el peso o el volumen superan los límites del vehículo, la operación es **rechazada automáticamente** (`400 Bad Request`).

5. **Actualización de Estados**:
   - El estado de cada guía despachada cambia de `"Creada"` a `"Despachada"`.
   - El estado del vehículo cambia a `"En ruta"`.

---

## 4. Endpoint REST HTTP

### 4.1 POST `/v1/transacciones/despacho`

Procesa una orden de despacho asignando una o más guías a un vehículo.

- **Solicitud**:
  - `Content-Type: application/json`
  - Body:
    ```json
    {
      "idVehiculo": 1,
      "idPiloto": 1,
      "guias": [1001, 1002]
    }
    ```

- **Respuesta Exitosa (`201 Created`)**:
  ```json
  {
    "success": true,
    "mensaje": "Despacho realizado exitosamente y vehículo actualizado a 'En ruta'",
    "despacho": {
      "idVehiculo": 1,
      "idPiloto": 1,
      "totalGuias": 2,
      "pesoTotalKg": 70.0,
      "volumenTotalM3": 0.18,
      "estadoVehiculo": "En ruta",
      "guiasDespachadas": [1001, 1002]
    }
  }
  ```

- **Respuesta de Error - Capacidad Excedida (`400 Bad Request`)**:
  ```json
  {
    "success": false,
    "mensaje": "Despacho rechazado: El peso acumulado (120.0 kg) excede la capacidad máxima del vehículo (100.0 kg)"
  }
  ```

- **Respuesta de Error - Vehículo en Taller (`400 Bad Request`)**:
  ```json
  {
    "success": false,
    "mensaje": "El vehículo con ID 3 está en taller y no puede despacharse"
  }
  ```

- **Respuesta de Error - Guía ya Despachada (`400 Bad Request`)**:
  ```json
  {
    "success": false,
    "mensaje": "La guía número 1001 ya se encuentra despachada"
  }
  ```
