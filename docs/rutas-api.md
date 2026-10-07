# API REST - UMG Express

## 1. Información general

La API utiliza Crow como framework HTTP.

Todos los endpoints utilizan el prefijo:

```text
/v1/
```

Las solicitudes y respuestas utilizan JSON cuando corresponda.

---

# 2. Autenticación

## POST `/v1/auth/login`

Valida las credenciales del usuario almacenado en:

```text
data/usuarios.dat
```

Roles contemplados:

* Administrador
* Operador
* Auditor

---

## POST `/v1/auth/usuarios`

Registra un nuevo usuario.

Debe validar que el identificador sea único.

---

# 3. Sucursales

## GET `/v1/sucursales`

Obtiene las sucursales activas.

## POST `/v1/sucursales`

Registra una nueva sucursal.

Debe validar que el ID sea único.

## PUT `/v1/sucursales/{id}`

Actualiza una sucursal existente.

La actualización debe realizarse respetando el tamaño fijo del registro.

## DELETE `/v1/sucursales/{id}`

Realiza una baja lógica.

No eliminar físicamente el registro.

---

# 4. Clientes

## GET `/v1/clientes`

Obtiene los clientes activos.

## POST `/v1/clientes`

Registra un nuevo cliente.

## PUT `/v1/clientes/{id}`

Modifica los datos de un cliente.

## DELETE `/v1/clientes/{id}`

Realiza baja lógica.

---

# 5. Rutas

## GET `/v1/rutas`

Obtiene las rutas activas.

## POST `/v1/rutas`

Registra una ruta.

## PUT `/v1/rutas/{id}`

Modifica una ruta.

## DELETE `/v1/rutas/{id}`

Realiza baja lógica.

---

# 6. Vehículos

## GET `/v1/vehiculos`

Obtiene los vehículos activos.

Debe incluir información relacionada con:

* Estado.
* Capacidad de peso.
* Capacidad de volumen.

## POST `/v1/vehiculos`

Registra un vehículo.

## PUT `/v1/vehiculos/{id}`

Modifica un vehículo.

## DELETE `/v1/vehiculos/{id}`

Realiza baja lógica.

---

# 7. Pilotos

## GET `/v1/pilotos`

Obtiene los pilotos activos.

## POST `/v1/pilotos`

Registra un piloto.

## PUT `/v1/pilotos/{id}`

Modifica los datos del piloto.

## DELETE `/v1/pilotos/{id}`

Realiza baja lógica.

---

# 8. Tarifas

## GET `/v1/tarifas`

Obtiene las tarifas.

## POST `/v1/tarifas`

Registra una tarifa.

---

# 9. Guías

## POST `/v1/transacciones/guias`

Registra una nueva guía.

Debe:

1. Recibir las dimensiones.
2. Recibir el peso real.
3. Calcular peso volumétrico.
4. Determinar el peso facturable.
5. Calcular el costo.
6. Guardar el registro en `guias.dat`.

---

# 10. Despacho

## POST `/v1/transacciones/despacho`

Asigna carga a un vehículo.

Debe validar:

* Capacidad máxima de peso.
* Capacidad máxima de volumen.

Si la carga supera la capacidad disponible, la operación debe ser rechazada.

---

# 11. Reportes

## GET `/v1/reportes/rastreo/{numeroGuia}`

Obtiene información de rastreo de una guía.

---

## GET `/v1/reportes/manifiesto/{idVehiculo}`

Genera información consolidada de las guías asignadas a un vehículo.

---

## GET `/v1/reportes/liquidacion`

Obtiene información consolidada de ingresos por sucursal y tipo de servicio.

---

# 12. Convención de respuestas

Las respuestas deben utilizar JSON.

Los errores deben proporcionar información suficiente para identificar el problema.

Ejemplo conceptual:

```json
{
    "success": false,
    "message": "Vehículo sin capacidad disponible"
}
```

No modificar las rutas definidas aquí sin actualizar este documento.
