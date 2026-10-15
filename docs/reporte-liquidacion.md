# Reporte de Liquidación Financiera y Operativa — UMG Express

## 1. Descripción General

El reporte de liquidación proporciona un resumen consolidado de las guías de envío evaluadas en el sistema, sumando sus importes registrados (`costoTotal`), agrupando el volumen de guías por su estado actual (`Creada`, `Despachada`, etc.), por ruta y por sucursal de origen.

Utiliza los costos ya calculados y almacenados en las guías de la persistencia binaria (`data/guias.dat`, `data/rutas.dat`, `data/sucursales.dat`) sin recalcular tarifas antiguas ni alterar los registros existentes.

---

## 2. Endpoint REST

```text
GET /v1/reportes/liquidacion
```

---

## 3. Estructura de Respuesta JSON

### 3.1 Respuesta con Guías en el Sistema (`HTTP 200 OK`)

```json
{
  "success": true,
  "liquidacion": {
    "resumenFinanciero": {
      "totalGuiasConsideradas": 2,
      "totalGuiasInactivas": 1,
      "importeTotalFacturado": 440.3,
      "cobradoConfirmado": 0.0
    },
    "guiasPorEstado": [
      {
        "estado": "Creada",
        "cantidadGuias": 1,
        "importeTotal": 142.8
      },
      {
        "estado": "Despachada",
        "cantidadGuias": 1,
        "importeTotal": 297.5
      }
    ],
    "resumenPorRuta": [
      {
        "idRuta": 1,
        "cantidadGuias": 2,
        "importeTotal": 440.3
      }
    ],
    "resumenPorSucursalOrigen": [
      {
        "idSucursal": 1,
        "nombre": "Sucursal Guatemala Central",
        "cantidadGuias": 2,
        "importeTotal": 440.3
      }
    ],
    "desgloseGuias": [
      {
        "numeroGuia": 1001,
        "estado": "Creada",
        "pesoFacturableKg": 12.0,
        "costoTotal": 142.8,
        "idClienteRemitente": 1,
        "idClienteDestinatario": 2,
        "idRuta": 1,
        "sucursalOrigen": "Sucursal Guatemala Central"
      },
      {
        "numeroGuia": 1002,
        "estado": "Despachada",
        "pesoFacturableKg": 25.0,
        "costoTotal": 297.5,
        "idClienteRemitente": 1,
        "idClienteDestinatario": 2,
        "idRuta": 1,
        "sucursalOrigen": "Sucursal Guatemala Central"
      }
    ]
  }
}
```

### 3.2 Respuesta sin Guías en el Sistema (`HTTP 200 OK`)

```json
{
  "success": true,
  "liquidacion": {
    "resumenFinanciero": {
      "totalGuiasConsideradas": 0,
      "totalGuiasInactivas": 0,
      "importeTotalFacturado": 0.0,
      "cobradoConfirmado": 0.0
    },
    "guiasPorEstado": [],
    "resumenPorRuta": [],
    "resumenPorSucursalOrigen": [],
    "desgloseGuias": []
  }
}
```

---

## 4. Definiciones y Reglas Financieras

1. **Importe Total Facturado (`importeTotalFacturado`)**:
   $$\text{Importe Total Facturado} = \sum_{\text{guia.activo} = \text{true}} \text{guia.costoTotal}$$
   - Mantiene estrictamente el valor numérico registrado en cada `Guia` al momento de su emisión.
   - Evita recálculos con tarifas actuales para preservar la integridad histórica de la cobranza.

2. **Cobranza Confirmada (`cobradoConfirmado`)**:
   $$\text{Cobranza Confirmada} = \sum_{\substack{\text{guia.activo} = \text{true} \\ \text{guia.estado} = \text{"Entregada"}}} \text{guia.costoTotal}$$
   - Suma el importe de todas las guías que han completado su ciclo logístico y han alcanzado el estado `"Entregada"`.
   - Refleja la cobranza efectivamente cerrada en destino. Las guías en estado `"Creada"` o `"Despachada"` se consideran pendientes de cobro/entrega.

3. **Manejo de Guías Inactivas / Canceladas (`totalGuiasInactivas`)**:
   - Las guías con `activo == false` son contabilizadas en la métrica `totalGuiasInactivas`.
   - Sus importes son excluidos de `importeTotalFacturado` y de las agrupaciones por estado para no inflar los ingresos reales del sistema.
