# Informe Académico: Integración de Álgebra Lineal en el Sistema Logístico UMG Express

---

## 5.3 Marco Teórico

El presente marco teórico sustenta rigurosamente los conceptos matemáticos del curso de **Álgebra Lineal y Geometría Analítica** aplicados a la ingeniería de software y la optimización de procesos en el sistema logístico de paquetería **UMG Express**.

### 1. Espacios Vectoriales ($\mathbb{R}^n$) y Vectores
Un espacio vectorial real $\mathbb{R}^n$ es un conjunto no vacío de elementos llamados vectores, sobre el cual se definen dos operaciones cerradas: la suma de vectores y la multiplicación por un escalar real, satisfaciendo los diez axiomas clásicos de cerradura, asociatividad, conmutatividad, elementos neutros, inversos y distributividad (Grossman & Flores, 2019).
* En $\mathbb{R}^2$, un vector $\vec{x} = (x_1, x_2)$ representa un par ordenado de magnitudes continuas. En el dominio del transporte, permite agrupar variables físicas heterogéneas de un paquete (como masa en kg y volumen en $\text{m}^3$) como una entidad única tratable matemáticamente.
* En $\mathbb{R}^3$, un vector $\vec{v} = (x, y, z)$ define tanto la posición espacial de un punto como las dimensiones físicas tridimensionales de un cuerpo sólido.

### 2. Producto Punto (Producto Escalar)
Sean $\vec{u} = (u_1, u_2, \dots, u_n)$ y $\vec{v} = (v_1, v_2, \dots, v_n)$ dos vectores en $\mathbb{R}^n$. El producto punto o producto interno euclidiano se define algebraicamente como la suma del producto de sus componentes homólogas (Lay, Lay, & McDonald, 2016):

$$\vec{u} \cdot \vec{v} = \sum_{i=1}^n u_i v_i = u_1 v_1 + u_2 v_2 + \dots + u_n v_n$$

Geométricamente, $\vec{u} \cdot \vec{v} = \|\vec{u}\| \|\vec{v}\| \cos(\theta)$, donde $\theta$ es el ángulo entre ellos. En aplicaciones económicas y de costeo logístico, el producto punto actúa como un operador de combinación lineal o funcional de proyección, permitiendo calcular sumas ponderadas multivariables (por ejemplo, vector de consumo por vector de costos unitarios) en una sola operación vectorial formal.

### 3. Producto Cruz (Producto Vectorial)
Sean $\vec{u} = (u_1, u_2, u_3)$ y $\vec{v} = (v_1, v_2, v_3)$ dos vectores en $\mathbb{R}^3$. El producto cruz $\vec{u} \times \vec{v}$ es un operador bilineal y anticonmutativo que produce un tercer vector ortogonal a ambos vectores originales (Anton & Rorres, 2014):

$$\vec{u} \times \vec{v} = \det \begin{pmatrix} \hat{i} & \hat{j} & \hat{k} \\ u_1 & u_2 & u_3 \\ v_1 & v_2 & v_3 \end{pmatrix} = (u_2 v_3 - u_3 v_2)\hat{i} - (u_1 v_3 - u_3 v_1)\hat{j} + (u_1 v_2 - u_2 v_1)\hat{k}$$

La norma del producto vectorial $\|\vec{u} \times \vec{v}\|$ equivale al área del paralelogramo sustentado por $\vec{u}$ y $\vec{v}$. Su aplicación directa radica en el cálculo de áreas superficiales de embalaje y contención de paquetes ortogonales y poligonales.

### 4. Determinantes y Triple Producto Escalar (Cálculo de Volúmenes)
El determinante de una matriz cuadrada $A \in \mathbb{R}^{3 \times 3}$ formada por tres vectores fila o columna $\vec{u}, \vec{v}, \vec{w}$ representa el factor de escala del volumen orientado bajo la transformación lineal correspondiente. Geométricamente, el valor absoluto del determinante de orden 3 coincide con el **Triple Producto Escalar** (Strang, 2016):

$$\text{Volumen} = |\det([\vec{u} \quad \vec{v} \quad \vec{w}])| = |\vec{u} \cdot (\vec{v} \times \vec{w})|$$

