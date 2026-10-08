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
#include "transacciones.h"

const char* FILE_SUCURSALES_TEST_5A = "data/test_fase5a_sucursales.dat";
const char* FILE_CLIENTES_TEST_5A = "data/test_fase5a_clientes.dat";
const char* FILE_RUTAS_TEST_5A = "data/test_fase5a_rutas.dat";
const char* FILE_GUIAS_TEST_5A = "data/test_fase5a_guias.dat";

void limpiarArchivosTest() {
    std::remove(FILE_SUCURSALES_TEST_5A);
    std::remove(FILE_CLIENTES_TEST_5A);
    std::remove(FILE_RUTAS_TEST_5A);
    std::remove(FILE_GUIAS_TEST_5A);
}

void probarEmisionYGestionGuias() {
    std::cout << "[PRUEBA GUIAS] Creación, validaciones relacionales, búsqueda y persistencia..." << std::endl;
    limpiarArchivosTest();

    std::string err;

    // 1. Crear 2 sucursales, 2 clientes y 1 ruta base
    Sucursal s1, s2;
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_5A, "Sucursal Origen", "Zona 1", "2222-1111", s1, err);
    Sucursales::crearSucursal(FILE_SUCURSALES_TEST_5A, "Sucursal Destino", "Zona 10", "2222-2222", s2, err);

    Cliente cRem, cDest;
    Clientes::crearCliente(FILE_CLIENTES_TEST_5A, "111111-1", "Remitente S.A.", "Zona 1", "2444-1111", cRem, err);
    Clientes::crearCliente(FILE_CLIENTES_TEST_5A, "222222-2", "Destinatario Ltda", "Zona 10", "2444-2222", cDest, err);

    Ruta r;
    Rutas::crearRuta(FILE_RUTAS_TEST_5A, FILE_SUCURSALES_TEST_5A, s1.id, s2.id, 15.0, 30.0, r, err);

    // 2. Intento de crear guía con remitente inexistente (ID 999)
    Guia gInvalida;
    bool okRemInvalido = Guias::crearGuia(FILE_GUIAS_TEST_5A, FILE_CLIENTES_TEST_5A, FILE_RUTAS_TEST_5A, 999, cDest.id, r.id, 5.0, 10.0, 10.0, 10.0, gInvalida, err);
    assert(!okRemInvalido);
    std::cout << "  -> OK: Remitente inexistente rechazado: '" << err << "'" << std::endl;

    // 3. Intento de crear guía con ruta inexistente (ID 888)
    bool okRutaInvalida = Guias::crearGuia(FILE_GUIAS_TEST_5A, FILE_CLIENTES_TEST_5A, FILE_RUTAS_TEST_5A, cRem.id, cDest.id, 888, 5.0, 10.0, 10.0, 10.0, gInvalida, err);
    assert(!okRutaInvalida);
    std::cout << "  -> OK: Ruta inexistente rechazada: '" << err << "'" << std::endl;

    // 4. Intento de crear guía con remitente == destinatario
    bool okMismoCliente = Guias::crearGuia(FILE_GUIAS_TEST_5A, FILE_CLIENTES_TEST_5A, FILE_RUTAS_TEST_5A, cRem.id, cRem.id, r.id, 5.0, 10.0, 10.0, 10.0, gInvalida, err);
    assert(!okMismoCliente);
    std::cout << "  -> OK: Remitente == Destinatario rechazado: '" << err << "'" << std::endl;

    // 5. Crear guía válida
    Guia gValida;
    bool okCrear = Guias::crearGuia(FILE_GUIAS_TEST_5A, FILE_CLIENTES_TEST_5A, FILE_RUTAS_TEST_5A, cRem.id, cDest.id, r.id, 12.5, 40.0, 30.0, 20.0, gValida, err);
    assert(okCrear);
    assert(gValida.numeroGuia == 1001);
    assert(gValida.idClienteRemitente == cRem.id);
    assert(gValida.idClienteDestinatario == cDest.id);
    assert(gValida.idRuta == r.id);
    assert(gValida.pesoRealKg == 12.5);
    assert(std::strcmp(gValida.estado, "Creada") == 0);

    // 6. Consultar guía por número
    Guia gBuscada;
    bool okBusqueda = Guias::obtenerGuiaPorNumero(FILE_GUIAS_TEST_5A, 1001, gBuscada);
    assert(okBusqueda);
    assert(gBuscada.pesoRealKg == 12.5);

    // 7. Crear segunda guía y comprobar auto-incremento (1002)
    Guia gValida2;
    bool okCrear2 = Guias::crearGuia(FILE_GUIAS_TEST_5A, FILE_CLIENTES_TEST_5A, FILE_RUTAS_TEST_5A, cRem.id, cDest.id, r.id, 8.0, 25.0, 20.0, 15.0, gValida2, err);
    assert(okCrear2);
    assert(gValida2.numeroGuia == 1002);

    // 8. Actualizar estado a "Despachada"
    Guia gEstado;
    bool okActEstado = Guias::actualizarEstadoGuia(FILE_GUIAS_TEST_5A, 1001, "Despachada", gEstado, err);
    assert(okActEstado);
    assert(std::strcmp(gEstado.estado, "Despachada") == 0);

    // 9. Baja lógica de guía 1002
    bool okElim = Guias::eliminarGuia(FILE_GUIAS_TEST_5A, 1002, err);
    assert(okElim);

    std::vector<Guia> activas = Guias::listarGuias(FILE_GUIAS_TEST_5A);
    assert(activas.size() == 1);
    assert(activas[0].numeroGuia == 1001);

    std::cout << "  -> OK: Emisión y gestión de Guías funcionando correctamente." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE LA FASE 5A (EMISIÓN DE GUÍAS) ===" << std::endl;
    probarEmisionYGestionGuias();
    limpiarArchivosTest();
    std::cout << "\n¡TODAS LAS PRUEBAS DE LA FASE 5A COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
