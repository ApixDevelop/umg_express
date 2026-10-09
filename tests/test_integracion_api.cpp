#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "auth.h"
#include "sucursales.h"
#include "clientes.h"
#include "rutas.h"
#include "vehiculos.h"
#include "pilotos.h"
#include "tarifas.h"
#include "transacciones.h"
#include "reportes.h"

// Archivos temporales aislados para la suite de integración
const char* FILE_USUARIOS_INT = "data/test_int_usuarios.dat";
const char* FILE_SUCURSALES_INT = "data/test_int_sucursales.dat";
const char* FILE_CLIENTES_INT = "data/test_int_clientes.dat";
const char* FILE_RUTAS_INT = "data/test_int_rutas.dat";
const char* FILE_VEHICULOS_INT = "data/test_int_vehiculos.dat";
const char* FILE_PILOTOS_INT = "data/test_int_pilotos.dat";
const char* FILE_TARIFAS_INT = "data/test_int_tarifas.dat";
const char* FILE_GUIAS_INT = "data/test_int_guias.dat";
const char* FILE_DESPACHOS_INT = "data/test_int_despachos.dat";

void limpiarArchivosIntegracion() {
    std::remove(FILE_USUARIOS_INT);
    std::remove(FILE_SUCURSALES_INT);
    std::remove(FILE_CLIENTES_INT);
    std::remove(FILE_RUTAS_INT);
    std::remove(FILE_VEHICULOS_INT);
    std::remove(FILE_PILOTOS_INT);
    std::remove(FILE_TARIFAS_INT);
    std::remove(FILE_GUIAS_INT);
    std::remove(FILE_DESPACHOS_INT);
}