Para un paralelepípedo rectangular generado por aristas cartesianas directoras $\vec{u} = (l, 0, 0)$, $\vec{v} = (0, w, 0)$ y $\vec{w} = (0, 0, h)$, el determinante evalúa exactamente el volumen cúbico $V = l \cdot w \cdot h$, generalizable analíticamente a cualquier orientación y deformación espacial del sólido en el espacio $\mathbb{R}^3$.

### 5. Ecuación Paramétrica y Simétrica de una Recta en $\mathbb{R}^3$
Una recta en el espacio tridimensional queda unívocamente determinada por un punto base $\vec{P}_0 = (x_0, y_0, z_0)$ y un vector director no nulo $\vec{v} = (v_x, v_y, v_z)$ (Grossman & Flores, 2019):
* **Ecuación Vectorial/Paramétrica:**
  $$\vec{r}(t) = \vec{P}_0 + t \cdot \vec{v} \iff \begin{cases} x(t) = x_0 + t \cdot v_x \\ y(t) = y_0 + t \cdot v_y \\ z(t) = z_0 + t \cdot v_z \end{cases}, \quad t \in \mathbb{R}$$
* **Ecuación Simétrica:** Al despejar el parámetro escalar $t$ (asumiendo componentes directores no nulos):
  $$\frac{x - x_0}{v_x} = \frac{y - y_0}{v_y} = \frac{z - z_0}{v_z} = t$$
Permite modelar analíticamente trayectorias de desplazamiento vehicular entre nodos logísticos (sucursales), facilitando el rastreo continuo del avance temporal $t \in [0, 1]$.

#### Referencias Bibliográficas del Marco Teórico
* Anton, H., & Rorres, C. (2014). *Elementary Linear Algebra: Applications Version* (11th ed.). Wiley.
* Grossman, S. I., & Flores, J. J. (2019). *Álgebra Lineal* (8va ed.). McGraw-Hill Education.
* Lay, D. C., Lay, S. R., & McDonald, J. J. (2016). *Linear Algebra and Its Applications* (5th ed.). Pearson.
* Strang, G. (2016). *Introduction to Linear Algebra* (5th ed.). Wellesley-Cambridge Press.

---

## 5.4 Problema, Situación o Proyecto a Resolver o Analizar

### a. Nombre de la propuesta
**Modelo Vectorial y Matricial para el Control de Capacidad Vehicular y Tarificación Multivariable en el Sistema Logístico UMG Express.**

### b. Descripción del problema
En los sistemas de logística y transporte de encomiendas, dos de las operaciones más críticas y propensas a errores financieros y operativos son:
1. **La asignación física de carga en vehículos:** Si un vehículo de despacho se carga sobrepasando su volumen cúbico máximo ($m^3$) o su capacidad de masa ($kg$), se generan riesgos mecánicos severos, accidentes y sanciones legales de transporte.
2. **El cálculo heterogéneo de fletes:** Cobrar un envío considerando únicamente el peso real perjudica a la empresa cuando los paquetes son voluminosos pero livianos (por ejemplo, cajas grandes con almohadillas protectoras), y calcularlo mediante reglas aritméticas aisladas dificulta extender el modelo a nuevas tarifas (combustible, recargos especiales, seguros de carga).

### c. Tema de Álgebra Lineal
* **Determinantes $3 \times 3$ y Triple Producto Escalar:** Aplicados al cálculo volumétrico y verificación de restricciones geométricas de carga en paralelepípedos.
* **Espacios Vectoriales ($\mathbb{R}^2$) y Producto Punto:** Aplicados al costeo lineal multivariable de tarifas y fletes.
* **Ecuación Paramétrica de la Recta:** Aplicada a la modelación continua de rutas de despacho entre sucursales.

### d. Aplicación
* Se modelan las dimensiones físicas del paquete como vectores base ortogonales en $\mathbb{R}^3$, calculando su volumen espacial exacto mediante determinantes de tercer orden. Este valor se acumula y se contrasta analíticamente contra el espacio tridimensional útil del vehículo de despacho.
* Se estructuran las propiedades de cobro del paquete como un vector de estado de carga $\vec{C} = (pesoFacturable, volumenM3)$ y las políticas de precio de la empresa como un vector de tarifas $\vec{T} = (precioKg, precioM3)$, obteniendo el flete variable mediante el producto escalar $\vec{C} \cdot \vec{T}$.
* Se modela la ruta de entrega entre sucursales mediante la ecuación vectorial $\vec{r}(t) = \vec{P}_0 + t \cdot \vec{v}$, evaluando la posición estimada del vehículo a partir del parámetro de avance temporal $t$.

