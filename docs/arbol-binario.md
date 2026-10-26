# Matemática Discreta: Árboles Binarios de Búsqueda (BST) y Recursividad

**Módulo:** `include/arbol_bst.h`  
**Pruebas Unitarias:** `tests/test_arbol_bst.cpp` (ejecutable `./test_arbol_bst`)  
**Endpoints REST:** `/v1/arbol/guias/...`

---

## 1. Fundamentos Teóricos de Matemática Discreta

### 1.1 Definición Formal de Árbol Binario
En teoría de grafos y matemática discreta, un **árbol** es un grafo simple, no dirigido, conexo y acíclico:
$$G = (V, E) \quad \text{con } |E| = |V| - 1$$

Un **árbol binario con raíz** es un grafo dirigido donde:
1. Existe un único vértice especial llamado **raíz** con grado de entrada 0.
2. Todo vértice distinto de la raíz tiene grado de entrada exactamente 1.
3. Cada vértice tiene a lo sumo dos hijos (grado de salida $\le 2$), distinguidos formalmente como **hijo izquierdo** e **hijo derecho**.

### 1.2 Propiedad de Orden del Árbol Binario de Búsqueda (BST)
Sea $x$ un nodo cualquiera de un árbol binario $T$:
* Si $y$ es un nodo en el subárbol izquierdo de $x$, entonces:
  $$\text{clave}(y) < \text{clave}(x)$$
* Si $z$ es un nodo en el subárbol derecho de $x$, entonces:
  $$\text{clave}(z) > \text{clave}(x)$$

### 1.3 Teorema del Recorrido In-Order (Inorden)
> **Teorema:** Si $T$ es un árbol binario de búsqueda, el recorrido *In-Order* (Izquierda $\to$ Raíz $\to$ Derecha) visita los vértices de $T$ en orden estrictamente ascendente de sus claves.

*Demostración inductiva:*
- **Caso base:** Si el árbol está vacío o consta de un solo nodo, el recorrido visita trivialmente el único elemento en orden.
- **Paso inductivo:** Supóngase que el recorrido in-order ordena correctamente cualquier árbol de tamaño $< n$. Por la propiedad de BST, todos los nodos del subárbol izquierdo son menores que la raíz, y todos los del subárbol derecho son mayores. El recorrido visita primero el subárbol izquierdo (ordenado por hipótesis inductiva), luego la raíz, y finalmente el subárbol derecho (ordenado por hipótesis inductiva). Por tanto, la secuencia global resultante está estrictamente ordenada. $\blacksquare$

---

## 2. Aplicación Práctica en UMG Express

En UMG Express, la persistencia se realiza en archivos binarios planos `.dat`. La búsqueda secuencial directa en disco tiene una complejidad temporal lineal:
$$O(n)$$

Al indexar las guías en memoria mediante un **Árbol Binario de Búsqueda** utilizando como clave `numeroGuia`:
1. **Búsqueda Logarítmica:** La búsqueda binaria recursiva reduce el número de comparaciones a:
   $$O(\log_2 n)$$
2. **Generación de Reportes Ordenados:** El recorrido In-Order recursivo genera el catálogo de guías ordenado ascendentemente en tiempo lineal $O(n)$ sin necesidad de algoritmos de ordenamiento auxiliares como QuickSort ($O(n \log n)$).

---

## 3. Algoritmos Recursivos Implementados

### 3.1 Inserción Recursiva (`insertarRecursivo`)
- **Caso Base:** Si el nodo es `nullptr`, crea un nuevo nodo con la clave y dato.
- **Paso Recursivo:**
  - Si $\text{clave} < \text{raiz}\to\text{clave}$: desciende recursivamente al hijo izquierdo.
  - Si $\text{clave} > \text{raiz}\to\text{clave}$: desciende recursivamente al hijo derecho.
  - Si $\text{clave} == \text{raiz}\to\text{clave}$: rechaza la inserción por unicidad de clave primaria.

### 3.2 Búsqueda Binaria Recursiva (`buscarRecursivo`)
- **Caso Base 1:** Si el nodo es `nullptr`, la clave no existe en el árbol (retorna `false`).
- **Caso Base 2:** Si $\text{raiz}\to\text{clave} == \text{clave}$, elemento encontrado (retorna `true`).
- **Paso Recursivo:** Desciende a la izquierda si la clave buscada es menor, o a la derecha si es mayor, incrementando el contador de comparaciones.

### 3.3 Cálculo Recursivo de la Altura (`calcularAlturaRecursivo`)
$$\text{Altura}(T) = \begin{cases} 0 & \text{si } T = \emptyset \\ 1 + \max(\text{Altura}(T_{izq}), \text{Altura}(T_{der})) & \text{en otro caso} \end{cases}$$

### 3.4 Conteo Recursivo de Nodos (`contarNodosRecursivo`)
$$|V| = \begin{cases} 0 & \text{si } T = \emptyset \\ 1 + |V_{izq}| + |V_{der}| & \text{en otro caso} \end{cases}$$

---

## 4. Endpoints de la API REST

| Método | Endpoint | Descripción | Complejidad |
| :--- | :--- | :--- | :--- |
| `GET` | `/v1/arbol/guias/metricas` | Total de nodos, altura y secuencias de claves en In-Order, Pre-Order y Post-Order. | $O(n)$ |
| `GET` | `/v1/arbol/guias/inorder` | Listado completo de guías ordenadas de menor a mayor mediante recorrido In-Order. | $O(n)$ |
| `GET` | `/v1/arbol/guias/buscar/{numeroGuia}` | Búsqueda binaria recursiva que reporta las comparaciones realizadas. | $O(\log n)$ |
| `GET` | `/v1/arbol/guias/estructura` | Estructura jerárquica del árbol en formato JSON anidado (apta para visualización en frontend). | $O(n)$ |

---

## 5. Pruebas y Validación

La suite unitaria [`tests/test_arbol_bst.cpp`](file:///Users/josue/Uni/umgExpress/tests/test_arbol_bst.cpp) valida formalmente:
1. Inserción correcta y rechazo de claves duplicadas.
2. Verificación de comparaciones en búsqueda (ej: 1 comparación en raíz, 3 en hojas).
3. Prueba del Teorema In-Order: comprueba que `[50, 25, 75, 10, 30]` produce exactamente `[10, 25, 30, 50, 75]`.
4. Serialización JSON de la jerarquía.
5. Indexación de estructuras reales `Guia`.
