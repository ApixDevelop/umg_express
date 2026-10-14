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

---

## 12. Fundamentos de Álgebra Lineal y Geometría Analítica

El sistema integra formulaciones vectoriales y geométricas rigurosas (`include/algebra.h`):

1. **Espacios Vectoriales ($\mathbb{R}^2$ y $\mathbb{R}^3$):** Modelado de vectores de dimensiones de carga $\vec{d} = (largo, ancho, alto)$ y vectores de estado de carga/tarifas.
2. **Producto Punto:** Cálculo del costo variable del flete mediante el producto escalar entre el vector de carga $\vec{x} = (peso, volumen)$ y el vector de tarifas $\vec{t} = (P_{kg}, P_{m3})$:
   $$\text{Costo Variable} = \vec{x} \cdot \vec{t} = (p \times P_{kg}) + (v \times P_{m3})$$
3. **Producto Cruz y Cálculo de Áreas:** Cálculo del área superficial de embalaje a partir de aristas ortogonales:
   $$\text{Área} = 2 \cdot (\|\vec{u} \times \vec{v}\| + \|\vec{u} \times \vec{w}\| + \|\vec{v} \times \vec{w}\|)$$
4. **Determinantes y Volúmenes:** Determinante $3 \times 3$ como triple producto escalar para el volumen del paralelepípedo de carga:
   $$\text{Volumen} = |\det(M)| = |\vec{u} \cdot (\vec{v} \times \vec{w})|$$
5. **Ecuación Paramétrica y Simétrica de la Recta:** Modelado continuo de rutas de transporte $\vec{r}(t) = \vec{P}_0 + t \cdot \vec{v}$ con $t \in [0, 1]$ y validación de alineación mediante forma simétrica.
