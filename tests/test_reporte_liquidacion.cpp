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

const char* FILE_SUCURSALES_TEST_6C = "data/test_fase6c_sucursales.dat";
const char* FILE_CLIENTES_TEST_6C = "data/test_fase6c_clientes.dat";
const char* FILE_RUTAS_TEST_6C = "data/test_fase6c_rutas.dat";
const char* FILE_GUIAS_TEST_6C = "data/test_fase6c_guias.dat";

void limpiarArchivosTest6C() {
    std::remove(FILE_SUCURSALES_TEST_6C);
    std::remove(FILE_CLIENTES_TEST_6C);
    std::remove(FILE_RUTAS_TEST_6C);
    std::remove(FILE_GUIAS_TEST_6C);
}

void probarReporteLiquidacion() {
    std::cout << "[PRUEBA LIQUIDACIÓN] Verificación de liquidación financiera y consolidación de importes..." << std::endl;
    limpiarArchivosTest6C();

    std::string err;

    // -------------------------------------------------------------------------
    // PRUEBA 1: Reporte sin guías (archivo vacío)
    // -------------------------------------------------------------------------
    Reportes::DatosLiquidacion liqVacia;
    bool okVacia = Reportes::obtenerReporteLiquidacion(FILE_GUIAS_TEST_6C, FILE_RUTAS_TEST_6C, FILE_SUCURSALES_TEST_6C, liqVacia, err);
    assert(okVacia);
    assert(liqVacia.totalGuiasConsideradas == 0);
    assert(liqVacia.totalGuiasInactivas == 0);
    assert(liqVacia.importeTotalFacturado == 0.0);
    assert(liqVacia.cobradoConfirmado == 0.0);
    assert(liqVacia.guias.empty());
    std::cout << "  -> OK: Reporte sin guías retornó métricas en 0 e importes limpios." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 2: Liquidación con varias guías, diferentes importes y estados
    // -------------------------------------------------------------------------
    Sucursal sOrigen, sDestino;
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_6C, "Sucursal Guatemala Central", "Zona 1", "2222-1000", sOrigen, err);
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_6C, "Sucursal Quetzaltenango", "Zona 3 Xela", "2222-2000", sDestino, err);

    Cliente cRem, cDest;
    Clientes::crearCliente(FILE_CLIENTES_TEST_6C, "123456-7", "Empresa Remitente S.A.", "Zona 1", "2444-1000", cRem, err);
    Clientes::crearCliente(FILE_CLIENTES_TEST_6C, "765432-1", "Empresa Destino Ltda", "Zona 3", "2444-2000", cDest, err);

    Ruta r1;
    Rutas::crearRuta(FILE_RUTAS_TEST_6C, FILE_SUCURSALES_TEST_6C, sOrigen.id, sDestino.id, 200.0, 75.0, r1, err);

    // Crear Guía 1 (Creada - Q142.80)
    Guia g1;
    bool okG1 = Guias::crearGuia(FILE_GUIAS_TEST_6C, FILE_CLIENTES_TEST_6C, FILE_RUTAS_TEST_6C, cRem.id, cDest.id, r1.id, 10.0, 50.0, 40.0, 30.0, g1, err);
    assert(okG1); // Costo: 142.8

    // Crear Guía 2 (Creada - Q297.50)
    Guia g2;
    bool okG2 = Guias::crearGuia(FILE_GUIAS_TEST_6C, FILE_CLIENTES_TEST_6C, FILE_RUTAS_TEST_6C, cRem.id, cDest.id, r1.id, 25.0, 60.0, 50.0, 40.0, g2, err);
    assert(okG2); // Costo: 297.5

    // Cambiar estado de Guía 2 a "Entregada" para probar cobranza confirmada y agrupación de estados
    g2.activo = true;
    std::strncpy(g2.estado, "Entregada", sizeof(g2.estado) - 1);
    Persistence::actualizarEnIndice<Guia>(FILE_GUIAS_TEST_6C, 1, g2);

    // Crear Guía 3 (Cancelada / Inactiva - Q100.00)
    Guia g3;
    bool okG3 = Guias::crearGuia(FILE_GUIAS_TEST_6C, FILE_CLIENTES_TEST_6C, FILE_RUTAS_TEST_6C, cRem.id, cDest.id, r1.id, 5.0, 20.0, 20.0, 20.0, g3, err);
    assert(okG3);
    Persistence::eliminarGuiaLogico(FILE_GUIAS_TEST_6C, g3.numeroGuia); // Baja lógica / inactiva

    // Ejecutar reporte de liquidación
    Reportes::DatosLiquidacion liq;
    bool okLiq = Reportes::obtenerReporteLiquidacion(FILE_GUIAS_TEST_6C, FILE_RUTAS_TEST_6C, FILE_SUCURSALES_TEST_6C, liq, err);
    assert(okLiq);

    assert(liq.totalGuiasConsideradas == 2); // 2 guías activas
    assert(liq.totalGuiasInactivas == 1);     // 1 guía inactiva
    double importeEsperado = g1.costoTotal + g2.costoTotal; // 142.8 + 297.5 = 440.3
    assert(liq.importeTotalFacturado > (importeEsperado - 0.01) && liq.importeTotalFacturado < (importeEsperado + 0.01));
    assert(std::abs(liq.cobradoConfirmado - g2.costoTotal) < 0.01); // Cobrado confirmado = solo guías "Entregada"

    // Verificar desglose por estado
    assert(liq.resumenPorEstado.size() == 2); // "Creada" y "Entregada"
    std::cout << "  -> OK: Liquidación de guías con importes acumulados y desglose de estados verificado (Total Facturado: Q" << liq.importeTotalFacturado << ")." << std::endl;

    // -------------------------------------------------------------------------
    // PRUEBA 3: Manejo de persistencia nula (filepath = nullptr)
    // -------------------------------------------------------------------------
    Reportes::DatosLiquidacion liqNull;
    bool okNull = Reportes::obtenerReporteLiquidacion(nullptr, nullptr, nullptr, liqNull, err);
    assert(okNull);
    assert(liqNull.totalGuiasConsideradas == 0);
    std::cout << "  -> OK: Tolerancia a puntero nulo de archivo verificada." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 6C (REPORTE DE LIQUIDACIÓN) ===" << std::endl;
    probarReporteLiquidacion();
    limpiarArchivosTest6C();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 6C COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
