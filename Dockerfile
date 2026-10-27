# ==============================================================================
# Etapa 1: Compilación (Build Stage)
# ==============================================================================
FROM debian:bookworm-slim AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Dependencias necesarias para compilar el backend C++17 con Crow:
# - g++ (compilador C++17 moderno)
# - libasio-dev (red/sockets asíncronos requeridos por Crow)
# - curl, ca-certificates, tar (para descargar los encabezados oficiales de Crow v1.3.4)
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    libasio-dev \
    curl \
    ca-certificates \
    tar \
    && rm -rf /var/lib/apt/lists/*

# Descargar e instalar encabezados oficiales de Crow v1.3.4
WORKDIR /tmp/crow
RUN curl -sSL https://github.com/CrowCpp/Crow/archive/refs/tags/v1.3.4.tar.gz | tar -xz --strip-components=1 \
    && cp -r include/* /usr/local/include/ \
    && rm -rf /tmp/crow

WORKDIR /app

# Copiar código fuente, encabezados y datos del proyecto
COPY include/ ./include/
COPY main.cpp ./
COPY data/ ./data/

# Compilar binario de producción optimizado (-O2)
RUN g++ -std=c++17 -O2 -I/usr/local/include -Iinclude main.cpp -o server -lpthread

# ==============================================================================
# Etapa 2: Imagen Final de Producción (Runtime Stage)
# ==============================================================================
FROM debian:bookworm-slim AS runner

ENV DEBIAN_FRONTEND=noninteractive

# Dependencias mínimas de ejecución en Debian
RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copiar el binario optimizado desde la etapa de compilación
COPY --from=builder /app/server /app/server

# Copiar carpeta de persistencia con datos iniciales del sistema
COPY --from=builder /app/data /app/data

# Configuración de red y persistencia
# Railway asigna dinámicamente la variable PORT en el despliegue
ENV PORT=18080 \
    HOST=0.0.0.0 \
    DATA_DIR=data

EXPOSE 18080

CMD ["./server"]
