# Estructuras y Archivos Binarios

## 1. Reglas generales

Los datos persistentes se almacenan en archivos binarios `.dat`.

Cada archivo contiene registros de tamaño fijo.

Las operaciones principales son:

```cpp
fopen()
fclose()
fread()
fwrite()
fseek()
ftell()
```

Las estructuras persistidas deben mantener un tamaño fijo.

No utilizar `std::string` dentro de estructuras que se escriben directamente mediante `fwrite()`.

---

# 2. Usuarios

Archivo:

```text
data/usuarios.dat
```

Estructura:

```cpp
// Completar con la estructura real de structs.h
struct Usuario {
    // ...
};
```

Uso:

* Autenticación.
* Roles.
* Estado activo/inactivo.

---

# 3. Sucursales

Archivo:

```text
data/sucursales.dat
```

Estructura:

```cpp
struct Sucursal {
    // Completar con la estructura real de structs.h
};
```

Uso:

* Catálogo de sucursales.
* Origen y destino de rutas.

---

# 4. Clientes

Archivo:

```text
data/clientes.dat
```

Estructura:

```cpp
struct Cliente {
    // Completar con la estructura real de structs.h
};
```

Uso:

* Remitentes.
* Destinatarios.

---

# 5. Rutas

Archivo:

```text
data/rutas.dat
```

Estructura:

```cpp
struct Ruta {
    // Completar con la estructura real de structs.h
};
```

Uso:

* Conexiones entre sucursales.
* Distancias.
* Tarifas base.

---

# 6. Vehículos

Archivo:

```text
data/vehiculos.dat
```

Estructura:

```cpp
struct Vehiculo {
    // Completar con la estructura real de structs.h
};
```

Uso:

* Control de flota.
* Capacidad de peso.
* Capacidad de volumen.
* Estado operativo.

---

# 7. Pilotos

Archivo:

```text
data/pilotos.dat
```

Estructura:

```cpp
struct Piloto {
    // Completar con la estructura real de structs.h
};
```

Uso:

* Información del piloto.
* Licencia.
* Estado operativo.

---

# 8. Tarifas

Archivo:

```text
data/tarifas.dat
```

Estructura:

```cpp
struct Tarifa {
    // Completar con la estructura real de structs.h
};
```

Uso:

* Clasificación de carga.
* Tarifas.
* Recargos.

---

# 9. Guías

Archivo:

```text
data/guias.dat
```

Estructura:

```cpp
struct Guia {
    // Completar con la estructura real de structs.h
};
```

Uso:

* Información de paquetes.
* Peso.
* Dimensiones.
* Costos.
* Rastreo.
* Despacho.

---

# 10. Regla para modificar estructuras

Antes de modificar una estructura persistida:

1. Revisar todos los lugares donde se utiliza.
2. Revisar el archivo `.dat` asociado.
3. Revisar `sizeof(struct)`.
4. Revisar las operaciones `fread()`.
5. Revisar las operaciones `fwrite()`.
6. Revisar los cálculos realizados mediante `fseek()`.
7. Compilar y probar nuevamente.

No modificar estructuras persistidas de manera arbitraria.
