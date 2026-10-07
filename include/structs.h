#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstring>

// -----------------------------------------------------------------------------
// Estructura: Usuario
// Archivo: data/usuarios.dat
// -----------------------------------------------------------------------------
struct Usuario {
    int id;
    char username[32];
    char password[64];
    char nombre[64];
    char rol[20]; // "Administrador", "Operador", "Auditor"
    bool activo;
};

// -----------------------------------------------------------------------------
// Estructura: Sucursal
// Archivo: data/sucursales.dat
// -----------------------------------------------------------------------------
struct Sucursal {
    int id;
    char nombre[64];
    char direccion[128];
    char telefono[20];
    bool activo;
};

// -----------------------------------------------------------------------------
// Estructura: Cliente
// Archivo: data/clientes.dat
// -----------------------------------------------------------------------------
struct Cliente {
    int id;
    char nit[20];
    char nombre[64];
    char direccion[128];
    char telefono[20];
    bool activo;
};

// -----------------------------------------------------------------------------
// Estructura: Ruta
// Archivo: data/rutas.dat
// -----------------------------------------------------------------------------
struct Ruta {
    int id;
    int idSucursalOrigen;
    int idSucursalDestino;
    double distanciaKm;
    double tarifaBase;
    bool activo;
};

// -----------------------------------------------------------------------------
// Estructura: Vehiculo
// Archivo: data/vehiculos.dat
// -----------------------------------------------------------------------------
struct Vehiculo {
    int id;
    char placa[15];
    char marcaModelo[50];
    double capacidadPesoKg;
    double capacidadVolumenM3;
    char estado[20]; // "Disponible", "En ruta", "En taller"
    bool activo;
};

// -----------------------------------------------------------------------------
// Estructura: Piloto
// Archivo: data/pilotos.dat
// -----------------------------------------------------------------------------
struct Piloto {
    int id;
    char nombre[64];
    char licencia[20];
    char telefono[20];
    bool activo;
};

// -----------------------------------------------------------------------------
// Estructura: Tarifa
// Archivo: data/tarifas.dat
// -----------------------------------------------------------------------------
struct Tarifa {
    int id;
    char tipoCarga[32];
    double precioPorKg;
    double precioPorM3;
    double recargoEspecial;
    bool activo;
};

// -----------------------------------------------------------------------------
// Estructura: Guia
// Archivo: data/guias.dat
// -----------------------------------------------------------------------------
struct Guia {
    int numeroGuia;
    int idClienteRemitente;
    int idClienteDestinatario;
    int idRuta;
    double pesoRealKg;
    double largoCm;
    double anchoCm;
    double altoCm;
    double pesoVolumetricoKg;
    double pesoFacturableKg;
    double costoTotal;
    char estado[20]; // "Creada", "Despachada", "Entregada"
    bool activo;
};

#endif // STRUCTS_H
