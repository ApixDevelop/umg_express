# Guía de Despliegue de UMG Express en Railway

Este documento detalla el procedimiento técnico para desplegar el backend **UMG Express** (C++17, Crow, Persistencia Binaria `.dat`) en la plataforma de nube **Railway**, garantizando la integridad de los datos, el soporte de CORS para clientes web (React) y la correcta configuración de red.

---

## A. Arquitectura de Despliegue

La solución de UMG Express se compone de una arquitectura desacoplada:

```
[ Frontend Web en React ]
   (Vercel / Netlify / Railway)
              │
              │ HTTPS / JSON / Bearer Token
              ▼
┌────────────────────────────────────────────────────────┐
│               Railway Cloud Environment                │
│                                                        │
│  ┌──────────────────────────────────────────────────┐  │
│  │ Servicio Backend: UMG Express (C++17 + Crow)     │  │
│  │                                                  │  │
│  │  • Host: 0.0.0.0                                 │  │
│  │  • Puerto: ${PORT} asignado dinámicamente        │  │
│  │  • CORS habilitado (Permite peticiones externas) │  │
│  │  • Réplicas: 1 (Obligatorio para archivos .dat)  │  │
│  └──────────────────────────┬───────────────────────┘  │
│                             │ Montaje en /app/data     │
│                             ▼                          │
│  ┌──────────────────────────────────────────────────┐  │
│  │ Volumen Persistente Railway (Railway Volume)     │  │
│  │                                                  │  │
│  │  • usuarios.dat     • guias.dat                  │  │
│  │  • sucursales.dat   • despachos.dat              │  │
│  │  • clientes.dat     • pilotos.dat                │  │
│  │  • rutas.dat        • tarifas.dat                │  │
│  │  • vehiculos.dat                                 │  │
│  └──────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────┘
```

### Justificación de la Arquitectura
1. **Backend C++17 Independiente:** Este repositorio alberga exclusivamente el motor de API REST y persistencia binaria.
2. **Frontend React Externo:** El frontend se compila y aloja como servicio web independiente (pudiendo ser en Railway u otro proveedor como Vercel), consumiendo la API REST mediante HTTPS.
3. **Persistencia por Volumen Dedicado:** Dado que la persistencia académica se implementa con estructuras de tamaño fijo mediante C stdio (`fopen`, `fwrite`, `fseek`), los archivos `.dat` **deben residir en un volumen persistente** montado en el contenedor; de lo contrario, cualquier reinicio o nuevo despliegue borraría los registros.

---

## B. Prerrequisitos

Antes de iniciar el despliegue, asegúrese de contar con:

