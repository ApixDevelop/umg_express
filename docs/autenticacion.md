# Módulo de Autenticación y Usuarios - UMG Express

## 1. Descripción General

El módulo de Autenticación y Usuarios gestiona las credenciales, roles y acceso de los usuarios del sistema UMG Express.
La información se almacena en el archivo binario `data/usuarios.dat` utilizando registros de tamaño constante (`struct Usuario`).

---

## 2. Estructura de Datos (`include/structs.h`)

```cpp
struct Usuario {
    int id;               // ID secuencial de usuario (4 bytes)
    char username[32];    // Nombre de usuario único (32 bytes)
    char password[64];    // Contraseña (64 bytes)
    char nombre[64];      // Nombre completo del usuario (64 bytes)
    char rol[20];         // Rol asignado ("Administrador", "Operador", "Auditor") (20 bytes)
    bool activo;          // Estado activo/inactivo para baja lógica (1 byte + padding)
};
```

---

## 3. Roles Soportados

El sistema implementa tres roles definidos por las reglas de negocio:

1. **`Administrador`**: Control total sobre catálogos, usuarios y operaciones.
2. **`Operador`**: Emisión de guías, asignación de despachos y gestión de envíos.
3. **`Auditor`**: Consulta de reportes y supervisión de manifiestos/liquidaciones.

---

## 4. Endpoints HTTP

### 4.1 POST `/v1/auth/login`

Valida las credenciales de un usuario y comprueba su estado activo.

- **Solicitud**:
  - `Content-Type: application/json`
  - Body:
    ```json
    {
      "username": "admin",
      "password": "admin123"
    }
    ```

- **Respuesta Exitosa (`200 OK`)**:
  ```json
  {
    "success": true,
    "mensaje": "Autenticación exitosa",
    "usuario": {
      "id": 1,
      "username": "admin",
      "nombre": "Administrador Inicial",
      "rol": "Administrador"
    }
  }
  ```

- **Respuesta de Error - Credenciales / Inactivo (`401 Unauthorized`)**:
  ```json
  {
    "success": false,
    "mensaje": "Credenciales inválidas (contraseña incorrecta)"
  }
  ```

---

### 4.2 POST `/v1/auth/usuarios`

Registra un nuevo usuario en el archivo binario `data/usuarios.dat`.

- **Solicitud**:
  - `Content-Type: application/json`
  - Body:
    ```json
    {
      "username": "operador1",
      "password": "op123password",
      "nombre": "Carlos Gómez",
      "rol": "Operador"
    }
    ```

- **Respuesta Exitosa (`201 Created`)**:
  ```json
  {
    "success": true,
    "mensaje": "Usuario creado exitosamente",
    "usuario": {
      "id": 2,
      "username": "operador1",
      "nombre": "Carlos Gómez",
      "rol": "Operador"
    }
  }
  ```

- **Respuesta de Error - Username Duplicado (`409 Conflict`)**:
  ```json
  {
    "success": false,
    "mensaje": "El nombre de usuario 'operador1' ya existe en el sistema"
  }
  ```

- **Respuesta de Error - Datos o Rol Inválido (`400 Bad Request`)**:
  ```json
  {
    "success": false,
    "mensaje": "Rol no válido. Debe ser: Administrador, Operador o Auditor"
  }
  ```

---

## 5. Usuario Administrador por Defecto

Al iniciar el servidor por primera vez (`main.cpp`), si `data/usuarios.dat` no contiene usuarios activos, se crea automáticamente un usuario administrador inicial:

- **Username**: `admin`
- **Password**: `admin123`
- **Nombre**: `Administrador Inicial`
- **Rol**: `Administrador`
