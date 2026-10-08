#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "sucursales.h"
#include "rutas.h"
#include "vehiculos.h"

const char* FILE_SUCURSALES_TEST_4B = "data/test_fase4b_sucursales.dat";
const char* FILE_RUTAS_TEST_4B = "data/test_fase4b_rutas.dat";
const char* FILE_VEHICULOS_TEST_4B = "data/test_fase4b_vehiculos.dat";

void limpiarArchivosTest() {
    std::remove(FILE_SUCURSALES_TEST_4B);
    std::remove(FILE_RUTAS_TEST_4B);
    std::remove(FILE_VEHICULOS_TEST_4B);
}

void probarCRUDRutas() {
    std::cout << "[PRUEBA RUTAS] Creación, validación de sucursales, búsqueda, actualización y baja lógica..." << std::endl;
    limpiarArchivosTest();

    std::string err;

    // Crear 2 sucursales base requeridas para la ruta
    Sucursal s1, s2;
    bool okS1 = Sucursales::crearSucursal(FILE_SUCURSALES_TEST_4B, "Sucursal Capital", "Zona 1", "2222-1111", s1, err);
    bool okS2 = Sucursales::crearSucursal(FILE_SUCURSALES_TEST_4B, "Sucursal Quetzaltenango", "Zona 3 Xela", "7777-2222", s2, err);
    assert(okS1 && okS2);

    // 1. Intento de crear ruta con sucursal origen inexistente (ID 999)
    Ruta rInvalida;
    bool okInvalida = Rutas::crearRuta(FILE_RUTAS_TEST_4B, FILE_SUCURSALES_TEST_4B, 999, s2.id, 200.0, 50.0, rInvalida, err);
    assert(!okInvalida);
    std::cout << "  -> OK: Origen inexistente rechazado: '" << err << "'" << std::endl;

    // 2. Intento de crear ruta con origen == destino
    bool okMisima = Rutas::crearRuta(FILE_RUTAS_TEST_4B, FILE_SUCURSALES_TEST_4B, s1.id, s1.id, 10.0, 15.0, rInvalida, err);
    assert(!okMisima);
    std::cout << "  -> OK: Origen == Destino rechazado: '" << err << "'" << std::endl;

    // 3. Crear ruta válida
    Ruta rValida;
    bool okCrear = Rutas::crearRuta(FILE_RUTAS_TEST_4B, FILE_SUCURSALES_TEST_4B, s1.id, s2.id, 205.5, 75.0, rValida, err);
    assert(okCrear);
    assert(rValida.id == 1);
    assert(rValida.idSucursalOrigen == s1.id);
    assert(rValida.idSucursalDestino == s2.id);
    assert(rValida.distanciaKm == 205.5);

    // 4. Intento de ruta duplicada (mismo origen y destino)
    Ruta rDup;
    bool okDup = Rutas::crearRuta(FILE_RUTAS_TEST_4B, FILE_SUCURSALES_TEST_4B, s1.id, s2.id, 200.0, 70.0, rDup, err);
    assert(!okDup);
    std::cout << "  -> OK: Ruta duplicada rechazada: '" << err << "'" << std::endl;

    // 5. Consultar por ID
    Ruta rBuscada;
    bool okBusqueda = Rutas::obtenerRutaPorId(FILE_RUTAS_TEST_4B, 1, rBuscada);
    assert(okBusqueda);
    assert(rBuscada.tarifaBase == 75.0);

    // 6. Actualizar ruta
    Ruta rActualizada;
    bool okAct = Rutas::actualizarRuta(FILE_RUTAS_TEST_4B, FILE_SUCURSALES_TEST_4B, 1, s1.id, s2.id, 210.0, 80.0, rActualizada, err);
    assert(okAct);
    assert(rActualizada.distanciaKm == 210.0);
    assert(rActualizada.tarifaBase == 80.0);

    // 7. Baja lógica
    bool okDel = Rutas::eliminarRuta(FILE_RUTAS_TEST_4B, 1, err);
    assert(okDel);

    std::vector<Ruta> listaActivas = Rutas::listarRutas(FILE_RUTAS_TEST_4B);
    assert(listaActivas.empty());

    std::cout << "  -> OK: CRUD de Rutas funcionando correctamente." << std::endl;
}

void probarCRUDVehiculos() {
    std::cout << "[PRUEBA VEHÍCULOS] Creación, validación de placas, capacidades, actualización y baja..." << std::endl;

    Vehiculo vOut;
    std::string err;

    // 1. Crear vehículo 1
    bool ok1 = Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_4B, "C-123ABC", "Isuzu NPR", 3500.0, 18.5, "Disponible", vOut, err);
    assert(ok1);
    assert(vOut.id == 1);
    assert(std::strcmp(vOut.placa, "C-123ABC") == 0);
    assert(vOut.capacidadPesoKg == 3500.0);

    // 2. Intento de placa duplicada
    Vehiculo vDup;
    bool okDup = Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_4B, "C-123ABC", "Hino 300", 4000.0, 20.0, "Disponible", vDup, err);
    assert(!okDup);
    std::cout << "  -> OK: Placa duplicada rechazada: '" << err << "'" << std::endl;

    // 3. Intento de estado inválido
    Vehiculo vEst;
    bool okEst = Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_4B, "C-999ZZZ", "Ford F-350", 2500.0, 12.0, "Volando", vEst, err);
    assert(!okEst);
    std::cout << "  -> OK: Estado inválido rechazado: '" << err << "'" << std::endl;

    // 4. Crear vehículo 2
    Vehiculo vOut2;
    bool ok2 = Vehiculos::crearVehiculo(FILE_VEHICULOS_TEST_4B, "C-456DEF", "Freightliner M2", 8000.0, 35.0, "Disponible", vOut2, err);
    assert(ok2);
    assert(vOut2.id == 2);

    // 5. Buscar por ID
    Vehiculo vBuscado;
    bool okBus = Vehiculos::obtenerVehiculoPorId(FILE_VEHICULOS_TEST_4B, 1, vBuscado);
    assert(okBus);
    assert(std::strcmp(vBuscado.marcaModelo, "Isuzu NPR") == 0);

    // 6. Actualizar vehículo (cambio de estado a "En ruta")
    Vehiculo vAct;
    bool okActualizado = Vehiculos::actualizarVehiculo(FILE_VEHICULOS_TEST_4B, 1, "C-123ABC", "Isuzu NPR 2024", 3800.0, 19.0, "En ruta", vAct, err);
    assert(okActualizado);
    assert(std::strcmp(vAct.estado, "En ruta") == 0);

    // 7. Baja lógica vehículo 2
    bool okElim = Vehiculos::eliminarVehiculo(FILE_VEHICULOS_TEST_4B, 2, err);
    assert(okElim);

    std::vector<Vehiculo> listaActivos = Vehiculos::listarVehiculos(FILE_VEHICULOS_TEST_4B);
    assert(listaActivos.size() == 1);
    assert(listaActivos[0].id == 1);

    std::cout << "  -> OK: CRUD de Vehículos funcionando correctamente." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 4B (RUTAS Y VEHÍCULOS) ===" << std::endl;
    probarCRUDRutas();
    probarCRUDVehiculos();
    limpiarArchivosTest();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 4B COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
