# Módulo de Autenticación, Seguridad y Autorización — UMG Express

## 1. Descripción General

El módulo de Autenticación y Autorización gestiona la identidad, credenciales, sesiones y permisos de acceso para los usuarios del sistema UMG Express.
La información de los usuarios se persiste en el archivo binario `data/usuarios.dat` utilizando registros de tamaño constante (`struct Usuario`).
Las sesiones activas se administran de forma segura en memoria mediante `Auth::SessionManager`, emitiendo tokens Bearer de alta entropía.

---

## 2. Estructura de Datos (`include/structs.h`)

```cpp
struct Usuario {
    int id;               // ID secuencial de usuario (4 bytes)
    char username[32];    // Nombre de usuario único (32 bytes)
    char password[64];    // Hash con sal o contraseña protegida (64 bytes)
    char nombre[64];      // Nombre completo del usuario (64 bytes)
    char rol[20];         // Rol asignado ("Administrador", "Operador", "Auditor") (20 bytes)
    bool activo;          // Estado activo/inactivo para baja lógica (1 byte + padding)
};
```

---

## 3. Seguridad de Contraseñas y Tokens

### 3.1 Generación de Tokens (CSPRNG)
- Se utiliza el generador criptográficamente seguro del sistema operativo (`arc4random_buf` en macOS/BSD y `/dev/urandom` en Linux).
- Cada token consta de 32 bytes de entropía (64 caracteres hexadecimales).
- No se utilizan marcas de tiempo, identificadores secuenciales ni funciones pseudoaleatorias débiles.

### 3.2 Almacenamiento de Contraseñas
- Nuevos registros: Almacenan un hash derivado con 1,000 rondas SHA-256 y sal única por usuario (`$pbk$<hex>`).
- Longitud del hash: 60 caracteres, encajando en `char password[64]` sin modificar el esquema binario ni el tamaño del archivo `.dat`.
- Compatibilidad: Admite verificación de usuarios existentes en texto plano sin alterar ni sobrescribir automáticamente los archivos `.dat` de producción.

### 3.3 Ciclo de Vida de las Sesiones
- **Vigencia (TTL):** 86,400 segundos (24 horas).
- **Revocación:** El endpoint `POST /v1/auth/logout` invalida de inmediato la sesión.
- **Validación continua:** Cada petición protegida comprueba tanto la sesión en memoria como el estado activo del usuario en `data/usuarios.dat`.

---

## 4. Roles y Matriz de Permisos (RBAC)

1. **`Administrador`**: Control total. Gestión de usuarios (`/v1/auth/usuarios`), creación/modificación/eliminación de catálogos, emisión de guías, despachos y reportes.
2. **`Operador`**: Emisión y consulta de guías, asignación de despachos, creación de clientes y consulta de catálogos y manifiestos.
3. **`Auditor`**: Supervisión y auditoría de sólo lectura en catálogos, guías, despachos y acceso completo a reportes (incluyendo liquidaciones financieras).

---

## 5. Endpoints HTTP del Módulo

### 5.1 POST `/v1/auth/login` (Público)
Valida credenciales y emite token Bearer.
- **Request Body:**
  ```json
  {
    "username": "admin",
    "password": "admin123"
  }
  ```
- **Response `200 OK`:**
  ```json
  {
    "success": true,
    "mensaje": "Autenticación exitosa",
    "access_token": "48b2506b56f15393a270ef0a91f537da48b2506b56f15393a270ef0a91f537da",
    "token_type": "Bearer",
    "expires_in": 86400,
    "user": {
      "id": 1,
      "username": "admin",
      "nombre": "Administrador Principal",
      "rol": "Administrador"
    }
  }
  ```
- **Response `401 Unauthorized`:**
  ```json
  {
    "success": false,
    "mensaje": "Credenciales inválidas"
  }
  ```

### 5.2 POST `/v1/auth/logout` (Protegido)
Invalida el token de acceso actual.
- **Headers:** `Authorization: Bearer <token>`
- **Response `200 OK`:**
  ```json
  {
    "success": true,
    "mensaje": "Sesión cerrada exitosamente"
  }
  ```

### 5.3 GET `/v1/auth/me` (Protegido)
Consulta el perfil público del usuario en sesión sin exponer contraseñas ni hashes.
- **Headers:** `Authorization: Bearer <token>`
- **Response `200 OK`:**
  ```json
  {
    "success": true,
    "user": {
      "id": 1,
      "username": "admin",
      "nombre": "Administrador Principal",
      "rol": "Administrador"
    }
  }
  ```

### 5.4 POST `/v1/auth/usuarios` (Protegido - Exclusivo Administrador)
Registra un nuevo usuario en `data/usuarios.dat`.
- **Headers:** `Authorization: Bearer <token_admin>`
- **Request Body:**
  ```json
  {
    "username": "operador1",
    "password": "op123password",
    "nombre": "Carlos Gómez",
    "rol": "Operador"
  }
  ```
- **Response `201 Created`:**
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
- **Response `403 Forbidden` (Si el rol no es Administrador):**
  ```json
  {
    "success": false,
    "mensaje": "Acceso denegado: el rol 'Operador' no cuenta con permisos suficientes para esta operación"
  }
  ```
