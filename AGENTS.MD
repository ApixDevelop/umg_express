# UMG Express - Autonomous Agent Instructions

## 1. Objetivo

Desarrollar desde cero el proyecto académico **UMG Express**, un sistema de gestión logística y paquetería.

El proyecto debe proporcionar un backend desarrollado en C++ que exponga una API REST mediante Crow y que posteriormente pueda ser consumida por un frontend en React.

El agente debe trabajar de forma autónoma, tomando las decisiones de implementación necesarias siempre que respete los requisitos establecidos en este documento.

---

# 2. Tecnologías y restricciones

## Lenguaje

* C++17

## Backend

* Crow
* API REST
* JSON

## Persistencia

La información debe almacenarse mediante archivos binarios `.dat`.

Los registros deben utilizar estructuras de tamaño fijo.

Las operaciones de persistencia deben realizarse utilizando mecanismos de archivos de C/C++ como:

* `fopen`
* `fclose`
* `fread`
* `fwrite`
* `fseek`
* `ftell`

## Frontend

El backend debe quedar preparado para ser consumido posteriormente desde React.

---

# 3. Restricciones académicas

Este es un proyecto universitario.

La implementación debe mantener un enfoque de programación estructurada.

No introducir innecesariamente:

* Bases de datos SQL.
* MongoDB.
* ORM.
* Arquitecturas empresariales complejas.
* Frameworks adicionales.
* Dependencias innecesarias.
* Sistemas de persistencia externos.

No reemplazar los archivos `.dat` por una base de datos.

No utilizar una arquitectura excesivamente compleja para un proyecto académico.

Priorizar soluciones simples, comprensibles y justificables.

---

# 4. Sistema de construcción

No utilizar CMake.

No crear `CMakeLists.txt`.

Utilizar el sistema de compilación y ejecución más sencillo que permita trabajar correctamente con C++17 y Crow en el entorno disponible.

Antes de asumir rutas de instalación o dependencias específicas, analizar el entorno disponible.

---

# 5. Desarrollo desde cero

El repositorio inicialmente puede estar vacío excepto por este archivo.

No asumir que existen:

* `main.cpp`
* `include/`
* `data/`
* `docs/`
* `structs.h`
* módulos
* archivos `.dat`

El agente debe crear la estructura necesaria.

---

# 6. Autonomía

El agente debe trabajar de manera autónoma.

No solicitar instrucciones para decisiones normales de implementación.

El agente debe decidir:

* Qué archivos crear.
* Qué carpetas crear.
* Cómo organizar los módulos.
* Qué funciones necesita cada módulo.
* Cómo conectar los módulos.
* Cómo implementar la persistencia.
* Cómo registrar los endpoints.
* Cómo realizar las pruebas.

Las decisiones deben priorizar simplicidad y cumplimiento de los requisitos.

---

# 7. Cuándo detenerse y preguntar

El agente debe detenerse y solicitar confirmación únicamente cuando exista una decisión que pueda:

* Contradecir los requisitos académicos.
* Eliminar información importante.
* Romper intencionalmente datos existentes.
* Cambiar significativamente el alcance del proyecto.
* Requerir una tecnología no contemplada.
* Generar una decisión irreversible o difícil de revertir.

Para decisiones normales de programación, continuar de forma autónoma.

---

# 8. Arquitectura

El agente debe diseñar una estructura de proyecto sencilla y adecuada al tamaño del sistema.

Como mínimo, el proyecto deberá separar razonablemente:

* Definición de estructuras.
* Persistencia.
* Lógica de negocio.
* Endpoints HTTP.
* Reportes.

El agente puede decidir la estructura final de carpetas y archivos.

No está obligado a utilizar exactamente una estructura predeterminada.

Antes de crear una arquitectura compleja, evaluar si realmente es necesaria.

---

# 9. Persistencia binaria

Cada entidad persistente debe almacenarse en un archivo `.dat` correspondiente.

Los registros deben tener tamaño fijo.

La persistencia debe permitir como mínimo:

* Crear.
* Leer.
* Buscar.
* Modificar.
* Baja lógica.

Las bajas deberán realizarse mediante un campo de estado, por ejemplo:

```cpp
bool activo;
```

Los registros inactivos no deben aparecer en consultas normales.

---

# 10. Integridad de los archivos

Antes de modificar una estructura que se almacene directamente en un archivo binario:

1. Analizar su tamaño.
2. Analizar cómo se escribe.
3. Analizar cómo se lee.
4. Analizar cómo se localiza mediante `fseek`.
5. Verificar el impacto del cambio.

Mantener consistencia entre las estructuras y sus archivos `.dat`.

---

# 11. Módulos funcionales

