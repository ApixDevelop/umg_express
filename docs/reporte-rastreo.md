# Reporte de Rastreo de Guías — UMG Express

## 1. Descripción General

El reporte de rastreo de guías permite consultar el estado actual de un envío mediante su número correlativo de guía (`numeroGuia`). 
Reutiliza las entidades existentes (`Guia`, `Cliente`, `Ruta`, `Sucursal`, `Despacho`, `Vehiculo`, `Piloto`) y sus archivos binarios `.dat` asociados sin inventar estados ficticios ni depender de bases de datos externas.

---

## 2. Endpoint REST

```text
GET /v1/reportes/rastreo/{numeroGuia}
```

### Parámetros de Ruta
* `numeroGuia` (entero): Número correlativo de la guía a consultar (ej: `1001`).

---

## 3. Estructura de Respuesta JSON

### 3.1 Guía en estado `"Creada"` (sin despachar)

Cuando una guía ha sido emitida pero aún no ha sido asignada a un despacho:

```json
{
  "success": true,
  "rastreo": {
    "numeroGuia": 1001,
    "estado": "Creada",
    "fechaEmision": "2026-10-09 09:30:00",
    "paquete": {
      "pesoRealKg": 5.0,
      "largoCm": 60.0,
      "anchoCm": 50.0,
      "altoCm": 40.0,
      "pesoVolumetricoKg": 24.0,
      "pesoFacturableKg": 24.0,
      "costoTotal": 285.6
    },
    "remitente": {
      "id": 1,
      "nombre": "Juan Pérez",
      "nit": "1234567-8",
      "telefono": "5555-1234"
    },
    "destinatario": {
      "id": 2,
      "nombre": "Maria Lopez",
      "nit": "8765432-1",
      "telefono": "5555-4321"
    },
    "ruta": {
      "id": 1,
      "nombre": "Ruta Central - Guatemala a Quetzaltenango",
      "sucursalOrigen": {
        "id": 1,
        "nombre": "Central Guatemala",
        "direccion": "Zona 1, Ciudad de Guatemala"
      },
      "sucursalDestino": {
        "id": 2,
        "nombre": "Sucursal Xela",
        "direccion": "Zona 3, Quetzaltenango"
      }
    },
    "despachada": false,
    "despacho": null
  }
}
```

### 3.2 Guía en estado `"Despachada"` (asignada a vehículo)

Cuando la guía ha sido despachada mediante `POST /v1/transacciones/despacho`, se incluyen los detalles del vehículo y del piloto a cargo:

```json
{
  "success": true,
  "rastreo": {
    "numeroGuia": 1001,
    "estado": "Despachada",
    "fechaEmision": "2026-10-09 09:30:00",
    "paquete": {
      "pesoRealKg": 5.0,
      "largoCm": 60.0,
      "anchoCm": 50.0,
      "altoCm": 40.0,
      "pesoVolumetricoKg": 24.0,
      "pesoFacturableKg": 24.0,
      "costoTotal": 285.6
    },
    "remitente": {
      "id": 1,
      "nombre": "Juan Pérez",
      "nit": "1234567-8",
      "telefono": "5555-1234"
    },
    "destinatario": {
      "id": 2,
      "nombre": "Maria Lopez",
      "nit": "8765432-1",
      "telefono": "5555-4321"
    },
    "ruta": {
      "id": 1,
      "nombre": "Ruta Central - Guatemala a Quetzaltenango",
      "sucursalOrigen": {
        "id": 1,
        "nombre": "Central Guatemala",
        "direccion": "Zona 1, Ciudad de Guatemala"
      },
      "sucursalDestino": {
        "id": 2,
        "nombre": "Sucursal Xela",
        "direccion": "Zona 3, Quetzaltenango"
      }
    },
    "despachada": true,
    "despacho": {
      "idDespacho": 1,
      "fechaDespacho": "2026-10-09 10:15:00",
      "vehiculo": {
        "id": 1,
        "placa": "P-123ABC",
        "modelo": "Isuzu NPR 2022",
        "estado": "En ruta"
      },
      "piloto": {
        "id": 1,
        "nombre": "Mario Estrada",
        "licencia": "LIC-12345",
        "telefono": "5555-1111"
      }
    }
  }
}
```

---

## 4. Respuestas de Error HTTP

### 4.1 Guía No Encontrada o Inactiva (`HTTP 404 Not Found`)

```json
{
  "success": false,
  "mensaje": "Guía número 9999 no encontrada o inactiva"
}
```

---

## 5. Limitaciones Reales del Rastreo Actual

1. **Estados Soportados**: En la presente fase (Fase 6A), las guías sólo registran los estados `"Creada"` y `"Despachada"`. Estados como `"En tránsito"`, `"En sucursal de destino"` o `"Entregada"` se incorporarán cuando existan en los flujos operativos correspondientes.
2. **Historial de Puntos de Control**: No se generan eventos ficticios de rastreo GPS ni checkpoints intermedios que no existan en la persistencia física (`data/guias.dat` y `data/despachos.dat`).
3. **Manejo de Archivos**: Las lecturas binarias son tolerantes a fallos de IO; si un archivo auxiliar (como piloto o vehículo) se encuentra corrupto o faltante, la respuesta omite la sub-sección defectuosa en lugar de hacer fallar el servidor HTTP.