### e. Resultado esperado
1. Eliminación de sobrecargas volumétricas en el despacho vehicular mediante validaciones algebraicas automáticas antes de registrar la operación en disco.
2. Un motor de costeo consistente, extensible a $N$ variables de tarificación mediante álgebra vectorial.
3. Precisión analítica verificada al 100% mediante pruebas unitarias automatizadas (`test_algebra`).

---

## 5.5 Aplicación de Álgebra Lineal

### a. Nombre de la propuesta
**Optimización de cubicaje vehicular mediante determinantes tridimensionales y tarificación vectorial por producto escalar.**

### b. Descripción del problema
La gestión de envíos de encomiendas requiere transformar mediciones físicas individuales tomadas en mostrador ($largo, ancho, alto$ en centímetros y masa en kilogramos) en decisiones operativas confiables de asignación de carga vehicular y cobro al cliente, garantizando que el camión asignado satisfaga simultáneamente restricciones de volumen y peso sin incurrir en desbordamientos de capacidad.

### c. Tema de Álgebra Lineal
1. **Determinantes ($3 \times 3$) y Triple Producto Escalar:** Para determinar el volumen ocupado por los cuerpos geométricos de la carga.
2. **Producto Punto (Producto Escalar):** Para calcular la función objetivo de costo total en un espacio bidimensional de tarificación.
3. **Ecuación Paramétrica de la Recta:** Para modelar el vector desplazamiento de la unidad de despacho entre coordenadas nodales de origen y destino.

