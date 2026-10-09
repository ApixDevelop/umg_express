# Fórmulas y Cálculos Matemáticos de Guías - UMG Express

## 1. Descripción General

En la **Fase 5B**, UMG Express incorpora el motor de cálculo matemático para evaluar automáticamente el peso volumétrico, el peso facturable y el costo total estimado de las guías de envío registradas en el sistema.

---

## 2. Fórmulas Matemáticas

### 2.1 Peso Volumétrico (`pesoVolumetricoKg`)

El peso volumétrico refleja la densidad del espacio ocupado por el paquete en relación con su peso físico.

$$\text{pesoVolumetricoKg} = \frac{\text{largoCm} \times \text{anchoCm} \times \text{altoCm}}{5000}$$

- **Unidades**:
  - `largoCm`, `anchoCm`, `altoCm`: centímetros ($\text{cm}$).
  - Divisor internacional estandarizado: $5000$.
  - Resultado: kilogramos ($\text{kg}$).

---

### 2.2 Peso Facturable (`pesoFacturableKg`)

Para garantizar el cobro justo según espacio u ocupación de carga, el peso facturable corresponde al valor máximo entre el peso real medido en báscula y el peso volumétrico calculado.

$$\text{pesoFacturableKg} = \max(\text{pesoRealKg}, \text{pesoVolumetricoKg})$$

---

### 2.3 Volumen en Metros Cúbicos (`volumenM3`)

$$\text{volumenM3} = \frac{\text{largoCm} \times \text{anchoCm} \times \text{altoCm}}{1000000}$$

---

### 2.4 Costo Total del Envío (`costoTotal`)

El costo total integra la tarifa base de la ruta seleccionada con la tarifa asignada por el tipo de carga:

$$\text{costoTotal} = \text{tarifaBaseRuta} + (\text{pesoFacturableKg} \times \text{precioPorKg}) + (\text{volumenM3} \times \text{precioPorM3}) + \text{recargoEspecial}$$

- **Componentes**:
  - `tarifaBaseRuta`: Tarifa fija de la ruta asignada a la guía (proveniente de `data/rutas.dat`).
  - `precioPorKg`: Precio por kilogramo facturable de la tarifa seleccionada (proveniente de `data/tarifas.dat`).
  - `precioPorM3`: Precio por metro cúbico ocupado de la tarifa seleccionada (proveniente de `data/tarifas.dat`).
  - `recargoEspecial`: Recargo adicional aplicable según la tarifa del paquete (proveniente de `data/tarifas.dat`).
  - *Nota*: En ausencia de una tarifa específica, se aplica una tasa predeterminada de Q10.00 por kg facturable sobre la tarifa base de la ruta.

---

## 3. Ejemplos de Cálculo

### Ejemplo A: Paquete Pequeño y Pesado (Herramientas metálicas)
- **Dimensiones**: $20 \text{ cm} \times 15 \text{ cm} \times 10 \text{ cm}$
- **Peso real**: $10.0 \text{ kg}$
- **Calculo Volumétrico**: $(20 \times 15 \times 10) / 5000 = 0.6 \text{ kg}$
- **Peso Facturable**: $\max(10.0, 0.6) = 10.0 \text{ kg}$ (Aplica peso real)

### Ejemplo B: Paquete Grande y Ligero (Almohadillas decorativas)
- **Dimensiones**: $60 \text{ cm} \times 50 \text{ cm} \times 40 \text{ cm}$
- **Peso real**: $3.0 \text{ kg}$
- **Calculo Volumétrico**: $(60 \times 50 \times 40) / 5000 = 24.0 \text{ kg}$
- **Peso Facturable**: $\max(3.0, 24.0) = 24.0 \text{ kg}$ (Aplica peso volumétrico)

---

## 4. Validaciones de Seguridad y Límites

- **Dimensiones Mínimas**: `largoCm`, `anchoCm`, `altoCm` deben ser estrictamente $> 0 \text{ cm}$.
- **Peso Mínimo**: `pesoRealKg` debe ser estrictamente $> 0 \text{ kg}$.
- **Límites Máximos**: Se rechazan dimensiones o pesos superiores a $100,000$ unidades para evitar valores desbordados o inconsistentes.
