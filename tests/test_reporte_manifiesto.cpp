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

const char* FILE_SUCURSALES_TEST_6B = "data/test_fase6b_sucursales.dat";
const char* FILE_CLIENTES_TEST_6B = "data/test_fase6b_clientes.dat";
const char* FILE_RUTAS_TEST_6B = "data/test_fase6b_rutas.dat";
const char* FILE_VEHICULOS_TEST_6B = "data/test_fase6b_vehiculos.dat";
const char* FILE_PILOTOS_TEST_6B = "data/test_fase6b_pilotos.dat";
const char* FILE_GUIAS_TEST_6B = "data/test_fase6b_guias.dat";
const char* FILE_DESPACHOS_TEST_6B = "data/test_fase6b_despachos.dat";

void limpiarArchivosTest6B() {
    std::remove(FILE_SUCURSALES_TEST_6B);
    std::remove(FILE_CLIENTES_TEST_6B);
    std::remove(FILE_RUTAS_TEST_6B);
    std::remove(FILE_VEHICULOS_TEST_6B);
    std::remove(FILE_PILOTOS_TEST_6B);
    std::remove(FILE_GUIAS_TEST_6B);
    std::remove(FILE_DESPACHOS_TEST_6B);
}

void probarReporteManifiestoVehiculo() {
    std::cout << "[PRUEBA MANIFIESTO VEHÍCULO] Verificación de manifiestos con múltiples guías, sin guías y vehículos inexistentes..." << std::endl;
    limpiarArchivosTest6B();

    std::string err;

    // 1. Configurar infraestructura inicial (sucursales, clientes, rutas, vehículos, pilotos)
    Sucursal sOrigen, sDestino;
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_6B, "Sucursal Guatemala Central", "Zona 1", "2222-1000", sOrigen, err);
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_6B, "Sucursal Quetzaltenango", "Zona 3 Xela", "2222-2000", sDestino, err);

    Cliente cRem, cDest;
    Clientes::crearCliente(FILE_CLIENTES_TEST_6B, "123456-7", "Distribuidora Central S.A.", "Zona 1", "2444-1000", cRem, err);
    Clientes::crearCliente(FILE_CLIENTES_TEST_6B, "765432-1", "Comercial Xela Ltda", "Zona 3", "2444-2000", cDest, err);

    Ruta r;
    Rutas::crearRuta(FILE_RUTAS_TEST_6B, FILE_SUCURSALES_TEST_6B, sOrigen.id, sDestino.id, 200.0, 75.0, r, err);

    Vehiculo v1, v2;
    Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_6B, "P-100AAA", "Isuzu NPR 2022", 1000.0, 10.0, "Disponible", v1, err);
    Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_6B, "P-200BBB", "Hino Truck 500", 2000.0, 20.0, "Disponible", v2, err);

    Piloto piloto1;
    Pilotos::crearPiloto(FILE_PILOTOS_TEST_6B, "Mario Estrada", "LIC-12345", "5555-1111", piloto1, err);

    // 2. Crear Guías (1001, 1002, 1003)
    Guia g1001, g1002, g1003;
    bool okG1 = Guias::crearGuia(FILE_GUIAS_TEST_6B, FILE_CLIENTES_TEST_6B, FILE_RUTAS_TEST_6B, cRem.id, cDest.id, r.id, 10.0, 50.0, 40.0, 30.0, g1001, err); // Vol: 0.06 m3, pesoFact: 12 kg (50x40x30/5000=12)
    bool okG2 = Guias::crearGuia(FILE_GUIAS_TEST_6B, FILE_CLIENTES_TEST_6B, FILE_RUTAS_TEST_6B, cRem.id, cDest.id, r.id, 25.0, 60.0, 50.0, 40.0, g1002, err); // Vol: 0.12 m3, pesoFact: 25 kg (60x50x40/5000=24, real 25)
    bool okG3 = Guias::crearGuia(FILE_GUIAS_TEST_6B, FILE_CLIENTES_TEST_6B, FILE_RUTAS_TEST_6B, cRem.id, cDest.id, r.id, 5.0, 30.0, 20.0, 10.0, g1003, err);  // Para Vehículo 2

    assert(okG1 && okG2 && okG3);

    // Despachar guías 1001 y 1002 en Vehículo 1 (v1.id)
    Despachos::ResultadoDespacho resDesp1;
    bool okDesp1 = Despachos::procesarDespacho(FILE_DESPACHOS_TEST_6B, FILE_GUIAS_TEST_6B, FILE_VEHICULOS_TEST_6B, FILE_PILOTOS_TEST_6B, v1.id, piloto1.id, {g1001.numeroGuia, g1002.numeroGuia}, resDesp1, err);
    assert(okDesp1);

    // Despachar guía 1003 en Vehículo 2 (v2.id) con otro despacho
    Despachos::ResultadoDespacho resDesp2;
    bool okDesp2 = Despachos::procesarDespacho(FILE_DESPACHOS_TEST_6B, FILE_GUIAS_TEST_6B, FILE_VEHICULOS_TEST_6B, FILE_PILOTOS_TEST_6B, v2.id, piloto1.id, {g1003.numeroGuia}, resDesp2, err);
    assert(okDesp2);

    // -------------------------------------------------------------------------
    // PRUEBA 1: Manifiesto de Vehículo 1 con guías 1001 y 1002
    // -------------------------------------------------------------------------
    Reportes::DatosManifiestoVehiculo m1;
    bool okM1 = Reportes::obtenerManifiestoVehiculo(FILE_VEHICULOS_TEST_6B, FILE_DESPACHOS_TEST_6B, FILE_GUIAS_TEST_6B, FILE_PILOTOS_TEST_6B, FILE_RUTAS_TEST_6B, FILE_SUCURSALES_TEST_6B, FILE_CLIENTES_TEST_6B, v1.id, m1, err);
    assert(okM1);
    assert(m1.vehiculo.id == v1.id);
    assert(std::strcmp(m1.vehiculo.placa, "P-100AAA") == 0);
    assert(m1.tienePiloto);
    assert(std::strcmp(m1.piloto.nombre, "Mario Estrada") == 0);
    assert(m1.totalGuias == 2);
    assert(m1.guias.size() == 2);
    assert(m1.pesoTotalKg == (12.0 + 25.0)); // 37.0 kg
    assert(m1.volumenTotalM3 > 0.179 && m1.volumenTotalM3 < 0.181); // 0.06 + 0.12 = 0.18 m3
    std::cout << "  -> OK: Manifiesto de vehículo con múltiples guías verificado (Total Guías: " << m1.totalGuias << ", Peso Total: " << m1.pesoTotalKg << " kg)." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 2: Exclusión de guías de otros vehículos (Guía 1003 no debe estar en Vehículo 1)
    // -------------------------------------------------------------------------
    for (const auto& gm : m1.guias) {
        assert(gm.guia.numeroGuia != g1003.numeroGuia);
    }
    std::cout << "  -> OK: Exclusión de guías de otros vehículos verificada." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 3: Vehículo existente sin guías asignadas
    // -------------------------------------------------------------------------
    Vehiculo vSinGuias;
    Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_6B, "P-300CCC", "Volvo FM 400", 3000.0, 30.0, "Disponible", vSinGuias, err);

    Reportes::DatosManifiestoVehiculo mSinGuias;
    bool okMSin = Reportes::obtenerManifiestoVehiculo(FILE_VEHICULOS_TEST_6B, FILE_DESPACHOS_TEST_6B, FILE_GUIAS_TEST_6B, FILE_PILOTOS_TEST_6B, FILE_RUTAS_TEST_6B, FILE_SUCURSALES_TEST_6B, FILE_CLIENTES_TEST_6B, vSinGuias.id, mSinGuias, err);
    assert(okMSin);
    assert(mSinGuias.vehiculo.id == vSinGuias.id);
    assert(mSinGuias.totalGuias == 0);
    assert(mSinGuias.guias.empty());
    assert(mSinGuias.pesoTotalKg == 0.0);
    assert(mSinGuias.volumenTotalM3 == 0.0);
    std::cout << "  -> OK: Vehículo sin guías asignadas devolvió manifiesto válido con lista vacía y totales en cero." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 4: Vehículo inexistente (HTTP 404)
    // -------------------------------------------------------------------------
    Reportes::DatosManifiestoVehiculo mInexistente;
    bool okMInex = Reportes::obtenerManifiestoVehiculo(FILE_VEHICULOS_TEST_6B, FILE_DESPACHOS_TEST_6B, FILE_GUIAS_TEST_6B, FILE_PILOTOS_TEST_6B, FILE_RUTAS_TEST_6B, FILE_SUCURSALES_TEST_6B, FILE_CLIENTES_TEST_6B, 9999, mInexistente, err);
    assert(!okMInex);
    assert(err.find("no encontrado") != std::string::npos);
    std::cout << "  -> OK: Vehículo inexistente correctamente rechazado: '" << err << "'" << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 5: Identificador inválido (-1)
    // -------------------------------------------------------------------------
    Reportes::DatosManifiestoVehiculo mInvalido;
    bool okMInval = Reportes::obtenerManifiestoVehiculo(FILE_VEHICULOS_TEST_6B, FILE_DESPACHOS_TEST_6B, FILE_GUIAS_TEST_6B, FILE_PILOTOS_TEST_6B, FILE_RUTAS_TEST_6B, FILE_SUCURSALES_TEST_6B, FILE_CLIENTES_TEST_6B, -1, mInvalido, err);
    assert(!okMInval);
    std::cout << "  -> OK: Identificador inválido (-1) rechazado correctamente." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 6: Ausencia de duplicados al procesar múltiples despachos del mismo vehículo
    // -------------------------------------------------------------------------
    // Agregar un registro de despacho duplicado manualmente para simular reintento o histórico
    Despacho dDup;
    dDup.id = 99;
    dDup.idVehiculo = v1.id;
    dDup.idPiloto = piloto1.id;
    dDup.numeroGuia = g1001.numeroGuia; // Guía 1001 duplicada
    dDup.pesoKg = 12.0;
    dDup.volumenM3 = 0.06;
    std::strncpy(dDup.fechaDespacho, "2026-10-09 10:00:00", sizeof(dDup.fechaDespacho) - 1);
    dDup.activo = true;
    Persistence::guardarRegistro<Despacho>(FILE_DESPACHOS_TEST_6B, dDup);

    Reportes::DatosManifiestoVehiculo mDup;
    bool okMDup = Reportes::obtenerManifiestoVehiculo(FILE_VEHICULOS_TEST_6B, FILE_DESPACHOS_TEST_6B, FILE_GUIAS_TEST_6B, FILE_PILOTOS_TEST_6B, FILE_RUTAS_TEST_6B, FILE_SUCURSALES_TEST_6B, FILE_CLIENTES_TEST_6B, v1.id, mDup, err);
    assert(okMDup);
    assert(mDup.totalGuias == 2); // Debe seguir siendo 2 guías únicas
    assert(mDup.guias.size() == 2);
    std::cout << "  -> OK: Desduplicación de guías en el manifiesto verificada exitosamente." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 6B (REPORTE DE MANIFIESTO DE VEHÍCULO) ===" << std::endl;
    probarReporteManifiestoVehiculo();
    limpiarArchivosTest6B();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 6B COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
