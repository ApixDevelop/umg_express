# Estructuras y Archivos Binarios (.dat) - UMG Express

## 1. Principios de Persistencia Binaria

El sistema UMG Express implementa persistencia de datos mediante archivos binarios de extensión `.dat` en el directorio `data/`.

Reglas fundamentales:
- **Estructuras de tamaño fijo**: No se utilizan punteros dinámicos ni `std::string` dentro de las estructuras guardadas directamente en disco. Se emplean cadenas estilo C (`char[]`) de tamaño constante.
- **Acceso directo en disco**: Las operaciones de actualización y eliminación se realizan mediante posicionamiento de puntero de archivo con `fseek(f, index * sizeof(Struct), SEEK_SET)` y sobreescritura con `fwrite()`.
- **Funciones C/C++ permitidas**:
  ```cpp
  fopen()
  fclose()
  fread()
  fwrite()
  fseek()
  ftell()
  ```
- **Baja lógica**: Los registros no se borran físicamente del archivo `.dat`. Se utiliza un campo `bool activo`. Cuando un registro se elimina, se cambia `activo = false` y las consultas por defecto filtran únicamente registros activos (`activo == true`).

---

## 2. Definición de Estructuras (header `include/structs.h`)

### 2.1 `Usuario`
- **Archivo**: `data/usuarios.dat`
- **Tamaño**: 188 bytes
- **Campos**:
  ```cpp
  struct Usuario {
      int id;               // ID único (4 bytes)
      char username[32];    // Nombre de usuario (32 bytes)
      char password[64];    // Contraseña hash/plana (64 bytes)
      char nombre[64];      // Nombre completo (64 bytes)
      char rol[20];         // "Administrador", "Operador", "Auditor" (20 bytes)
      bool activo;          // Estado activo/inactivo (1 byte + padding)
  };
  ```

### 2.2 `Sucursal`
- **Archivo**: `data/sucursales.dat`
- **Tamaño**: 220 bytes
- **Campos**:
  ```cpp
  struct Sucursal {
      int id;               // ID único (4 bytes)
      char nombre[64];      // Nombre de la sucursal (64 bytes)
      char direccion[128];  // Dirección física (128 bytes)
      char telefono[20];    // Teléfono de contacto (20 bytes)
      bool activo;          // Estado activo/inactivo (1 byte + padding)
  };
  ```

### 2.3 `Cliente`
- **Archivo**: `data/clientes.dat`
- **Tamaño**: 240 bytes
- **Campos**:
  ```cpp
  struct Cliente {
      int id;               // ID único (4 bytes)
      char nit[20];         // Identificación tributaria (20 bytes)
      char nombre[64];      // Nombre o Razón Social (64 bytes)
      char direccion[128];  // Dirección del cliente (128 bytes)
      char telefono[20];    // Teléfono (20 bytes)
      bool activo;          // Estado activo/inactivo (1 byte + padding)
  };
  ```

### 2.4 `Ruta`
- **Archivo**: `data/rutas.dat`
- **Tamaño**: 40 bytes
- **Campos**:
  ```cpp
  struct Ruta {
      int id;                  // ID de ruta (4 bytes)
      int idSucursalOrigen;    // ID sucursal de origen (4 bytes)
      int idSucursalDestino;   // ID sucursal de destino (4 bytes)
      double distanciaKm;      // Distancia en kilómetros (8 bytes)
      double tarifaBase;       // Tarifa base del trayecto (8 bytes)
      bool activo;             // Estado activo/inactivo (1 byte + padding)
  };
  ```

### 2.5 `Vehiculo`
- **Archivo**: `data/vehiculos.dat`
- **Tamaño**: 112 bytes
- **Campos**:
  ```cpp
  struct Vehiculo {
      int id;                  // ID de vehículo (4 bytes)
      char placa[15];          // Número de placa (15 bytes)
      char marcaModelo[50];    // Marca y modelo (50 bytes)
      double capacidadPesoKg;  // Peso máximo soportado en kg (8 bytes)
      double capacidadVolumenM3;// Volumen máximo soportado en m3 (8 bytes)
      char estado[20];         // "Disponible", "En ruta", "En taller" (20 bytes)
      bool activo;             // Estado activo/inactivo (1 byte + padding)
  };
  ```

