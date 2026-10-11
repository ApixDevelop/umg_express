# Auditoría Técnica y Especificación de Seguridad Backend — UMG Express

**Proyecto:** UMG Express — Sistema de Gestión Logística y Paquetería  
**Fase de Implementación:** Fase 1 — Autenticación y Autorización mediante Tokens  
**Entorno:** C++17 / Framework HTTP Crow / Persistencia Binaria `.dat`  
**Fecha:** Octubre 2026  

---

## 1. Resumen Ejecutivo y Diagnóstico Inicial

En la revisión técnica previa del backend de UMG Express se identificaron las siguientes vulnerabilidades y carencias críticas de seguridad:

1. **Ausencia de Tokens de Sesión en el Login:**
   El endpoint `POST /v1/auth/login` únicamente validaba credenciales y devolvía los datos del usuario, sin generar un token de acceso (`access_token`, `token_type`, `expires_in`). El cliente no tenía mecanismo para identificarse en solicitudes posteriores.
2. **Endpoints de Negocio Desprotegidos:**
   Todos los catálogos (sucursales, clientes, rutas, vehículos, pilotos, tarifas), transacciones (emisión de guías, despacho físico) y reportes (rastreo, manifiesto y liquidación financiera) podían ser consumidos por cualquier cliente anónimo sin enviar credenciales ni tokens de autorización.
3. **Exposición del Registro de Usuarios:**
   El endpoint `POST /v1/auth/usuarios` era completamente público, permitiendo a cualquier actor anónimo registrar cuentas con el rol de `Administrador`.
4. **Almacenamiento de Contraseñas en Texto Plano:**
   Las contraseñas se almacenaban tal cual en la estructura binaria `Usuario` (`char password[64]`).
5. **Inexistencia de Cierre de Sesión (Logout):**
   No existía ningún mecanismo para revocar o cerrar sesiones.

---

## 2. Decisiones de Arquitectura e Implementación

### 2.1 Generador Criptográficamente Seguro de Tokens (CSPRNG)
- **Implementación:** Se utiliza la entropía provista directamente por el sistema operativo mediante `arc4random_buf` en macOS/BSD y `/dev/urandom` en Linux.
- **Entropía:** 32 bytes (256 bits) de aleatoriedad criptográfica, representados como una cadena de 64 caracteres hexadecimales.
- **Regla estricta:** No se utilizan números secuenciales, marcas de tiempo ni la función pseudoaleatoria débil `rand()`.

### 2.2 Gestor de Sesiones en Memoria (`Auth::SessionManager`)
- **Almacenamiento:** Tabla hash protegida por exclusión mutua (`std::mutex`) para garantizar seguridad en entornos multihilo (`crow::multithreaded()`).
- **Tiempo de Vida (TTL):** 86,400 segundos (24 horas). Cada sesión almacena su marca de expiración.
- **Verificación de Usuario en Persistencia:** Al validar un token, se verifica no solo la existencia y vigencia de la sesión en memoria, sino también que el usuario siga existiendo y se encuentre activo (`activo == true`) en el archivo binario `data/usuarios.dat`. Si un usuario sufre una baja lógica, su token es invalidado de inmediato.
- **Cierre de Sesión:** `POST /v1/auth/logout` marca la sesión como inactiva (`activa = false`), impidiendo su reutilización.
- **Persistencia de Sesiones:** Las sesiones residen en memoria, lo que significa que al reiniciar el servidor las sesiones activas caducan de manera limpia y segura, obligando a los clientes a reautenticarse. Esto respeta la arquitectura académica sin introducir bases de datos externas como Redis.

### 2.3 Manejo de Contraseñas y Compatibilidad Retroactiva
- **Algoritmo:** Función de derivación con estiramiento de clave (1,000 iteraciones SHA-256) y sal única basada en el nombre de usuario (`UMG_EXPRESS_SALT_<username>_SECURE_V1`).
- **Formato:** `$pbk$` seguido de 55 caracteres hexadecimales (longitud total: 60 caracteres). Encaja de forma exacta dentro del campo binario `char password[64]` de la estructura `Usuario`, sin alterar el tamaño de 188 bytes del registro ni romper la compatibilidad binaria.
- **Compatibilidad con Datos Existentes:** `Auth::verificarPassword` admite tanto contraseñas con formato hash `$pbk$` como contraseñas heredadas en texto plano preexistentes en `data/usuarios.dat` (`admin123`, `op123password`).
- **Estrategia Segura de Transición:** Conforme a las instrucciones del proyecto, **no se modifican ni sobrescriben los archivos `.dat` reales automáticamente** durante las pruebas ni en el inicio del servidor, evitando corrupción o efectos irreversibles. Los nuevos registros creados a través de la API se almacenan con el hash seguro.

### 2.4 Proxy y CORS
- El frontend se comunicará con el backend mediante un proxy de desarrollo/producción. Por lo tanto, **no se implementó CORS en esta fase** en el backend para evitar configuraciones permisivas o sobreingeniería innecesaria. Toda la autenticación se basa estrictamente en el encabezado `Authorization: Bearer <token>`.

---

## 3. Matriz de Autorización Basada en Roles (RBAC)

El sistema define 3 roles institucionales: `Administrador`, `Operador` y `Auditor`.