### d. Aplicación
El sistema encapsula estas operaciones en una librería especializada ([include/algebra.h](file:///Users/josue/Uni/umgExpress/include/algebra.h)):
1. Los paquetes ingresados se transforman en vectores $\vec{u} = (l,0,0), \vec{v} = (0,w,0), \vec{w} = (0,0,h)$; su determinante calcula el volumen en $m^3$. En el proceso de despacho, el algoritmo suma dichos volúmenes y asegura que $\sum V_i \le V_{\text{vehículo}}$.
2. El costo variable se obtiene evaluando el producto interno entre el vector de carga y el vector de tarifas registradas en el catálogo del sistema.
3. La posición espacial o avance relativo del vehículo durante el traslado se interpola resolviendo $\vec{r}(t) = \vec{P}_{\text{origen}} + t(\vec{P}_{\text{destino}} - \vec{P}_{\text{origen}})$.

### e. Resultado esperado
* Aprobación o rechazo matemático estricto del despacho con mensajes de error descriptivos cuando se exceda la capacidad máxima volumétrica.
* Totalización matemática verificable de fletes y reportes de liquidación consistentes.

### Justificación de la Selección
* **Por qué se seleccionó la propuesta:** Porque la paquetería y la logística son naturalmente problemas de geometría y optimización espacial. Las dimensiones y tarifas forman vectores intrínsecos del negocio.
* **Qué problema se pretende resolver:** Errores de sobrecarga en camiones, discrepancias en facturación manual y falta de formalización matemática en algoritmos de asignación de flota.
* **Qué relación tiene con el proyecto del curso pivote:** Se integra directamente con los módulos de transacciones, validaciones de despacho y reportes de manifiesto del sistema en C++17 (`UMG Express`), interactuando de forma limpia con los registros binarios `.dat`.
* **Qué concepto de Álgebra Lineal se utilizará:** Determinantes, producto punto, producto cruz, vectores y rectas paramétricas.
* **Qué aporta Álgebra Lineal al proyecto:** Proporciona formalismo, rigor conceptual y extensibilidad. En lugar de acumular fórmulas aritméticas arbitrarias o dispersas ("código espagueti"), el sistema queda respaldado por leyes matemáticas comprobadas de espacios vectoriales y proyecciones lineales.

---

## 5.6 Materiales a Utilizar

Para el diseño, desarrollo, verificación y despliegue de la propuesta se utilizaron los siguientes recursos:

| Categoría | Recurso / Herramienta | Finalidad en el Proyecto |
| :--- | :--- | :--- |
| **Lenguaje de Programación** | C++17 (Clang / LLVM) | Implementación de bajo nivel de estructuras, persistencia binaria y clases vectoriales matemáticas. |
| **Librería de Red / API** | Crow C++ Framework | Exposición de endpoints RESTful para consumir los cálculos matemáticos desde clientes HTTP / React. |
| **Compilador y Entorno** | Clang++ 17 sobre macOS / Linux | Compilación nativa optimizada mediante el script automatizado [build.sh](file:///Users/josue/Uni/umgExpress/build.sh). |
| **Almacenamiento** | Archivos Binarios `.dat` (`stdio.h`) | Persistencia directa de registros de tamaño fijo mediante punteros de archivo (`fseek`, `fread`, `fwrite`). |
| **Control de Versiones** | Git / GitHub | Control de versiones, trazabilidad de código y sincronización con repositorio institucional. |
| **Herramientas de Prueba** | C++ Assertions (`<cassert>`) | Suites unitarias automatizadas para verificar la precisión decimal y el cumplimiento de axiomas vectoriales. |
| **Entorno y Asistente IA** | Google Antigravity / Gemini 3.8 Flash | Soporte en diseño arquitectónico, estructuración de pruebas unitarias y generación de documentación técnica formal. |

---

## 5.9 Desarrollo de la Aplicación de Álgebra Lineal

### 1. Problema planteado
Un cliente presenta en sucursal un paquete de dimensiones $50\text{ cm} \times 40\text{ cm} \times 30\text{ cm}$ con un peso real de $2.0\text{ kg}$. La tarifa asignada estipula $Q5.00$ por kilogramo facturable, $Q20.00$ por metro cúbico transportado, un recargo especial de $Q10.00$ y una tarifa base de ruta de $Q50.00$. Asimismo, dicho paquete debe despacharse en un vehículo cuya capacidad volumétrica máxima disponible es de $0.05\text{ m}^3$.

Se requiere:
1. Calcular el volumen exacto del paquete y su peso volumétrico internacional.
2. Calcular el costo total del flete mediante producto punto.
3. Determinar algebraicamente si el vehículo tiene capacidad para transportar dicho paquete.

### 2. Datos utilizados
* Dimensiones de aristas: $l = 50.0\text{ cm} = 0.50\text{ m}$, $w = 40.0\text{ cm} = 0.40\text{ m}$, $h = 30.0\text{ cm} = 0.30\text{ m}$.
* Masa del paquete: $m = 2.0\text{ kg}$.
* Constante IATA peso volumétrico: $K = 5000\text{ cm}^3/\text{kg}$.
* Vector de tarifas: $\vec{T} = (5.0, 20.0)$.
* Tarifa base y recargo: $T_{\text{base}} = 50.0$, $R_{\text{esp}} = 10.0$.
* Capacidad del camión: $V_{\text{max}} = 0.05\text{ m}^3$.

### 3. Modelo matemático

#### A. Modelo de Volumen (Determinante y Triple Producto Escalar)
Definiendo los vectores de aristas ortogonales en $\mathbb{R}^3$:
$$\vec{u} = (0.50, 0, 0), \quad \vec{v} = (0, 0.40, 0), \quad \vec{w} = (0, 0, 0.30)$$
$$V = |\det(M)| = |\vec{u} \cdot (\vec{v} \times \vec{w})|$$

#### B. Modelo de Peso Facturable
$$p_{\text{vol}} = \frac{l \cdot w \cdot h}{5000}, \quad p_{\text{fact}} = \max(m, p_{\text{vol}})$$

#### C. Modelo de Flete (Producto Escalar)
Vector de especificaciones de carga $\vec{C} \in \mathbb{R}^2$:
$$\vec{C} = \begin{pmatrix} p_{\text{fact}} \\ V \end{pmatrix}$$
$$\text{Costo Total} = T_{\text{base}} + (\vec{C} \cdot \vec{T}) + R_{\text{esp}}$$

#### D. Modelo de Capacidad Vehicular
$$\text{Condición de Aceptación:} \quad \sum_{i=1}^k V_i \le V_{\text{max}}$$

---

### 4. Procedimiento y 5. Operaciones realizadas

#### Paso I: Cálculo del Volumen mediante Triple Producto Escalar
1. Producto cruz entre las aristas de base y altura:
   $$\vec{v} \times \vec{w} = \det \begin{pmatrix} \hat{i} & \hat{j} & \hat{k} \\ 0 & 0.40 & 0 \\ 0 & 0 & 0.30 \end{pmatrix} = (0.40 \times 0.30)\hat{i} - 0\hat{j} + 0\hat{k} = (0.12, 0, 0)$$
2. Producto punto con la arista longitudinal:
   $$\vec{u} \cdot (\vec{v} \times \vec{w}) = (0.50)(0.12) + (0)(0) + (0)(0) = 0.060\text{ m}^3$$
   **Volumen obtenido:** $V = 0.06\text{ m}^3$.

#### Paso II: Determinación del Peso Facturable
1. Peso volumétrico:
   $$p_{\text{vol}} = \frac{50 \times 40 \times 30}{5000} = \frac{60000}{5000} = 12.0\text{ kg}$$
2. Peso facturable:
   $$p_{\text{fact}} = \max(2.0, 12.0) = 12.0\text{ kg}$$

#### Paso III: Cálculo de Costos mediante Producto Punto
1. Formación del vector de carga:
   $$\vec{C} = \begin{pmatrix} 12.0 \\ 0.06 \end{pmatrix}$$
2. Evaluación del producto punto con el vector de tarifas $\vec{T} = \begin{pmatrix} 5.0 \\ 20.0 \end{pmatrix}$:
   $$\vec{C} \cdot \vec{T} = (12.0 \times 5.0) + (0.06 \times 20.0) = 60.00 + 1.20 = Q61.20$$
3. Suma de componentes escalares de ruta y recargo:
   $$\text{Costo Total} = 50.00 + 61.20 + 10.00 = Q121.20$$

#### Paso IV: Validación de Capacidad Vehicular en Despacho
1. Comparación contra el vehículo disponible:
   $$V = 0.06\text{ m}^3 > V_{\text{max}} = 0.05\text{ m}^3$$
2. Evaluación lógica: La condición $0.06 \le 0.05$ resulta en **FALSO**.
3. El sistema aborta el despacho y genera el mensaje de error:
   `"Despacho rechazado: El volumen acumulado (0.060000 m3) excede la capacidad máxima del vehículo (0.050000 m3)"`.

---

### 6. Software y Herramientas Tecnológicas Utilizadas
* **Lenguaje C++17:** Implementación de las clases en [include/algebra.h](file:///Users/josue/Uni/umgExpress/include/algebra.h).
* **Compilador Clang++:** Verificación sintáctica estricta y generación de código binario.
* **Curl / API Client:** Pruebas de integración HTTP sobre endpoints Crow `/v1/transacciones/guias` y `/v1/transacciones/despacho`.

### 7. Herramientas de Inteligencia Artificial Utilizadas
* **Herramienta:** Asistente IA Antigravity (Google DeepMind).
* **Uso:** Asistencia en la formulación analítica de los operadores de Álgebra Lineal en C++, estructuración del test unitario [tests/test_algebra.cpp](file:///Users/josue/Uni/umgExpress/tests/test_algebra.cpp) y redacción del marco teórico con normas académicas.
* **Validación por el Estudiante:** Todos los cálculos matemáticos fueron contrastados manualmente a mano con lápiz y papel, verificados matemáticamente mediante aserciones unitarias en código (`assert(casiIgual(costoTotal, 121.20))`) y revalidados al compilar y ejecutar el ejecutable `./test_algebra`.

### 8. Resultado Obtenido
* Volumen exacto por determinante: $0.06\text{ m}^3$.
* Costo variable por producto punto: $Q61.20$. Costo total flete: $Q121.20$.
* Rechazo exitoso del despacho por desbordamiento volumétrico de $0.01\text{ m}^3$.

### 9. Interpretación del Resultado
La formalización algebraica impidió un despacho físicamente inviable en un camión subdimensionado ($0.05\text{ m}^3$). A su vez, el producto punto garantizó que se cobrara por los $12.0\text{ kg}$ volumétricos y no por los $2.0\text{ kg}$ físicos, asegurando la rentabilidad del viaje.

### 10. Relación con el Proyecto del Curso Pivote (Algoritmos)
Demuestra cómo los tipos de datos primitivos (`double`, `struct`) y los algoritmos condicionales de validación se fundamentan en un modelo matemático previo que justifica y valida la lógica de programación.

---

## 5.10 Integración con el Curso Pivote (Algoritmos)

| Componente | Curso Pivote (Algoritmos) | Curso Integrado (Álgebra Lineal) | Aporte / Sinergia |
| :--- | :--- | :--- | :--- |
| **Entidad Guía** | `struct Guia` con campos `largoCm`, `anchoCm`, `altoCm`, persistida en `guias.dat`. | Vector tridimensional de aristas $\vec{u}, \vec{v}, \vec{w} \in \mathbb{R}^3$. | El algoritmo calcula el volumen mediante el determinante $3 \times 3$ en lugar de asunciones empíricas. |
| **Tarificación** | Función `calcularCostoTotal` invocada al emitir la guía por POST HTTP. | Producto punto $\vec{Carga} \cdot \vec{Tarifa}$ en el espacio vectorial $\mathbb{R}^2$. | Permite escalar a modelos tarifarios multivariables simplemente incrementando la dimensión del vector. |
| **Despacho** | Algoritmo de validación de límites en [include/transacciones.h](file:///Users/josue/Uni/umgExpress/include/transacciones.h#L364-L367). | Inecuación en espacio euclidiano: $\sum \|\vec{u}_i \cdot (\vec{v}_i \times \vec{w}_i)\| \le V_{\text{max}}$. | Evita sobrecupos físicos en el vehículo y garantiza seguridad en ruta. |
| **Rutas y Rastreo** | Consulta GET `/v1/reportes/rastreo/{numeroGuia}`. | Ecuación paramétrica de la recta $\vec{r}(t) = \vec{P}_0 + t \cdot \vec{v}$. | Permite interpolar la ubicación en tiempo real de la encomienda a lo largo del vector director. |

La integración es completamente funcional y no un anexo aislado: el servidor web en producción (`server`) invoca directamente las funciones de [algebra.h](file:///Users/josue/Uni/umgExpress/include/algebra.h) al momento de procesar cada transacción.

---

## 5.11 Uso de Programación

El programa desarrollado en el curso de Algoritmos automatiza de forma estricta los cálculos de Álgebra Lineal:

1. **Datos que recibe el programa:**
   * Dimensiones espaciales del paquete en centímetros (`largoCm`, `anchoCm`, `altoCm`) ingresadas en el cuerpo JSON de la petición HTTP.
   * Masa registrada en báscula (`pesoRealKg`).
   * Parámetros de tarifas activas recuperados desde el archivo binario `tarifas.dat`.
   * Parámetros de capacidad del vehículo asignado (`capacidadVolumenM3`) leídos desde `vehiculos.dat`.
2. **Procedimiento que realiza:**
   * Instancia vectores `Vector3D` y calcula el determinante $3 \times 3$ para obtener el volumen métrico cúbico.
   * Evalúa la función `max` entre peso físico y peso volumétrico ($l \cdot w \cdot h / 5000$).
   * Construye el vector de carga y el vector de tarifas de dimensión 2, ejecutando el método `Algebra::productoPunto`.
   * En el despacho, ejecuta un ciclo de suma vectorial acumulando volúmenes y masas, comparándolos contra las restricciones de capacidad vehicular mediante operadores relacionales.
3. **Conceptos de Álgebra Lineal que utiliza:**
   * Vectores en $\mathbb{R}^2$ y $\mathbb{R}^3$.
   * Producto punto y proyecciones escalares.
   * Triple producto escalar y determinantes de matrices $3 \times 3$.
   * Ecuaciones vectoriales y paramétricas de la recta.
4. **Resultado que obtiene:**
   * Monto exacto en Quetzales del envío persistido en `guias.dat`.
   * Registro del despacho en `despachos.dat` o respuesta de rechazo con código `400 Bad Request` si se vulnera la capacidad volumétrica.
5. **Significado del resultado:**
   * Garantía de viabilidad física del transporte y cobro matemáticamente exacto.

---

## 5.12 Conclusiones y Recomendaciones

### Conclusiones
1. **Relación con el problema planteado:** Se resolvió de manera robusta y automatizada la problemática del sobrecupo volumétrico en vehículos de reparto y la inconsistencia en el cobro de paquetes voluminosos de baja masa.
2. **Cumplimiento de objetivos:** Se demostró la aplicabilidad práctica de los conceptos de Álgebra Lineal dentro de un sistema informático real, conectando la teoría abstracta universitaria con la ingeniería de software aplicada.
3. **Efectividad del Álgebra Lineal:** El modelado mediante determinantes y productos escalares proporcionó un código más limpio, comprensible, elegante y extensible que el uso de fórmulas aritméticas aisladas.
4. **Integración con Algoritmos:** La persistencia binaria (`.dat`) y las transacciones en C++17 se acoplaron sin fricción con los operadores matemáticos lineales desarrollados en [include/algebra.h](file:///Users/josue/Uni/umgExpress/include/algebra.h).
5. **Validación empírica:** Mediante la suite de pruebas unitarias automatizadas (`test_algebra`), se verificó al 100% la exactitud de los resultados numéricos frente a cálculos teóricos manuales.

### Recomendaciones
1. **Extensión a Modelos de Empaque Óptimo (Bin Packing 3D):** Se recomienda a futuros desarrolladores incorporar matrices de rotación ortogonales ($R_x, R_y, R_z$) para evaluar la orientación espacial óptima de cajas dentro del furgón del camión.
2. **Adopción de Sistemas de Ecuaciones Lineales:** Para optimizaciones de flotas de múltiples vehículos con destinos concurrentes, se recomienda implementar el método Simplex o matrices de adyacencia y costos mínimos mediante resolución de sistemas $A\vec{x} = \vec{b}$.
3. **Lectura de Coordenadas Geográficas (GPS):** Se sugiere migrar el catálogo de sucursales para almacenar coordenadas latitud/longitud en formato de vector 3D esférico/cartesiano, permitiendo que la ecuación paramétrica de la recta proyecte trayectorias georreferenciadas en mapas en tiempo real.

---

## 5.13 Otros Elementos de Investigación

### 1. Limitaciones del Proyecto
* **Suposición de cuerpos ortogonales rígidos:** El modelo asume que todos los paquetes son paralelepípedos rectangulares regulares indeformables. No contempla paquetes cilíndricos ni amorfos, en cuyo caso el determinante representaría una cota superior envolvente (bounding box).
* **Trayectoria rectilínea idealizada:** La ecuación paramétrica de la recta asume desplazamientos euclidianos directos entre nodos; en la realidad geográfica, las rutas siguen curvas sobre la orografía terrestre, por lo que la recta modela el vector de desplazamiento resultante y no la longitud total de curvas en carretera.

### 2. Evidencias del Proceso de Investigación y Desarrollo
* **Módulo Matemático Principal:** Archivo [include/algebra.h](file:///Users/josue/Uni/umgExpress/include/algebra.h) con implementaciones de `Vector2D`, `Vector3D`, `productoPunto`, `productoCruz`, `determinante3x3`, `calcularAreaParalelogramo`, `RectaParametrica3D` y `puntoPerteneceARecta`.
* **Integración en Transacciones:** Archivo [include/transacciones.h](file:///Users/josue/Uni/umgExpress/include/transacciones.h#L35-L52) utilizando `Algebra::calcularCostoVariableVectorial` y `Algebra::calcularVolumenPaqueteM3`.
* **Suite de Pruebas Unitarias Automatizadas:** Archivo [tests/test_algebra.cpp](file:///Users/josue/Uni/umgExpress/tests/test_algebra.cpp), compilable y ejecutable mediante:
  ```bash
  clang++ -std=c++17 -Iinclude tests/test_algebra.cpp -o test_algebra && ./test_algebra
  ```
  **Salida de ejecución certificada:**
  ```text
  === Test de Álgebra Lineal y Geometría Analítica (UMG Express) ===
    [1] Probando Vectores y Operaciones de Espacio Vectorial (R2 y R3)...
        -> OK: Cerradura bajo suma, multiplicación por escalar y normas verificadas.
    [2] Probando Producto Punto y Costo Vectorial...
        -> OK: Producto punto y aplicación al costo logístico verificados.
    [3] Probando Producto Cruz y Cálculo de Áreas...
        -> OK: Producto cruz y cálculo de áreas de embalaje verificados.
    [4] Probando Determinantes (3x3) y Cálculo de Volúmenes...
        -> OK: Determinante 3x3 y volumen de paralelepípedo verificados.
    [5] Probando Ecuación Paramétrica y Simétrica de la Recta...
        -> OK: Trayectoria paramétrica r(t) y ecuación simétrica verificadas.
  === Todos los conceptos matemáticos implementados y probados con éxito! ===
  ```
