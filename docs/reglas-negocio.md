# Reglas de Negocio - UMG Express

## 1. Usuarios

Cada usuario debe tener un identificador único.

Los usuarios pueden tener diferentes roles:

* Administrador
* Operador
* Auditor

Un usuario inactivo no debe poder autenticarse.

---

## 2. Baja lógica

Las entidades que puedan ser eliminadas deben utilizar baja lógica.

La eliminación consiste en cambiar:

```cpp
activo = false;
```

El registro debe permanecer físicamente en el archivo `.dat`.

Las consultas normales deben ignorar registros inactivos.

---

## 3. Identificadores

Los identificadores deben ser únicos dentro de su respectivo catálogo.

Antes de insertar un registro:

1. Buscar el ID.
2. Verificar que no exista un registro activo con ese ID.
3. Insertar solamente si es válido.

---

## 4. Peso volumétrico

El sistema debe calcular el peso volumétrico utilizando las dimensiones del paquete.

Fórmula definida por el proyecto:

```text
peso volumétrico =
(largo × ancho × alto) / 5000
```

El peso facturable debe corresponder al mayor entre:

* Peso real.
* Peso volumétrico.

---

## 5. Costo

El costo final de una guía debe calcularse utilizando la información correspondiente de tarifas y carga.

Los recargos aplicables deben considerarse en el cálculo.

---

## 6. Vehículos

Cada vehículo posee límites de:

* Peso.
* Volumen.

El vehículo puede encontrarse en estados como:

* Disponible.
* En ruta.
* En taller.

---

## 7. Despacho

Antes de realizar un despacho se debe comprobar la capacidad disponible del vehículo.

La operación debe rechazarse si:

```text
peso acumulado + nuevo peso > capacidad máxima
```

o:

```text
volumen acumulado + nuevo volumen > capacidad máxima
```

El sistema no debe permitir que un despacho exceda las capacidades físicas del vehículo.

---

## 8. Integridad

No realizar operaciones que puedan generar:

* IDs duplicados.
* Guías inexistentes.
* Vehículos inexistentes.
* Sucursales inexistentes.
* Pilotos inexistentes.
* Registros corruptos.
* Cargas superiores a la capacidad del vehículo.

---

## 9. Persistencia

Las operaciones sobre archivos deben mantener los registros de tamaño fijo.

Cuando se actualice un registro existente:

1. Localizar el registro.
2. Posicionarse con `fseek`.
3. Modificar el registro.
4. Escribir nuevamente mediante `fwrite`.

---

## 10. Rastreo

Cada guía debe poder identificarse mediante su número de guía.

El sistema debe permitir consultar la información asociada a una guía.

---

## 11. Reportes

Los reportes deben utilizar información almacenada en los archivos `.dat`.

No crear datos ficticios para completar reportes.
