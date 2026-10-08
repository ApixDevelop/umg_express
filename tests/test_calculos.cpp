#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "sucursales.h"
#include "clientes.h"
#include "rutas.h"
#include "tarifas.h"
#include "transacciones.h"

const char* FILE_SUCURSALES_TEST_5B = "data/test_fase5b_sucursales.dat";
const char* FILE_CLIENTES_TEST_5B = "data/test_fase5b_clientes.dat";
const char* FILE_RUTAS_TEST_5B = "data/test_fase5b_rutas.dat";
const char* FILE_TARIFAS_TEST_5B = "data/test_fase5b_tarifas.dat";
const char* FILE_GUIAS_TEST_5B = "data/test_fase5b_guias.dat";

void limpiarArchivosTest5B() {
    std::remove(FILE_SUCURSALES_TEST_5B);
    std::remove(FILE_CLIENTES_TEST_5B);
    std::remove(FILE_RUTAS_TEST_5B);
    std::remove(FILE_TARIFAS_TEST_5B);
    std::remove(FILE_GUIAS_TEST_5B);
}

bool sonCasiIguales(double a, double b, double epsilon = 0.001) {
    return std::abs(a - b) < epsilon;
}

void probarFormulasMatematicasDirectas() {
    std::cout << "  [1] Verificando fórmulas matemáticas unitarias..." << std::endl;

    // Caso A: Paquete liviano pero voluminoso (50cm x 40cm x 30cm, peso real 2.0 kg)
    double pVolA = Guias::calcularPesoVolumetrico(50.0, 40.0, 30.0);
    assert(sonCasiIguales(pVolA, 12.0)); // (50 * 40 * 30) / 5000 = 60000 / 5000 = 12.0 kg
    
    double pFactA = Guias::calcularPesoFacturable(2.0, pVolA);
    assert(sonCasiIguales(pFactA, 12.0)); // max(2.0, 12.0) = 12.0 kg

    double volM3A = Guias::calcularVolumenM3(50.0, 40.0, 30.0);
    assert(sonCasiIguales(volM3A, 0.06)); // 60000 / 1000000 = 0.06 m3

    // Caso B: Paquete pequeño pero pesado (10cm x 10cm x 10cm, peso real 15.0 kg)
    double pVolB = Guias::calcularPesoVolumetrico(10.0, 10.0, 10.0);
    assert(sonCasiIguales(pVolB, 0.2)); // (10 * 10 * 10) / 5000 = 1000 / 5000 = 0.2 kg

    double pFactB = Guias::calcularPesoFacturable(15.0, pVolB);
    assert(sonCasiIguales(pFactB, 15.0)); // max(15.0, 0.2) = 15.0 kg

    // Caso C: Cálculo de costo total con Tarifa activa
    Tarifa t;
    std::memset(&t, 0, sizeof(Tarifa));
    t.id = 1;
    t.precioPorKg = 5.0;
    t.precioPorM3 = 20.0;
    t.recargoEspecial = 10.0;
    t.activo = true;

    double tarifaBaseRuta = 50.0;
    double costoTotalA = Guias::calcularCostoTotal(tarifaBaseRuta, pFactA, volM3A, &t);
    // Costo = 50.0 + (12.0 * 5.0) + (0.06 * 20.0) + 10.0 = 50.0 + 60.0 + 1.20 + 10.0 = 121.20
    assert(sonCasiIguales(costoTotalA, 121.20));

    std::cout << "      -> Fórmulas matemáticas verificadas correctamente." << std::endl;
}