| Endpoint | Método HTTP | Acceso / Roles Autorizados | Código si Falla |
| :--- | :---: | :--- | :---: |
| `/` | `GET` | **Público** | — |
| `/v1/health` | `GET` | **Público** | — |
| `/v1/auth/login` | `POST` | **Público** (Sin token) | `401` (Credenciales inválidas) |
| `/v1/auth/logout` | `POST` | `Administrador`, `Operador`, `Auditor` | `401` (Token ausente/inválido) |
| `/v1/auth/me` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` (Token ausente/inválido) |
| `/v1/auth/usuarios` | `POST` | **`Administrador`** | `401` / `403` (Rol insuficiente) |
| `/v1/sucursales` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/sucursales/{id}` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/sucursales` | `POST` | `Administrador` | `401` / `403` |
| `/v1/sucursales/{id}` | `PUT` | `Administrador` | `401` / `403` |
| `/v1/sucursales/{id}` | `DELETE` | `Administrador` | `401` / `403` |
| `/v1/clientes` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/clientes/{id}` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/clientes` | `POST` | `Administrador`, `Operador` | `401` / `403` |
| `/v1/clientes/{id}` | `PUT` | `Administrador`, `Operador` | `401` / `403` |
| `/v1/clientes/{id}` | `DELETE` | `Administrador` | `401` / `403` |
| `/v1/rutas` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/rutas/{id}` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/rutas` | `POST` | `Administrador` | `401` / `403` |
| `/v1/rutas/{id}` | `PUT` | `Administrador` | `401` / `403` |
| `/v1/rutas/{id}` | `DELETE` | `Administrador` | `401` / `403` |
| `/v1/vehiculos` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/vehiculos/{id}` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/vehiculos` | `POST` | `Administrador` | `401` / `403` |
| `/v1/vehiculos/{id}` | `PUT` | `Administrador` | `401` / `403` |
| `/v1/vehiculos/{id}` | `DELETE` | `Administrador` | `401` / `403` |
| `/v1/pilotos` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/pilotos/{id}` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/pilotos` | `POST` | `Administrador` | `401` / `403` |
| `/v1/pilotos/{id}` | `PUT` | `Administrador` | `401` / `403` |
| `/v1/pilotos/{id}` | `DELETE` | `Administrador` | `401` / `403` |
| `/v1/tarifas` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/tarifas/{id}` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/tarifas` | `POST` | `Administrador` | `401` / `403` |
| `/v1/tarifas/{id}` | `PUT` | `Administrador` | `401` / `403` |
| `/v1/tarifas/{id}` | `DELETE` | `Administrador` | `401` / `403` |
| `/v1/transacciones/guias` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/transacciones/guias/{no}`| `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/transacciones/guias` | `POST` | `Administrador`, `Operador` | `401` / `403` |
| `/v1/transacciones/despacho` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/transacciones/despacho` | `POST` | `Administrador`, `Operador` | `401` / `403` |
| `/v1/reportes/rastreo/{no}` | `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/reportes/manifiesto/{id}`| `GET` | `Administrador`, `Operador`, `Auditor` | `401` |
| `/v1/reportes/liquidacion` | `GET` | `Administrador`, `Auditor` | `401` / `403` (`Operador` bloqueado) |

---

## 4. Ejemplos de Peticiones y Respuestas

### 4.1 Login Exitoso (`POST /v1/auth/login`)
**Solicitud:**
```http
POST /v1/auth/login HTTP/1.1
Host: 127.0.0.1:18080
Content-Type: application/json

{
  "username": "admin",
  "password": "admin123"
}
```

**Respuesta (`200 OK`):**
```json
{
  "success": true,
  "mensaje": "Autenticación exitosa",
  "access_token": "a7f9c2e0b1d4498aa95fd4829c79605e3f18cdba852f4a2aa6a67a2ac14d1591",
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

### 4.2 Solicitud Autenticada con Bearer Token
**Solicitud:**
```http
GET /v1/sucursales HTTP/1.1
Host: 127.0.0.1:18080
Authorization: Bearer a7f9c2e0b1d4498aa95fd4829c79605e3f18cdba852f4a2aa6a67a2ac14d1591
```

**Respuesta (`200 OK`):**
```json
{
  "success": true,
  "sucursales": [
    {
      "id": 1,
      "nombre": "Sucursal Central Guatemala",
      "direccion": "Zona 10, Ciudad de Guatemala",
      "telefono": "2222-0001"
    }
  ]
}
```

### 4.3 Acceso Denegado por Rol Insuficiente (`403 Forbidden`)
**Solicitud:**
```http
GET /v1/reportes/liquidacion HTTP/1.1
Host: 127.0.0.1:18080
Authorization: Bearer <token_de_operador>
```

**Respuesta (`403 Forbidden`):**
```json
{
  "success": false,
  "mensaje": "Acceso denegado: el rol 'Operador' no cuenta con permisos suficientes para esta operación"
}
```

---

## 5. Limitaciones y Riesgos Residuales

1. **Persistencia de Sesiones en Memoria:**
   Las sesiones no sobreviven al reinicio del proceso `./server`. Para un proyecto académico en C++17 sin bases de datos adicionales, esta es la decisión recomendada y segura. En un entorno de producción empresarial con balanceo de carga, se requeriría un almacén distribuido o JWT con rotación de claves.
2. **Capacidad del Campo `password[64]`:**
   El campo `char password[64]` limita la cadena de almacenamiento a 63 caracteres útiles. El formato `$pbk$` de 60 caracteres se ajusta a este tamaño. Si en futuras fases se requiere un algoritmo más largo (e.g. bcrypt de 60 caracteres o Argon2), se debe evaluar la migración del esquema binario y los archivos `.dat`.
