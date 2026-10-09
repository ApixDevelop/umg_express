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

const char* FILE_SUCURSALES_TEST_5C = "data/test_fase5c_sucursales.dat";
const char* FILE_CLIENTES_TEST_5C = "data/test_fase5c_clientes.dat";
const char* FILE_RUTAS_TEST_5C = "data/test_fase5c_rutas.dat";
const char* FILE_VEHICULOS_TEST_5C = "data/test_fase5c_vehiculos.dat";
const char* FILE_PILOTOS_TEST_5C = "data/test_fase5c_pilotos.dat";
const char* FILE_GUIAS_TEST_5C = "data/test_fase5c_guias.dat";
const char* FILE_DESPACHOS_TEST_5C = "data/test_fase5c_despachos.dat";

void limpiarArchivosTest5C() {
    std::remove(FILE_SUCURSALES_TEST_5C);
    std::remove(FILE_CLIENTES_TEST_5C);
    std::remove(FILE_RUTAS_TEST_5C);
    std::remove(FILE_VEHICULOS_TEST_5C);
    std::remove(FILE_PILOTOS_TEST_5C);
    std::remove(FILE_GUIAS_TEST_5C);
    std::remove(FILE_DESPACHOS_TEST_5C);
}

void probarDespachoYCapacidad() {
    std::cout << "[PRUEBA DESPACHO] Procesamiento de despacho, control acumulativo y límites de capacidad..." << std::endl;
    limpiarArchivosTest5C();

    std::string err;

    // 1. Crear catálogos base
    Sucursal s1, s2;
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_5C, "Sucursal Guatemala", "Zona 1", "2222-1111", s1, err);
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_5C, "Sucursal Escuintla", "Km 55", "2222-2222", s2, err);

    Cliente cRem, cDest;
    Clientes::crearCliente(FILE_CLIENTES_TEST_5C, "111111-1", "Cliente Remitente", "Zona 1", "2444-1111", cRem, err);
    Clientes::crearCliente(FILE_CLIENTES_TEST_5C, "222222-2", "Cliente Destinatario", "Escuintla", "2444-2222", cDest, err);

    Ruta r;
    Rutas::crearRuta(FILE_RUTAS_TEST_5C, FILE_SUCURSALES_TEST_5C, s1.id, s2.id, 60.0, 40.0, r, err);

    Piloto piloto;
    Pilotos::crearPiloto(FILE_PILOTOS_TEST_5C, "Juan Pérez", "LIC-998877", "5555-1234", piloto, err);

    // 2. Crear vehículos con diferentes capacidades
    // Vehículo 1: Capacidad 100 kg, 1.0 m3, "Disponible"
    Vehiculo v1;
    Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_5C, "P-100AAA", "Isuzu NPR", 100.0, 1.0, "Disponible", v1, err);

    // Vehículo 2: Capacidad 1000 kg, 0.10 m3 (limitado en volumen)
    Vehiculo v2;
    Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_5C, "P-200BBB", "Hino 300", 1000.0, 0.10, "Disponible", v2, err);

    // Vehículo 3: "En taller"
    Vehiculo v3;
    Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_5C, "P-300CCC", "Hyundai HD65", 500.0, 5.0, "En taller", v3, err);

    // 3. Crear Guías de prueba
    // Guía 1001: 30 kg, 50x40x30 cm (0.06 m3)
    Guia g1001;
    Guias::crearGuia(FILE_GUIAS_TEST_5C, FILE_CLIENTES_TEST_5C, FILE_RUTAS_TEST_5C, cRem.id, cDest.id, r.id, 30.0, 50.0, 40.0, 30.0, g1001, err);

    // Guía 1002: 40 kg, 60x50x40 cm (0.12 m3)
    Guia g1002;
    Guias::crearGuia(FILE_GUIAS_TEST_5C, FILE_CLIENTES_TEST_5C, FILE_RUTAS_TEST_5C, cRem.id, cDest.id, r.id, 40.0, 60.0, 50.0, 40.0, g1002, err);

    // Guía 1003: 50 kg (para probar exceso de peso)
    Guia g1003;
    Guias::crearGuia(FILE_GUIAS_TEST_5C, FILE_CLIENTES_TEST_5C, FILE_RUTAS_TEST_5C, cRem.id, cDest.id, r.id, 50.0, 20.0, 20.0, 20.0, g1003, err);

    // Guía 1004: 10 kg, 100x100x100 cm (1.0 m3, para probar exceso de volumen)
    Guia g1004;
    Guias::crearGuia(FILE_GUIAS_TEST_5C, FILE_CLIENTES_TEST_5C, FILE_RUTAS_TEST_5C, cRem.id, cDest.id, r.id, 10.0, 100.0, 100.0, 100.0, g1004, err);

    // -------------------------------------------------------------------------
    // PRUEBA A: Rechazo por Vehículo "En taller"
    // -------------------------------------------------------------------------
    Despachos::ResultadoDespacho resFailTaller;
    bool okTaller = Despachos::procesarDespacho(FILE_DESPACHOS_TEST_5C, FILE_GUIAS_TEST_5C, FILE_VEHICULOS_TEST_5C, FILE_PILOTOS_TEST_5C, v3.id, piloto.id, {1001}, resFailTaller, err);
    assert(!okTaller);
    std::cout << "  -> OK: Vehículo en taller rechazado: '" << err << "'" << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA B: Exceso de Peso (30 + 40 + 50 = 120 kg > 100 kg capacidad v1)
    // -------------------------------------------------------------------------
    Despachos::ResultadoDespacho resFailPeso;
    bool okPesoExcedido = Despachos::procesarDespacho(FILE_DESPACHOS_TEST_5C, FILE_GUIAS_TEST_5C, FILE_VEHICULOS_TEST_5C, FILE_PILOTOS_TEST_5C, v1.id, piloto.id, {1001, 1002, 1003}, resFailPeso, err);
    assert(!okPesoExcedido);
    std::cout << "  -> OK: Exceso de peso rechazado: '" << err << "'" << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA C: Exceso de Volumen (1.0 m3 > 0.10 m3 capacidad v2)
    // -------------------------------------------------------------------------
    Despachos::ResultadoDespacho resFailVol;
    bool okVolExcedido = Despachos::procesarDespacho(FILE_DESPACHOS_TEST_5C, FILE_GUIAS_TEST_5C, FILE_VEHICULOS_TEST_5C, FILE_PILOTOS_TEST_5C, v2.id, piloto.id, {1004}, resFailVol, err);
    assert(!okVolExcedido);
    std::cout << "  -> OK: Exceso de volumen rechazado: '" << err << "'" << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA D: Despacho Exitoso (Guías 1001 + 1002 en Vehículo v1)
    // Peso: 30 + 40 = 70 kg <= 100 kg. Volumen: 0.06 + 0.12 = 0.18 m3 <= 1.0 m3
    // -------------------------------------------------------------------------
    Despachos::ResultadoDespacho resOK;
    bool okDespacho = Despachos::procesarDespacho(FILE_DESPACHOS_TEST_5C, FILE_GUIAS_TEST_5C, FILE_VEHICULOS_TEST_5C, FILE_PILOTOS_TEST_5C, v1.id, piloto.id, {1001, 1002}, resOK, err);
    assert(okDespacho);
    assert(resOK.totalGuias == 2);
    assert(resOK.pesoTotalKg == 70.0);
    assert(resOK.estadoVehiculo == "En ruta");
    std::cout << "  -> OK: Despacho exitoso procesado correctamente." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA E: Rechazo al intentar despachar guía ya despachada (Guía 1001)
    // -------------------------------------------------------------------------
    Despachos::ResultadoDespacho resFailYaDespachada;
    bool okYaDesp = Despachos::procesarDespacho(FILE_DESPACHOS_TEST_5C, FILE_GUIAS_TEST_5C, FILE_VEHICULOS_TEST_5C, FILE_PILOTOS_TEST_5C, v1.id, piloto.id, {1001}, resFailYaDespachada, err);
    assert(!okYaDesp);
    std::cout << "  -> OK: Guía ya despachada rechazada: '" << err << "'" << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA F: Persistencia en disco tras lectura posterior
    // -------------------------------------------------------------------------
    Guia g1Check, g2Check;
    Guias::obtenerGuiaPorNumero(FILE_GUIAS_TEST_5C, 1001, g1Check);
    Guias::obtenerGuiaPorNumero(FILE_GUIAS_TEST_5C, 1002, g2Check);
    assert(std::strcmp(g1Check.estado, "Despachada") == 0);
    assert(std::strcmp(g2Check.estado, "Despachada") == 0);

    Vehiculo v1Check;
    Vehiculos::obtenerVehiculoPorId(FILE_VEHICULOS_TEST_5C, v1.id, v1Check);
    assert(std::strcmp(v1Check.estado, "En ruta") == 0);

    std::vector<Despacho> listaDesp = Despachos::listarDespachos(FILE_DESPACHOS_TEST_5C);
    assert(listaDesp.size() == 2);
    assert(listaDesp[0].numeroGuia == 1001);
    assert(listaDesp[1].numeroGuia == 1002);
    std::cout << "  -> OK: Persistencia verificada correctamente en data/*.dat." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 5C (DESPACHO Y VALIDACIÓN DE CAPACIDAD) ===" << std::endl;
    probarDespachoYCapacidad();
    limpiarArchivosTest5C();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 5C COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