void probarValidacionesYPersistenciaGuiasCalculadas() {
    std::cout << "  [2] Verificando validaciones y creación de guías con costos..." << std::endl;
    limpiarArchivosTest5B();

    std::string err;

    // 1. Crear datos de catálogo requeridos
    Sucursal s1, s2;
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_5B, "Sucursal Capital", "Zona 1", "2222-1111", s1, err);
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_5B, "Sucursal Xela", "Zona 3", "2222-3333", s2, err);

    Cliente c1, c2;
    Clientes::crearCliente(FILE_CLIENTES_TEST_5B, "111111-1", "Emisor S.A.", "Zona 1", "2444-1111", c1, err);
    Clientes::crearCliente(FILE_CLIENTES_TEST_5B, "222222-2", "Receptor S.A.", "Zona 3", "2444-2222", c2, err);

    Ruta r;
    Rutas::crearRuta(FILE_RUTAS_TEST_5B, FILE_SUCURSALES_TEST_5B, s1.id, s2.id, 200.0, 75.0, r, err); // tarifaBase = 75.0

    Tarifa tar;
    Tarifas::crearTarifa(FILE_TARIFAS_TEST_5B, "Frágil", 8.0, 30.0, 15.0, tar, err);

    // 2. Intentos con dimensiones o peso inválidos
    Guia gErr;
    bool okDim0 = Guias::crearGuia(FILE_GUIAS_TEST_5B, FILE_CLIENTES_TEST_5B, FILE_RUTAS_TEST_5B, FILE_TARIFAS_TEST_5B, c1.id, c2.id, r.id, 5.0, 0.0, 10.0, 10.0, tar.id, "", gErr, err);
    assert(!okDim0);
    std::cout << "      -> Rechazo dimensiones 0: '" << err << "'" << std::endl;

    bool okPeso0 = Guias::crearGuia(FILE_GUIAS_TEST_5B, FILE_CLIENTES_TEST_5B, FILE_RUTAS_TEST_5B, FILE_TARIFAS_TEST_5B, c1.id, c2.id, r.id, 0.0, 10.0, 10.0, 10.0, tar.id, "", gErr, err);
    assert(!okPeso0);
    std::cout << "      -> Rechazo peso 0: '" << err << "'" << std::endl;

    // 3. Crear guía 1: Paquete voluminoso (largo=60, ancho=50, alto=40 cm, peso real = 5 kg)
    // pVol = (60*50*40)/5000 = 120000/5000 = 24.0 kg
    // pFact = max(5.0, 24.0) = 24.0 kg
    // volM3 = 120000 / 1000000 = 0.12 m3
    // costo = 75.0 (tarifaBase) + (24.0 * 8.0) + (0.12 * 30.0) + 15.0 = 75.0 + 192.0 + 3.6 + 15.0 = 285.60
    Guia g1;
    bool okG1 = Guias::crearGuia(FILE_GUIAS_TEST_5B, FILE_CLIENTES_TEST_5B, FILE_RUTAS_TEST_5B, FILE_TARIFAS_TEST_5B, c1.id, c2.id, r.id, 5.0, 60.0, 50.0, 40.0, tar.id, "", g1, err);
    assert(okG1);
    assert(g1.numeroGuia == 1001);
    assert(sonCasiIguales(g1.pesoVolumetricoKg, 24.0));
    assert(sonCasiIguales(g1.pesoFacturableKg, 24.0));
    assert(sonCasiIguales(g1.costoTotal, 285.60));

    // 4. Verificar persistencia leyendo la guía almacenada en disco
    Guia gRead;
    bool okRead = Guias::obtenerGuiaPorNumero(FILE_GUIAS_TEST_5B, 1001, gRead);
    assert(okRead);
    assert(sonCasiIguales(gRead.pesoVolumetricoKg, 24.0));
    assert(sonCasiIguales(gRead.pesoFacturableKg, 24.0));
    assert(sonCasiIguales(gRead.costoTotal, 285.60));

    std::cout << "      -> Guía persistida y leída correctamente con todos los valores calculados." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 5B (PESO VOLUMÉTRICO Y COSTOS) ===" << std::endl;
    probarFormulasMatematicasDirectas();
    probarValidacionesYPersistenciaGuiasCalculadas();
    limpiarArchivosTest5B();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 5B COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