void probarFlujoCompletoIntegracion() {
    std::cout << "\n========================================================" << std::endl;
    std::cout << "  PRUEBA DE INTEGRACIÓN DE EXTREMO A EXTREMO (END-TO-END) " << std::endl;
    std::cout << "========================================================\n" << std::endl;

    limpiarArchivosIntegracion();
    std::string err;

    // -------------------------------------------------------------------------
    // FLUJO 1: Autenticación y Cuentas de Usuario
    // -------------------------------------------------------------------------
    std::cout << "[1/7] Probando módulo de Autenticación y Usuarios..." << std::endl;
    Auth::inicializarUsuarioAdminPorDefecto(FILE_USUARIOS_INT);

    Usuario adminUser;
    bool okAuthAdmin = Auth::autenticarUsuario(FILE_USUARIOS_INT, "admin", "admin123", adminUser, err);
    assert(okAuthAdmin);
    assert(std::strcmp(adminUser.rol, "Administrador") == 0);

    Usuario operUser;
    bool okRegOper = Auth::registrarUsuario(FILE_USUARIOS_INT, "operador1", "pass123", "Carlos López", "Operador", operUser, err);
    assert(okRegOper);

    Usuario dupUser;
    bool okDup = Auth::registrarUsuario(FILE_USUARIOS_INT, "operador1", "pass123", "Carlos López", "Operador", dupUser, err);
    assert(!okDup);
    assert(err.find("ya existe") != std::string::npos);
    std::cout << "  -> OK: Autenticación y registro de usuarios verificado." << std::endl;

    // -------------------------------------------------------------------------
    // FLUJO 2: Catálogos de la Empresa (Sucursales, Clientes, Rutas, Vehículos, Pilotos, Tarifas)
    // -------------------------------------------------------------------------
    std::cout << "[2/7] Probando mantenimiento CRUD de Catálogos..." << std::endl;

    // Sucursales
    Sucursal sGuate, sXela;
    assert(Sucursales::crearSucursal(FILE_SUCURSALES_INT, "Sucursal Guatemala Central", "Zona 1", "2222-1000", sGuate, err));
    assert(Sucursales::crearSucursal(FILE_SUCURSALES_INT, "Sucursal Quetzaltenango", "Zona 3 Xela", "2222-2000", sXela, err));

    // Clientes
    Cliente cRem, cDest;
    assert(Clientes::crearCliente(FILE_CLIENTES_INT, "100000-1", "Remitente Central S.A.", "Zona 1, Guatemala", "2444-1000", cRem, err));
    assert(Clientes::crearCliente(FILE_CLIENTES_INT, "200000-2", "Destinatario Xela Ltda", "Zona 3, Quetzaltenango", "2444-2000", cDest, err));

    // Rutas
    Ruta r1;
    assert(Rutas::crearRuta(FILE_RUTAS_INT, FILE_SUCURSALES_INT, sGuate.id, sXela.id, 200.0, 100.0, r1, err));

    // Vehículos
    Vehiculo vCamion;
    assert(Vehiculos::crearVehiculo(FILE_VEHICULOS_INT, "P-100AAA", "Isuzu NPR Truck", 500.0, 5.0, "Disponible", vCamion, err));

    // Pilotos
    Piloto piloto1;
    assert(Pilotos::crearPiloto(FILE_PILOTOS_INT, "Mario Estrada", "LIC-99988", "5555-1111", piloto1, err));

    // Tarifas
    Tarifa tarifaEstandar;
    assert(Tarifas::crearTarifa(FILE_TARIFAS_INT, "Estándar", 5.0, 10.0, 0.0, tarifaEstandar, err));

    std::cout << "  -> OK: Catálogos creados y enlazados relacionalmente." << std::endl;

    // -------------------------------------------------------------------------
    // FLUJO 3: Emisión de Guías y Cálculo Matemático de Pesos/Costos
    // -------------------------------------------------------------------------
    std::cout << "[3/7] Probando Emisión de Guías y Cálculos..." << std::endl;

    Guia g1001; // Peso real: 10kg. Dim: 50x40x30 cm -> Vol: 0.06m3, PesoVol: 12kg. PesoFacturable: 12kg
    bool okG1 = Guias::crearGuia(FILE_GUIAS_INT, FILE_CLIENTES_INT, FILE_RUTAS_INT, FILE_TARIFAS_INT, cRem.id, cDest.id, r1.id, 10.0, 50.0, 40.0, 30.0, tarifaEstandar.id, "", g1001, err);
    assert(okG1);
    assert(g1001.numeroGuia == 1001);
    assert(g1001.pesoVolumetricoKg == 12.0);
    assert(g1001.pesoFacturableKg == 12.0);
    assert(g1001.costoTotal > 0.0);
    assert(std::strcmp(g1001.estado, "Creada") == 0);

    Guia g1002; // Peso real: 30kg. Dim: 40x30x20 cm -> Vol: 0.024m3, PesoVol: 4.8kg. PesoFacturable: 30kg
    bool okG2 = Guias::crearGuia(FILE_GUIAS_INT, FILE_CLIENTES_INT, FILE_RUTAS_INT, FILE_TARIFAS_INT, cRem.id, cDest.id, r1.id, 30.0, 40.0, 30.0, 20.0, tarifaEstandar.id, "", g1002, err);
    assert(okG2);
    assert(g1002.numeroGuia == 1002);
    assert(g1002.pesoFacturableKg == 30.0);

    // Validación de rechazo: Remitente y Destinatario idénticos
    Guia gError;
    bool okSameClient = Guias::crearGuia(FILE_GUIAS_INT, FILE_CLIENTES_INT, FILE_RUTAS_INT, FILE_TARIFAS_INT, cRem.id, cRem.id, r1.id, 10.0, 10.0, 10.0, 10.0, 0, "", gError, err);
    assert(!okSameClient);
    assert(err.find("mismo") != std::string::npos);

    std::cout << "  -> OK: Guías emitidas con numeración correlativa y fórmulas validadas." << std::endl;

    // -------------------------------------------------------------------------
    // FLUJO 4: Despacho y Control Físico de Capacidad Vehicular
    // -------------------------------------------------------------------------
    std::cout << "[4/7] Probando Despacho y Control de Capacidad..." << std::endl;

    Despachos::ResultadoDespacho resDesp;
    bool okDespacho = Despachos::procesarDespacho(FILE_DESPACHOS_INT, FILE_GUIAS_INT, FILE_VEHICULOS_INT, FILE_PILOTOS_INT, vCamion.id, piloto1.id, {g1001.numeroGuia, g1002.numeroGuia}, resDesp, err);
    assert(okDespacho);
    assert(resDesp.totalGuias == 2);
    assert(resDesp.pesoTotalKg == (12.0 + 30.0)); // 42.0 kg
    assert(resDesp.estadoVehiculo == "En ruta");

    // Intentar re-despachar guía ya despachada (Rechazo 400)
    Despachos::ResultadoDespacho resDup;
    bool okReDesp = Despachos::procesarDespacho(FILE_DESPACHOS_INT, FILE_GUIAS_INT, FILE_VEHICULOS_INT, FILE_PILOTOS_INT, vCamion.id, piloto1.id, {g1001.numeroGuia}, resDup, err);
    assert(!okReDesp);
    assert(err.find("ya se encuentra despachada") != std::string::npos);

    std::cout << "  -> OK: Despacho procesado, capacidades controladas y guías actualizadas a 'Despachada'." << std::endl;

    // -------------------------------------------------------------------------
    // FLUJO 5: Módulo de Reportes (Rastreo 6A, Manifiesto 6B, Liquidación 6C)
    // -------------------------------------------------------------------------
    std::cout << "[5/7] Probando Integración de Módulos de Reportes (Fase 6A/6B/6C)..." << std::endl;

    // 5A: Rastreo
    Reportes::DatosRastreoGuia rastreo;
    assert(Reportes::obtenerRastreoGuia(FILE_GUIAS_INT, FILE_CLIENTES_INT, FILE_RUTAS_INT, FILE_SUCURSALES_INT, FILE_DESPACHOS_INT, FILE_VEHICULOS_INT, FILE_PILOTOS_INT, g1001.numeroGuia, rastreo, err));
    assert(rastreo.despachada);
    assert(std::strcmp(rastreo.vehiculo.placa, "P-100AAA") == 0);
    assert(std::strcmp(rastreo.piloto.nombre, "Mario Estrada") == 0);

    // 5B: Manifiesto
    Reportes::DatosManifiestoVehiculo manifiesto;
    assert(Reportes::obtenerManifiestoVehiculo(FILE_VEHICULOS_INT, FILE_DESPACHOS_INT, FILE_GUIAS_INT, FILE_PILOTOS_INT, FILE_RUTAS_INT, FILE_SUCURSALES_INT, FILE_CLIENTES_INT, vCamion.id, manifiesto, err));
    assert(manifiesto.totalGuias == 2);
    assert(manifiesto.pesoTotalKg == 42.0);

    // 5C: Liquidación
    Reportes::DatosLiquidacion liquidacion;
    assert(Reportes::obtenerReporteLiquidacion(FILE_GUIAS_INT, FILE_RUTAS_INT, FILE_SUCURSALES_INT, liquidacion, err));
    assert(liquidacion.totalGuiasConsideradas == 2);
    assert(liquidacion.importeTotalFacturado > 0.0);

    std::cout << "  -> OK: Reportes integrados y validados con datos persistidos." << std::endl;

    // -------------------------------------------------------------------------
    // FLUJO 6: Verificación de Persistencia tras Reinicio de Servidor
    // -------------------------------------------------------------------------
    std::cout << "[6/7] Simulando reinicio del servidor y verificación de datos en disco..." << std::endl;

    std::vector<Guia> guiasDisco = Guias::listarGuias(FILE_GUIAS_INT);
    assert(guiasDisco.size() == 2);
    assert(guiasDisco[0].numeroGuia == 1001);
    assert(std::strcmp(guiasDisco[0].estado, "Despachada") == 0);

    std::vector<Vehiculo> vehDisco = Vehiculos::listarVehiculos(FILE_VEHICULOS_INT);
    assert(vehDisco.size() == 1);
    assert(std::strcmp(vehDisco[0].estado, "En ruta") == 0);

    std::cout << "  -> OK: Todos los cambios persisten de manera consistente en archivos binarios .dat." << std::endl;

    // -------------------------------------------------------------------------
    // FLUJO 7: Limpieza final de recursos temporales de prueba
    // -------------------------------------------------------------------------
    std::cout << "[7/7] Limpiando archivos temporales de prueba..." << std::endl;
    limpiarArchivosIntegracion();
    std::cout << "  -> OK: Entorno de prueba limpio." << std::endl;
}

int main() {
    std::cout << "=== PRUEBA DE INTEGRACIÓN Y REVISIÓN GENERAL DE API (FASE 7) ===" << std::endl;
    probarFlujoCompletoIntegracion();
    std::cout << "\n¡TODAS LAS PRUEBAS DE INTEGRACIÓN DE LA FASE 7 COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
