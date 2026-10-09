# Reporte de Manifiesto de Vehículo — UMG Express

## 1. Descripción General

El reporte de manifiesto de vehículo permite generar la hoja de carga / manifiesto de despacho con la información disponible de un vehículo (`idVehiculo`), su piloto asignado, su ruta de transporte y el detalle de todas las guías de envío asignadas a sus despachos operativos.

Reutiliza la información almacenada en los archivos binarios `.dat` (`vehiculos.dat`, `despachos.dat`, `guias.dat`, `pilotos.dat`, `rutas.dat`, `sucursales.dat`, `clientes.dat`) sin inventar datos ficticios ni depender de sistemas externos.

---

## 2. Endpoint REST

```text
GET /v1/reportes/manifiesto/{idVehiculo}
```

### Parámetros de Ruta
* `idVehiculo` (entero): Identificador único del vehículo a consultar.

---

## 3. Estructura de Respuesta JSON

### 3.1 Vehículo con guías despachadas (`HTTP 200 OK`)

```json
{
  "success": true,
  "manifiesto": {
    "vehiculo": {
      "id": 1,
      "placa": "P-100AAA",
      "marcaModelo": "Isuzu NPR 2022",
      "capacidadPesoKg": 1000.0,
      "capacidadVolumenM3": 10.0,
      "estado": "En ruta"
    },
    "piloto": {
      "id": 1,
      "nombre": "Mario Estrada",
      "licencia": "LIC-12345",
      "telefono": "5555-1111"
    },
    "ruta": {
      "id": 1,
      "sucursalOrigen": {
        "id": 1,
        "nombre": "Sucursal Guatemala Central",
        "direccion": "Zona 1"
      },
      "sucursalDestino": {
        "id": 2,
        "nombre": "Sucursal Quetzaltenango",
        "direccion": "Zona 3 Xela"
      }
    },
    "resumen": {
      "totalGuias": 2,
      "pesoTotalKg": 37.0,
      "volumenTotalM3": 0.18
    },
    "guias": [
      {
        "numeroGuia": 1001,
        "estado": "Despachada",
        "pesoRealKg": 10.0,
        "pesoVolumetricoKg": 12.0,
        "pesoFacturableKg": 12.0,
        "volumenM3": 0.06,
        "costoTotal": 142.8,
        "remitente": {
          "id": 1,
          "nombre": "Distribuidora Central S.A."
        },
        "destinatario": {
          "id": 2,
          "nombre": "Comercial Xela Ltda"
        }
      },
      {
        "numeroGuia": 1002,
        "estado": "Despachada",
        "pesoRealKg": 25.0,
        "pesoVolumetricoKg": 24.0,
        "pesoFacturableKg": 25.0,
        "volumenM3": 0.12,
        "costoTotal": 297.5,
        "remitente": {
          "id": 1,
          "nombre": "Distribuidora Central S.A."
        },
        "destinatario": {
          "id": 2,
          "nombre": "Comercial Xela Ltda"
        }
      }
    ]
  }
}
```

### 3.2 Vehículo existente sin guías asignadas (`HTTP 200 OK`)

Cuando un vehículo existe y está activo pero no ha realizado despachos ni tiene guías asignadas:

```json
{
  "success": true,
  "manifiesto": {
    "vehiculo": {
      "id": 3,
      "placa": "P-300CCC",
      "marcaModelo": "Volvo FM 400",
      "capacidadPesoKg": 3000.0,
      "capacidadVolumenM3": 30.0,
      "estado": "Disponible"
    },
    "piloto": null,
    "ruta": null,
    "resumen": {
      "totalGuias": 0,
      "pesoTotalKg": 0.0,
      "volumenTotalM3": 0.0
    },
    "guias": []
  }
}
```

---

## 4. Respuestas de Error HTTP

### 4.1 Vehículo No Encontrado o Inactivo (`HTTP 404 Not Found`)

```json
{
  "success": false,
  "mensaje": "Vehículo ID 9999 no encontrado o inactivo"
}
```

---

## 5. Limitaciones Reales de los Datos

1. **Agrupación por Vehículo**: El manifiesto agrupa todas las guías activamente despachadas en el vehículo sin duplicar guías si existieron múltiples registros de despacho o reintentos.
2. **Volumen de Carga**: El volumen individual de cada paquete en $m^3$ se calcula directamente a partir de las dimensiones almacenadas en `Guia` (`largoCm * anchoCm * altoCm / 1000000.0`).
3. **Piloto y Ruta Asignados**: Se extrae el piloto asignado en los despachos registrados. Si no se ha asignado un piloto en el despacho, la sección `piloto` retorna `null`.
