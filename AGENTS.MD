# UMG Express - Agent Instructions

## 1. Descripción del proyecto

UMG Express es un sistema de gestión logística y paquetería desarrollado en C++17 para un proyecto académico de la Universidad Mariano Gálvez.

El backend utiliza Crow para exponer una API REST que será consumida posteriormente por una interfaz desarrollada en React.

La persistencia de información se realiza mediante archivos binarios `.dat` con registros de tamaño fijo.

---

## 2. Tecnologías

* C++17
* Crow
* CMake
* API REST
* JSON
* Archivos binarios `.dat`
* React para el frontend

---

## 3. Restricciones académicas

El proyecto debe mantener un enfoque de programación estructurada.

### Persistencia

La información debe almacenarse mediante archivos binarios utilizando principalmente:

* `fopen`
* `fclose`
* `fread`
* `fwrite`
* `fseek`
* `ftell`

No reemplazar los archivos `.dat` por:

* MySQL
* PostgreSQL
* SQLite
* MongoDB
* ORM
* Bases de datos externas

### Estructuras

Los registros persistidos deben utilizar `struct` con campos de tamaño fijo.

Evitar `std::string` dentro de estructuras que se escriban directamente en archivos binarios.

---

## 4. Arquitectura actual

Mantener la estructura actual del proyecto.

```text
umg_express/
├── crow.h
├── main.cpp
├── CMakeLists.txt
├── include/
│   ├── structs.h
│   ├── auth.h
│   ├── sucursales.h
│   ├── clientes.h
│   ├── rutas.h
│   ├── vehiculos.h
│   ├── pilotos.h
│   ├── tarifas.h
│   ├── transacciones.h
│   └── reportes.h
└── data/
    ├── usuarios.dat
    ├── sucursales.dat
    ├── clientes.dat
    ├── rutas.dat
    ├── vehiculos.dat
    ├── pilotos.dat
    ├── tarifas.dat
    └── guias.dat
```

No crear nuevas capas arquitectónicas como `controllers`, `services` o `repositories` a menos que se solicite explícitamente.

---

## 5. Responsabilidad de los archivos

### `main.cpp`

Responsable de:

* Inicializar Crow.
* Configurar la aplicación.
* Registrar endpoints.
* Iniciar el servidor.

No colocar lógica compleja de persistencia directamente en `main.cpp`.

### `include/structs.h`

Contiene las estructuras utilizadas por el sistema.

Las estructuras persistidas deben mantenerse compatibles con los archivos `.dat`.

### `include/auth.h`

Contiene:

* Validación de usuarios.
* Autenticación.
* Validación de credenciales.
* Manejo de roles.

### `include/sucursales.h`

Contiene las operaciones relacionadas con `sucursales.dat`.

### `include/clientes.h`

Contiene las operaciones relacionadas con `clientes.dat`.

### `include/rutas.h`

Contiene las operaciones relacionadas con `rutas.dat`.

### `include/vehiculos.h`

Contiene las operaciones relacionadas con `vehiculos.dat`.

### `include/pilotos.h`

Contiene las operaciones relacionadas con `pilotos.dat`.

### `include/tarifas.h`

Contiene las operaciones relacionadas con `tarifas.dat`.

### `include/transacciones.h`

Contiene:

* Creación de guías.
* Cálculo de peso volumétrico.
* Cálculo de costos.
* Asignación de carga.
* Validación de capacidad.
* Despacho.

### `include/reportes.h`

Contiene:

* Rastreo.
* Manifiestos.
* Liquidaciones.
* Consultas consolidadas.

---

## 6. Reglas para el código

### General

* Mantener C++17.
* Mantener el estilo de código existente.
* No introducir complejidad innecesaria.
* No duplicar funciones.
* Reutilizar funciones existentes.
* Evitar variables globales cuando no sean necesarias.
* Utilizar nombres descriptivos.
* Comentar únicamente la lógica que necesite explicación.

### Archivos binarios

Antes de modificar una estructura persistida:

1. Revisar su definición actual.
2. Revisar qué archivo `.dat` utiliza.
3. Revisar cómo se calcula la posición del registro.
4. Revisar las operaciones `fread`, `fwrite` y `fseek`.
5. Verificar que el cambio no rompa la compatibilidad del archivo.

No cambiar una estructura persistida sin analizar el impacto sobre el archivo `.dat`.

---

## 7. Baja lógica

Cuando una entidad requiera eliminación, utilizar baja lógica mediante el campo:

```cpp
bool activo;
```

No eliminar físicamente registros salvo que se solicite explícitamente.

---

## 8. API REST

Todos los endpoints deben utilizar el prefijo:

```text
/v1/
```

La especificación completa se encuentra en:

```text
docs/rutas-api.md
```

No inventar endpoints que no estén definidos en la especificación sin consultar primero.

---

## 9. Reglas de desarrollo

Antes de implementar una funcionalidad:

1. Revisar `docs/arquitectura.md`.
2. Revisar `docs/estructura-dat.md` si afecta persistencia.
3. Revisar `docs/reglas-negocio.md` si afecta lógica del sistema.
4. Revisar `docs/rutas-api.md` si afecta la API.
5. Revisar el código existente.
6. Reutilizar funciones existentes cuando sea posible.

---

## 10. Cambios

No reescribir archivos completos innecesariamente.

Realizar cambios pequeños y controlados.

No modificar funcionalidades que no estén relacionadas con la tarea solicitada.

Si una modificación puede afectar otros módulos, indicarlo antes de realizar cambios grandes.

---

## 11. Verificación

Después de implementar una funcionalidad:

* Compilar el proyecto.
* Corregir errores de compilación.
* Verificar que los endpoints respondan correctamente.
* Verificar que los archivos `.dat` continúen funcionando.
* Probar casos exitosos.
* Probar entradas inválidas.
* Probar casos límite.

---

## 12. Comunicación

Cuando se solicite una modificación, proporcionar:

1. Qué se va a modificar.
2. Qué archivos serán afectados.
3. Cómo se implementará.
4. Qué riesgos existen.

Después de implementar:

1. Resumen de cambios.
2. Archivos modificados.
3. Resultado de la compilación.
4. Pruebas realizadas.
5. Problemas pendientes, si existen.

---

## 13. Regla principal

No asumir que una solución técnicamente más moderna es mejor para este proyecto.

La prioridad es:

1. Requisitos académicos.
2. Compatibilidad con la arquitectura existente.
3. Correctitud.
4. Simplicidad.
5. Mantenibilidad.