### 2.6 `Piloto`
- **Archivo**: `data/pilotos.dat`
- **Tamaño**: 112 bytes
- **Campos**:
  ```cpp
  struct Piloto {
      int id;               // ID de piloto (4 bytes)
      char nombre[64];      // Nombre completo (64 bytes)
      char licencia[20];    // Número/tipo de licencia (20 bytes)
      char telefono[20];    // Teléfono (20 bytes)
      bool activo;          // Estado activo/inactivo (1 byte + padding)
  };
  ```

### 2.7 `Tarifa`
- **Archivo**: `data/tarifas.dat`
- **Tamaño**: 72 bytes
- **Campos**:
  ```cpp
  struct Tarifa {
      int id;                  // ID de tarifa (4 bytes)
      char tipoCarga[32];      // Tipo o categoría de carga (32 bytes)
      double precioPorKg;      // Precio cobrado por kg (8 bytes)
      double precioPorM3;      // Precio cobrado por m3 (8 bytes)
      double recargoEspecial;  // Recargo por tipo de servicio (8 bytes)
      bool activo;             // Estado activo/inactivo (1 byte + padding)
  };
  ```

### 2.8 `Guia`
- **Archivo**: `data/guias.dat`
- **Tamaño**: 96 bytes
- **Campos**:
  ```cpp
  struct Guia {
      int numeroGuia;           // Número único de guía (4 bytes)
      int idClienteRemitente;   // ID del remitente (4 bytes)
      int idClienteDestinatario;// ID del destinatario (4 bytes)
      int idRuta;               // ID de la ruta asignada (4 bytes)
      double pesoRealKg;        // Peso medido en báscula (8 bytes)
      double largoCm;           // Largo del paquete en cm (8 bytes)
      double anchoCm;           // Ancho del paquete en cm (8 bytes)
      double altoCm;            // Alto del paquete en cm (8 bytes)
      double pesoVolumetricoKg; // (Largo x Ancho x Alto) / 5000 (8 bytes)
      double pesoFacturableKg;  // max(pesoReal, pesoVolumetrico) (8 bytes)
      double costoTotal;        // Costo calculado (8 bytes)
      char estado[20];          // "Creada", "Despachada", "Entregada" (20 bytes)
      bool activo;              // Estado activo/inactivo (1 byte + padding)
  };
  ```

---

## 3. Motor de Persistencia (`include/persistence.h`)

Funciones principales disponibles en la capa de almacenamiento:

| Función | Descripción |
| :--- | :--- |
| `asegurarArchivo(path)` | Verifica y crea el archivo `.dat` en modo `a+b` si no existe. |
| `leerTodos<T>(path, soloActivos)` | Retorna un `std::vector<T>` leyendo el archivo de inicio a fin. |
| `insertar<T>(path, record)` | Escribe un nuevo registro al final del archivo (`a+b`). |
| `buscarPorId<T>(path, id, outRecord, outIndex)` | Recorre el archivo buscando el ID activo y retorna su registro e índice físico `0-based`. |
| `actualizarEnIndice<T>(path, index, record)` | Usa `fseek(f, index * sizeof(T), SEEK_SET)` y sobreescribe con `fwrite`. |
| `actualizarPorId<T>(path, id, nuevoRecord)` | Localiza el índice por ID y lo actualiza. |
| `eliminarLogico<T>(path, id)` | Marca `activo = false` y sobreescribe el registro con `fseek`. |
| `obtenerSiguienteId<T>(path)` | Escanea los registros y retorna `max(id) + 1` para asignación automática. |

---

## 4. Estrategia de Asignación y Validación de IDs

1. **Auto-incremento**: Al insertar una nueva entidad, se llama a `obtenerSiguienteId<T>()` para asignar un identificador secuencial correlativo.
2. **Prevención de Duplicados**: Se valida con `existeId<T>()` antes de insertar IDs personalizados.
3. **Baja Lógica**: El ID de un registro inactivo permanece en el archivo binario para garantizar integridad histórica, pero no se retorna en búsquedas ni en listas activas.
