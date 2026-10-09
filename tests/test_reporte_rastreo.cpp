#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "sucursales.h"
#include "clientes.h"
#include "rutas.h"
#include "vehiculos.h"
#include "pilotos.h"
#include "transacciones.h"
#include "reportes.h"

const char* FILE_SUCURSALES_TEST_6A = "data/test_fase6a_sucursales.dat";
const char* FILE_CLIENTES_TEST_6A = "data/test_fase6a_clientes.dat";
const char* FILE_RUTAS_TEST_6A = "data/test_fase6a_rutas.dat";
const char* FILE_VEHICULOS_TEST_6A = "data/test_fase6a_vehiculos.dat";
const char* FILE_PILOTOS_TEST_6A = "data/test_fase6a_pilotos.dat";
const char* FILE_GUIAS_TEST_6A = "data/test_fase6a_guias.dat";
const char* FILE_DESPACHOS_TEST_6A = "data/test_fase6a_despachos.dat";

void limpiarArchivosTest6A() {
    std::remove(FILE_SUCURSALES_TEST_6A);
    std::remove(FILE_CLIENTES_TEST_6A);
    std::remove(FILE_RUTAS_TEST_6A);
    std::remove(FILE_VEHICULOS_TEST_6A);
    std::remove(FILE_PILOTOS_TEST_6A);
    std::remove(FILE_GUIAS_TEST_6A);
    std::remove(FILE_DESPACHOS_TEST_6A);
}

void probarReporteRastreoGuias() {
    std::cout << "[PRUEBA RASTREO GUIA] Verificación de reporte de rastreo para guías creadas y despachadas..." << std::endl;
    limpiarArchivosTest6A();

    std::string err;

    // 1. Crear catálogos base
    Sucursal sOrigen, sDestino;
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_6A, "Sucursal Guatemala Central", "Zona 1", "2222-1000", sOrigen, err);
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_6A, "Sucursal Quetzaltenango", "Zona 3 Xela", "2222-2000", sDestino, err);

    Cliente cRem, cDest;
    Clientes::crearCliente(FILE_CLIENTES_TEST_6A, "123456-7", "Remitente Guatemala S.A.", "Zona 1", "2444-1000", cRem, err);
    Clientes::crearCliente(FILE_CLIENTES_TEST_6A, "765432-1", "Destinatario Xela Ltda", "Zona 3", "2444-2000", cDest, err);

    Ruta r;
    Rutas::crearRuta(FILE_RUTAS_TEST_6A, FILE_SUCURSALES_TEST_6A, sOrigen.id, sDestino.id, 200.0, 75.0, r, err);

    Vehiculo v;
    Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_6A, "P-555XYZ", "Hino Truck 500", 500.0, 5.0, "Disponible", v, err);

    Piloto piloto;
    Pilotos::crearPiloto(FILE_PILOTOS_TEST_6A, "Carlos Mendizábal", "LIC-77711", "5555-4321", piloto, err);

    // 2. Crear Guía 1001 (Estado inicial: "Creada")
    Guia g1001;
    bool okG1 = Guias::crearGuia(FILE_GUIAS_TEST_6A, FILE_CLIENTES_TEST_6A, FILE_RUTAS_TEST_6A, cRem.id, cDest.id, r.id, 10.0, 40.0, 30.0, 20.0, g1001, err);
    assert(okG1);
    assert(g1001.numeroGuia == 1001);

    // -------------------------------------------------------------------------
    // PRUEBA 1: Rastreo de guía existente no despachada ("Creada")
    // -------------------------------------------------------------------------
    Reportes::DatosRastreoGuia datosCreada;
    bool okRastreoCreada = Reportes::obtenerRastreoGuia(FILE_GUIAS_TEST_6A, FILE_CLIENTES_TEST_6A, FILE_RUTAS_TEST_6A, FILE_SUCURSALES_TEST_6A, FILE_DESPACHOS_TEST_6A, FILE_VEHICULOS_TEST_6A, FILE_PILOTOS_TEST_6A, 1001, datosCreada, err);
    assert(okRastreoCreada);
    assert(datosCreada.guia.numeroGuia == 1001);
    assert(std::strcmp(datosCreada.guia.estado, "Creada") == 0);
    assert(datosCreada.tieneRemitente);
    assert(datosCreada.tieneDestinatario);
    assert(datosCreada.tieneRuta);
    assert(datosCreada.tieneSucursalOrigen);
    assert(datosCreada.tieneSucursalDestino);
    assert(std::strcmp(datosCreada.sucursalOrigen.nombre, "Sucursal Guatemala Central") == 0);
    assert(std::strcmp(datosCreada.sucursalDestino.nombre, "Sucursal Quetzaltenango") == 0);
    assert(!datosCreada.despachada);
    std::cout << "  -> OK: Rastreo de guía en estado 'Creada' verificado exitosamente." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 2: Rastreo de guía despachada ("Despachada")
    // -------------------------------------------------------------------------
    Despachos::ResultadoDespacho resDesp;
    bool okDesp = Despachos::procesarDespacho(FILE_DESPACHOS_TEST_6A, FILE_GUIAS_TEST_6A, FILE_VEHICULOS_TEST_6A, FILE_PILOTOS_TEST_6A, v.id, piloto.id, {1001}, resDesp, err);
    assert(okDesp);

    Reportes::DatosRastreoGuia datosDespachada;
    bool okRastreoDesp = Reportes::obtenerRastreoGuia(FILE_GUIAS_TEST_6A, FILE_CLIENTES_TEST_6A, FILE_RUTAS_TEST_6A, FILE_SUCURSALES_TEST_6A, FILE_DESPACHOS_TEST_6A, FILE_VEHICULOS_TEST_6A, FILE_PILOTOS_TEST_6A, 1001, datosDespachada, err);
    assert(okRastreoDesp);
    assert(std::strcmp(datosDespachada.guia.estado, "Despachada") == 0);
    assert(datosDespachada.despachada);
    assert(datosDespachada.tieneVehiculo);
    assert(std::strcmp(datosDespachada.vehiculo.placa, "P-555XYZ") == 0);
    assert(datosDespachada.tienePiloto);
    assert(std::strcmp(datosDespachada.piloto.nombre, "Carlos Mendizábal") == 0);
    std::cout << "  -> OK: Rastreo de guía en estado 'Despachada' con datos de vehículo y piloto verificado exitosamente." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 3: Rastreo de guía inexistente (HTTP 404)
    // -------------------------------------------------------------------------
    Reportes::DatosRastreoGuia datosInexistente;
    bool okRastreoInexistente = Reportes::obtenerRastreoGuia(FILE_GUIAS_TEST_6A, FILE_CLIENTES_TEST_6A, FILE_RUTAS_TEST_6A, FILE_SUCURSALES_TEST_6A, FILE_DESPACHOS_TEST_6A, FILE_VEHICULOS_TEST_6A, FILE_PILOTOS_TEST_6A, 9999, datosInexistente, err);
    assert(!okRastreoInexistente);
    assert(err.find("no encontrada") != std::string::npos);
    std::cout << "  -> OK: Guía inexistente correctamente rechazada: '" << err << "'" << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 6A (REPORTE DE RASTREO DE GUÍAS) ===" << std::endl;
    probarReporteRastreoGuias();
    limpiarArchivosTest6A();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 6A COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
