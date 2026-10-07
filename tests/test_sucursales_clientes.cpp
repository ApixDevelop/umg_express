#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "sucursales.h"
#include "clientes.h"

const char* FILE_SUCURSALES_TEST_4A = "data/test_fase4a_sucursales.dat";
const char* FILE_CLIENTES_TEST_4A = "data/test_fase4a_clientes.dat";

void limpiarArchivosTest() {
    std::remove(FILE_SUCURSALES_TEST_4A);
    std::remove(FILE_CLIENTES_TEST_4A);
}

void probarCRUDSucursales() {
    std::cout << "[PRUEBA SUCURSALES] Creación, búsqueda, actualización y baja lógica..." << std::endl;
    limpiarArchivosTest();

    Sucursal sOut;
    std::string err;

    // 1. Crear sucursal 1
    bool ok1 = Sucursales::crearSucursal(FILE_SUCURSALES_TEST_4A, "Sucursal Central", "Zona 1, Ciudad", "2222-1111", sOut, err);
    assert(ok1);
    assert(sOut.id == 1);
    assert(std::strcmp(sOut.nombre, "Sucursal Central") == 0);

    // 2. Crear sucursal 2
    Sucursal sOut2;
    bool ok2 = Sucursales::crearSucursal(FILE_SUCURSALES_TEST_4A, "Sucursal Norte", "Zona 18, Ciudad", "2222-2222", sOut2, err);
    assert(ok2);
    assert(sOut2.id == 2);

    // 3. Intento de duplicado de nombre
    Sucursal sDup;
    bool okDup = Sucursales::crearSucursal(FILE_SUCURSALES_TEST_4A, "Sucursal Central", "Otra zona", "1111-1111", sDup, err);
    assert(!okDup);
    std::cout << "  -> OK: Duplicado de nombre de sucursal rechazado: '" << err << "'" << std::endl;

    // 4. Listar activas
    std::vector<Sucursal> lista = Sucursales::listarSucursales(FILE_SUCURSALES_TEST_4A);
    assert(lista.size() == 2);

    // 5. Buscar por ID
    Sucursal sBuscada;
    bool encontrada = Sucursales::obtenerSucursalPorId(FILE_SUCURSALES_TEST_4A, 1, sBuscada);
    assert(encontrada);
    assert(std::strcmp(sBuscada.telefono, "2222-1111") == 0);

    // 6. Actualizar sucursal
    Sucursal sActualizada;
    bool okAct = Sucursales::actualizarSucursal(FILE_SUCURSALES_TEST_4A, 1, "Sucursal Central Modificada", "Zona 10, Ciudad", "2333-4444", sActualizada, err);
    assert(okAct);
    assert(std::strcmp(sActualizada.nombre, "Sucursal Central Modificada") == 0);

    // 7. Baja lógica
    bool okDel = Sucursales::eliminarSucursal(FILE_SUCURSALES_TEST_4A, 1, err);
    assert(okDel);

    // Listar activas después de baja lógica
    std::vector<Sucursal> listaPost = Sucursales::listarSucursales(FILE_SUCURSALES_TEST_4A);
    assert(listaPost.size() == 1);
    assert(listaPost[0].id == 2);

    // Intentar buscar ID 1 inactivo
    Sucursal sInactiva;
    bool okInact = Sucursales::obtenerSucursalPorId(FILE_SUCURSALES_TEST_4A, 1, sInactiva);
    assert(!okInact);

    std::cout << "  -> OK: CRUD de Sucursales funcionando correctamente." << std::endl;
}

void probarCRUDClientes() {
    std::cout << "[PRUEBA CLIENTES] Creación, búsqueda, actualización y baja lógica..." << std::endl;

    Cliente cOut;
    std::string err;

    // 1. Crear cliente 1
    bool ok1 = Clientes::crearCliente(FILE_CLIENTES_TEST_4A, "123456-7", "Empresa Alfa", "Calzada Roosevelt", "2444-1111", cOut, err);
    assert(ok1);
    assert(cOut.id == 1);
    assert(std::strcmp(cOut.nit, "123456-7") == 0);

    // 2. Crear cliente 2
    Cliente cOut2;
    bool ok2 = Clientes::crearCliente(FILE_CLIENTES_TEST_4A, "888888-9", "Empresa Beta", "Avenida Hincapié", "2444-2222", cOut2, err);
    assert(ok2);
    assert(cOut2.id == 2);

    // 3. Intento de NIT duplicado
    Cliente cDup;
    bool okDup = Clientes::crearCliente(FILE_CLIENTES_TEST_4A, "123456-7", "Empresa Clon", "Otra direccion", "0000-0000", cDup, err);
    assert(!okDup);
    std::cout << "  -> OK: NIT duplicado rechazado: '" << err << "'" << std::endl;

    // 4. Buscar por ID inexistente
    Cliente cInexistente;
    bool okInex = Clientes::obtenerClientePorId(FILE_CLIENTES_TEST_4A, 999, cInexistente);
    assert(!okInex);

    // 5. Actualizar cliente
    Cliente cAct;
    bool okActualizado = Clientes::actualizarCliente(FILE_CLIENTES_TEST_4A, 1, "123456-7", "Empresa Alfa Modificada", "Roosevelt Km 15", "2444-9999", cAct, err);
    assert(okActualizado);
    assert(std::strcmp(cAct.nombre, "Empresa Alfa Modificada") == 0);

    // 6. Baja lógica cliente
    bool okElim = Clientes::eliminarCliente(FILE_CLIENTES_TEST_4A, 2, err);
    assert(okElim);

    std::vector<Cliente> listaActivos = Clientes::listarClientes(FILE_CLIENTES_TEST_4A);
    assert(listaActivos.size() == 1);
    assert(listaActivos[0].id == 1);

    std::cout << "  -> OK: CRUD de Clientes funcionando correctamente." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 4A (SUCURSALES Y CLIENTES) ===" << std::endl;
    probarCRUDSucursales();
    probarCRUDClientes();
    limpiarArchivosTest();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 4A COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