El sistema debe contemplar como mínimo:

### Autenticación

* Usuarios.
* Login.
* Roles.

Roles iniciales:

* Administrador.
* Operador.
* Auditor.

### Catálogos

* Sucursales.
* Clientes.
* Rutas.
* Vehículos.
* Pilotos.
* Tarifas.

### Transacciones

* Creación de guías.
* Cálculo de peso volumétrico.
* Cálculo de costo.
* Asignación de carga.
* Despacho.

### Reportes

* Rastreo.
* Manifiesto por vehículo.
* Liquidación.

---

# 12. API

La API debe utilizar el prefijo:

```text
/v1/
```

Endpoints mínimos esperados:

```text
POST /v1/auth/login
POST /v1/auth/usuarios

GET    /v1/sucursales
POST   /v1/sucursales
PUT    /v1/sucursales/{id}
DELETE /v1/sucursales/{id}

GET    /v1/clientes
POST   /v1/clientes
PUT    /v1/clientes/{id}
DELETE /v1/clientes/{id}

GET    /v1/rutas
POST   /v1/rutas
PUT    /v1/rutas/{id}
DELETE /v1/rutas/{id}

GET    /v1/vehiculos
POST   /v1/vehiculos
PUT    /v1/vehiculos/{id}
DELETE /v1/vehiculos/{id}

GET    /v1/pilotos
POST   /v1/pilotos
PUT    /v1/pilotos/{id}
DELETE /v1/pilotos/{id}

GET    /v1/tarifas
POST   /v1/tarifas

POST /v1/transacciones/guias
POST /v1/transacciones/despacho

GET /v1/reportes/rastreo/{numeroGuia}
GET /v1/reportes/manifiesto/{idVehiculo}
GET /v1/reportes/liquidacion
```

Estos endpoints representan el alcance inicial.

El agente puede proponer ajustes si encuentra una razón técnica válida, pero debe explicarlos antes de cambiar el alcance.

---

# 13. Reglas de negocio

## Peso volumétrico

Utilizar:

```text
peso volumétrico =
(largo × ancho × alto) / 5000
```

El peso facturable será el mayor entre:

* Peso real.
* Peso volumétrico.

---

## Capacidad vehicular

Un despacho debe ser rechazado cuando la carga acumulada supere:

* Capacidad máxima de peso.
* Capacidad máxima de volumen.

Conceptualmente:

```text
peso_actual + nuevo_peso > capacidad_peso
```

o:

```text
volumen_actual + nuevo_volumen > capacidad_volumen
```

---

# 14. Documentación autónoma

El agente debe crear una carpeta:

```text
docs/
```

y generar documentación a medida que el proyecto se desarrolla.

Como mínimo deberá mantener:

```text
docs/
├── arquitectura.md
├── estructura-dat.md
├── api.md
├── reglas-negocio.md
└── estado-proyecto.md
```

La documentación debe reflejar el código real.

No documentar funcionalidades como terminadas si todavía no están implementadas.

---

# 15. Desarrollo incremental

No intentar escribir todo el proyecto sin verificarlo.

Trabajar por módulos.

Después de cada módulo:

1. Compilar.
2. Ejecutar pruebas.
3. Corregir errores.
4. Actualizar documentación.
5. Continuar con el siguiente módulo.

El agente debe decidir el orden óptimo de implementación.

---

# 16. Pruebas

Cada módulo debe verificarse antes de considerarse terminado.

Como mínimo probar:

* Caso exitoso.
* Datos inválidos.
* ID inexistente.
* ID duplicado.
* Registro inactivo.
* Límites de capacidad.
* Errores de archivo.

Los endpoints deben probarse mediante solicitudes HTTP.

---

# 17. Estado del proyecto

Mantener:

```text
docs/estado-proyecto.md
```

Este archivo debe indicar:

* Funcionalidades completadas.
* Funcionalidades en desarrollo.
* Funcionalidades pendientes.
* Errores conocidos.
* Decisiones importantes.

Debe actualizarse después de cada etapa relevante.

---

# 18. Regla de implementación

No escribir código simplemente para cumplir una lista.

Primero comprender el problema.

Después:

```text
Analizar
   ↓
Diseñar
   ↓
Implementar
   ↓
Compilar
   ↓
Probar
   ↓
Corregir
   ↓
Documentar
   ↓
Continuar
```

El objetivo es que el agente pueda desarrollar el proyecto de manera autónoma manteniendo el código funcional en cada etapa.

---

# 19. Regla final

Cuando exista una solución sencilla que cumpla los requisitos, preferirla sobre una solución más compleja.

No sobreingenierizar el proyecto.

El código debe ser comprensible para un estudiante de C++ y defendible académicamente.
