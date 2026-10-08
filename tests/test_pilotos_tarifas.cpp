#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "pilotos.h"
#include "tarifas.h"

const char* FILE_PILOTOS_TEST_4C = "data/test_fase4c_pilotos.dat";
const char* FILE_TARIFAS_TEST_4C = "data/test_fase4c_tarifas.dat";

void limpiarArchivosTest() {
    std::remove(FILE_PILOTOS_TEST_4C);
    std::remove(FILE_TARIFAS_TEST_4C);
}

void probarCRUDPilotos() {
    std::cout << "[PRUEBA PILOTOS] Creación, validación de licencias, búsqueda, actualización y baja..." << std::endl;
    limpiarArchivosTest();

    Piloto pOut;
    std::string err;

    // 1. Crear piloto 1
    bool ok1 = Pilotos::crearPiloto(FILE_PILOTOS_TEST_4C, "Mario Estrada", "LIC-12345", "5555-1111", pOut, err);
    assert(ok1);
    assert(pOut.id == 1);
    assert(std::strcmp(pOut.nombre, "Mario Estrada") == 0);
    assert(std::strcmp(pOut.licencia, "LIC-12345") == 0);

    // 2. Intento de licencia duplicada
    Piloto pDup;
    bool okDup = Pilotos::crearPiloto(FILE_PILOTOS_TEST_4C, "Otro Piloto", "LIC-12345", "5555-2222", pDup, err);
    assert(!okDup);
    std::cout << "  -> OK: Licencia duplicada rechazada: '" << err << "'" << std::endl;

    // 3. Crear piloto 2
    Piloto pOut2;
    bool ok2 = Pilotos::crearPiloto(FILE_PILOTOS_TEST_4C, "Luis Morales", "LIC-67890", "5555-3333", pOut2, err);
    assert(ok2);
    assert(pOut2.id == 2);

    // 4. Buscar por ID
    Piloto pBuscado;
    bool okBus = Pilotos::obtenerPilotoPorId(FILE_PILOTOS_TEST_4C, 1, pBuscado);
    assert(okBus);
    assert(std::strcmp(pBuscado.telefono, "5555-1111") == 0);

    // 5. Actualizar piloto
    Piloto pAct;
    bool okActualizado = Pilotos::actualizarPiloto(FILE_PILOTOS_TEST_4C, 1, "Mario Estrada Perez", "LIC-12345", "5555-9999", pAct, err);
    assert(okActualizado);
    assert(std::strcmp(pAct.nombre, "Mario Estrada Perez") == 0);

    // 6. Baja lógica
    bool okElim = Pilotos::eliminarPiloto(FILE_PILOTOS_TEST_4C, 2, err);
    assert(okElim);

    std::vector<Piloto> listaActivos = Pilotos::listarPilotos(FILE_PILOTOS_TEST_4C);
    assert(listaActivos.size() == 1);
    assert(listaActivos[0].id == 1);

    std::cout << "  -> OK: CRUD de Pilotos funcionando correctamente." << std::endl;
}

void probarCRUDTarifas() {
    std::cout << "[PRUEBA TARIFAS] Creación, validación de tipo de carga, precios, actualización y baja..." << std::endl;

    Tarifa tOut;
    std::string err;

    // 1. Crear tarifa 1 (Estándar)
    bool ok1 = Tarifas::crearTarifa(FILE_TARIFAS_TEST_4C, "Estándar", 10.5, 15.0, 0.0, tOut, err);
    assert(ok1);
    assert(tOut.id == 1);
    assert(std::strcmp(tOut.tipoCarga, "Estándar") == 0);
    assert(tOut.precioPorKg == 10.5);

    // 2. Intento de tipo de carga duplicado
    Tarifa tDup;
    bool okDup = Tarifas::crearTarifa(FILE_TARIFAS_TEST_4C, "Estándar", 12.0, 18.0, 5.0, tDup, err);
    assert(!okDup);
    std::cout << "  -> OK: Tipo de carga duplicado rechazado: '" << err << "'" << std::endl;

    // 3. Intento de precio negativo
    Tarifa tNeg;
    bool okNeg = Tarifas::crearTarifa(FILE_TARIFAS_TEST_4C, "Peligroso", -5.0, 20.0, 10.0, tNeg, err);
    assert(!okNeg);
    std::cout << "  -> OK: Precio negativo rechazado: '" << err << "'" << std::endl;

    // 4. Crear tarifa 2 (Frágil)
    Tarifa tOut2;
    bool ok2 = Tarifas::crearTarifa(FILE_TARIFAS_TEST_4C, "Frágil", 18.0, 25.0, 15.0, tOut2, err);
    assert(ok2);
    assert(tOut2.id == 2);

    // 5. Buscar por ID
    Tarifa tBuscada;
    bool okBus = Tarifas::obtenerTarifaPorId(FILE_TARIFAS_TEST_4C, 1, tBuscada);
    assert(okBus);
    assert(tBuscada.precioPorM3 == 15.0);

    // 6. Actualizar tarifa
    Tarifa tAct;
    bool okActualizada = Tarifas::actualizarTarifa(FILE_TARIFAS_TEST_4C, 1, "Estándar", 11.5, 16.0, 2.0, tAct, err);
    assert(okActualizada);
    assert(tAct.precioPorKg == 11.5);
    assert(tAct.recargoEspecial == 2.0);

    // 7. Baja lógica tarifa 2
    bool okElim = Tarifas::eliminarTarifa(FILE_TARIFAS_TEST_4C, 2, err);
    assert(okElim);

    std::vector<Tarifa> listaActivas = Tarifas::listarTarifas(FILE_TARIFAS_TEST_4C);
    assert(listaActivas.size() == 1);
    assert(listaActivas[0].id == 1);

    std::cout << "  -> OK: CRUD de Tarifas funcionando correctamente." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 4C (PILOTOS Y TARIFAS) ===" << std::endl;
    probarCRUDPilotos();
    probarCRUDTarifas();
    limpiarArchivosTest();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 4C COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