1. **Cuenta en Railway:** Con plan activo y acceso a Railway Dashboard ([https://railway.com](https://railway.com)).
2. **Repositorio Git:** Repositorio en GitHub con la rama `feature/devops-structure` (o `main`/`develop` una vez fusionada).
3. **Herramientas Locales Recomendadas (Opcional):**
   - Git CLI
   - Railway CLI (`npm i -g @railway/cli`) si desea gestionar por terminal.
   - `curl` para pruebas de conectividad y endpoints.

---

## C. Preparación del Repositorio

El repositorio incluye los siguientes archivos específicos para el despliegue:

* [`Dockerfile`](../Dockerfile): Construcción multi-etapa (*multi-stage*):
  - **Etapa 1 (`builder`):** Basada en `debian:bookworm-slim`, instala `g++`, `libasio-dev` y descarga los encabezados de Crow v1.3.4 para compilar un binario C++17 altamente optimizado (`-O2`).
  - **Etapa 2 (`runner`):** Imagen limpia con solo las dependencias dinámicas necesarias (`libstdc++6`, `ca-certificates`), reduciendo la superficie de ataque y el peso de la imagen a menos de 90 MB.
* [`.dockerignore`](../.dockerignore): Filtra código git, documentación, archivos temporales, scripts de testing y datos locales para que el contexto de compilación sea rápido y ligero.
* [`railway.json`](../railway.json): Configuración como código de Railway:
  - Fija `numReplicas: 1` para proteger los archivos binarios contra escrituras simultáneas incompatibles.
  - Declara el healthcheck en `/v1/health`.
* [`main.cpp`](../main.cpp):
  - Integración de `CORSMiddleware` (responde con encabezados `Access-Control-Allow-*` y maneja peticiones preflight `OPTIONS 204`).
  - Detección dinámica de la variable de entorno `PORT` proporcionada por Railway (con fallback local a `18080`).
  - Escucha en `0.0.0.0` (o configurable mediante `HOST`).
* [`include/persistence.h`](../include/persistence.h): Creación automática de los directorios padre (`asegurarDirectorioPadre`) mediante `std::filesystem::create_directories`, evitando errores al montar volúmenes limpios.
* [`include/auth.h`](../include/auth.h): Inicialización configurable de la contraseña del usuario inicial `admin` vía `ADMIN_DEFAULT_PASSWORD`.

---

## D. Creación del Proyecto y Servicio en Railway

Siga estos pasos en la consola web de Railway:

1. Inicie sesión en [Railway Dashboard](https://railway.com/dashboard).
2. Haga clic en **New Project**.
3. Seleccione **Deploy from GitHub repo**.
4. Elija el repositorio `umgExpress`.
5. Seleccione la rama a desplegar (ej. `feature/devops-structure` o `develop`).
6. Railway detectará automáticamente el archivo [`Dockerfile`](../Dockerfile) y [`railway.json`](../railway.json).

---

## E. Variables de Entorno

Configure las siguientes variables en la pestaña **Variables** del servicio en Railway:

| Nombre de Variable | Servicio | Propósito | Obligatoria | Valor sugerido / Configuración |
| :--- | :--- | :--- | :--- | :--- |
| `PORT` | Backend | Puerto TCP donde escucha Crow | **Automática** | Proporcionada automáticamente por Railway (ej. `8080` o aleatorio). La aplicación la lee nativamente. |
| `HOST` | Backend | Interfaz de enlace de red | Opcional | `0.0.0.0` (Es el valor por defecto en la aplicación para aceptar tráfico del proxy de Railway). |
| `DATA_DIR` | Backend | Ruta relativa o absoluta para los `.dat` | Opcional | `data` (Si el volumen se monta en `/app/data`, coincide con el directorio de trabajo). |
| `ADMIN_DEFAULT_PASSWORD` | Backend | Clave inicial para el usuario `admin` | **Recomendada** | Defina una contraseña robusta (ej. `AdminUmg2026!#Secure`). Si no se define, utiliza `admin123`. |

> [!WARNING]
> Nunca escriba contraseñas de producción en el código fuente ni en el archivo `README.md`. Utilice siempre la pestaña **Variables** de Railway.

---

## F. Configuración del Volumen Persistente (Persistencia de Archivos `.dat`)

Para que los datos de usuarios, catálogos, guías y liquidaciones **no se borren** al reiniciar el contenedor o desplegar actualizaciones:

1. En el proyecto de Railway, haga clic en **+ New** (o en el menú contextual del servicio).
2. Seleccione **Volume**.
3. Conecte el volumen al servicio del backend de UMG Express.
4. En los ajustes del volumen (*Volume Settings*), establezca la ruta de montaje (*Mount Path*):
   ```text
   /app/data
   ```
5. Establezca un tamaño inicial sugerido (ej. `1 GB` o `5 GB`, suficiente para decenas de miles de registros de tamaño fijo).
6. Al asociar el volumen, la aplicación creará automáticamente dentro de `/app/data` los archivos:
   - `usuarios.dat`
   - `sucursales.dat`
   - `clientes.dat`
   - `rutas.dat`
   - `vehiculos.dat`
   - `pilotos.dat`
   - `tarifas.dat`
   - `guias.dat`
   - `despachos.dat`

---

## G. Generación de Dominio Público y Conexión con Frontend

1. Vaya a la pestaña **Settings** del servicio backend en Railway.
2. En la sección **Networking**, haga clic en **Generate Domain**.
3. Railway asignará un dominio HTTPS público, por ejemplo:
   ```text
   https://umgexpress-production.up.railway.app
   ```
4. **Configuración en el Frontend (React):**
   - En su proyecto React (Vite o Create React App), configure la variable de entorno de conexión hacia la API:
     - Si usa Vite:
       ```env
       VITE_API_BASE_URL=https://umgexpress-production.up.railway.app
       ```
     - Si usa Create React App / Next.js:
       ```env
       REACT_APP_API_BASE_URL=https://umgexpress-production.up.railway.app
       ```
   - Al compilar el frontend, las llamadas HTTP (`fetch` o `axios`) apuntarán a este dominio seguro. El middleware CORS integrado en UMG Express permitirá la comunicación entre ambos dominios sin bloqueos por políticas de mismo origen (*Same-Origin Policy*).

---

## H. Primer Despliegue

1. Una vez asignadas las variables y montado el volumen, Railway iniciará automáticamente el proceso de construcción (*Build*).
2. En la pestaña **Deployments**, observe el registro en vivo (*Deploy Logs*):
   - Descarga de dependencias y encabezados Crow v1.3.4.
   - Compilación exitosa del binario `server` con `g++ -std=c++17`.
   - Empaquetado en la imagen final `runner`.
   - Inicio del servidor mostrando:
     ```text
     [PERSISTENCIA] Archivos .dat e infraestructura inicializada.
     [AUTH] Usuario por defecto 'admin' inicializado exitosamente.
     Servidor UMG Express escuchando en 0.0.0.0:<PORT>...
     ```
3. El *Healthcheck* de Railway consultará `GET /v1/health`. Una vez devuelva HTTP `200`, el despliegue cambiará a estado **Active** (Verde).

---

## I. Verificación Posterior al Despliegue

Ejecute las siguientes peticiones reales para comprobar que la API se encuentra operativa (reemplace `<DOMINIO_RAILWAY>` por su URL real):

### 1. Verificación de Salud (Health Check)
```bash
curl -i https://<DOMINIO_RAILWAY>/v1/health
```
*Respuesta esperada:* `HTTP/1.1 200 OK`
```json
{
  "modulo_auth": "OK",
  "modulo_catalogos": "OK",
  "modulo_guias": "OK",
  "servicio": "UMG Express API",
  "status": "UP"
}
```

### 2. Verificación de Preflight CORS (OPTIONS)
```bash
curl -i -X OPTIONS https://<DOMINIO_RAILWAY>/v1/auth/login
```
*Respuesta esperada:* `HTTP/1.1 204 No Content` con cabeceras `Access-Control-Allow-Origin: *`.

### 3. Autenticación de Usuario Administrador (Login)
```bash
curl -s -X POST https://<DOMINIO_RAILWAY>/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "admin123"}'
```
*Respuesta esperada:* `HTTP/1.1 200 OK` con el token Bearer en el campo `access_token`.

### 4. Consulta de Catálogos (ej. Sucursales)
```bash
curl -s -X GET https://<DOMINIO_RAILWAY>/v1/sucursales \
  -H "Authorization: Bearer <TOKEN_OBTENIDO>"
```

### 5. Reporte de Liquidación
```bash
curl -s -X GET https://<DOMINIO_RAILWAY>/v1/reportes/liquidacion \
  -H "Authorization: Bearer <TOKEN_OBTENIDO>"
```

---

## J. Copias de Seguridad, Actualizaciones y Recuperación

### Respaldo de Archivos Binarios `.dat`
Dado que los datos son archivos binarios locales en el volumen:
1. **Mediante Railway CLI:**
   Puede descargar el contenido de la carpeta `/app/data` utilizando:
   ```bash
   railway run tar -czvf - data/ > backup_umg_express_$(date +%Y%m%d).tar.gz
   ```
2. **Restauración:**
   Para restaurar una copia previa:
   ```bash
   railway run tar -xzvf backup_umg_express_YYYYMMDD.tar.gz
   ```

### Actualización de Nuevas Versiones
Al realizar `git push` a la rama configurada:
- Railway reconstruirá el contenedor.
- El volumen persistente `/app/data` permanecerá intacto y se conectará al nuevo contenedor sin pérdida de información.

---

## K. Limitaciones Técnicas y Restricciones Conocidas

1. **Escalado Horizontal Prohibido (`numReplicas = 1`):**
   - El sistema almacena los registros directamente en archivos binarios mediante punteros de archivo (`fseek`, `fwrite`, `fread`).
   - Múltiples contenedores escribiendo concurrentemente en el mismo sistema de archivos compartidos provocarían condiciones de carrera y corrupción de bloques binarios.
   - **Regla:** Mantenga siempre el servicio configurado con exactamente **1 réplica activa**.
2. **Concurrencia en Hilos:**
   - Crow atiende peticiones en múltiples hilos locales dentro del proceso. La persistencia utiliza aperturas atómicas (`a+b` / `r+b`) con buffers de C stdio. Para alta concurrencia masiva en producción, se recomienda un esquema transaccional o mutex a nivel de archivo si se incrementara sustancialmente la carga.
3. **Persistencia Efímera sin Volumen:**
   - Si no se asocia un Railway Volume al montar el contenedor, cualquier re-despliegue reiniciará la base de datos a su estado inicial. El montaje de `/app/data` es un requisito indispensable.
