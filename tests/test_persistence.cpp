#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"

// Rutas de prueba aisladas en directorio data/
const char* FILE_USUARIOS_TEST = "data/test_usuarios.dat";
const char* FILE_SUCURSALES_TEST = "data/test_sucursales.dat";
const char* FILE_CLIENTES_TEST = "data/test_clientes.dat";
const char* FILE_GUIAS_TEST = "data/test_guias.dat";

void limpiarArchivosPrueba() {
    std::remove(FILE_USUARIOS_TEST);
    std::remove(FILE_SUCURSALES_TEST);
    std::remove(FILE_CLIENTES_TEST);
    std::remove(FILE_GUIAS_TEST);
}

void probarTamanosFijos() {
    std::cout << "[PRUEBA 1] Verificación de tamaños fijos de estructuras..." << std::endl;
    std::cout << "  sizeof(Usuario): " << sizeof(Usuario) << " bytes" << std::endl;
    std::cout << "  sizeof(Sucursal): " << sizeof(Sucursal) << " bytes" << std::endl;
    std::cout << "  sizeof(Cliente): " << sizeof(Cliente) << " bytes" << std::endl;
    std::cout << "  sizeof(Ruta): " << sizeof(Ruta) << " bytes" << std::endl;
    std::cout << "  sizeof(Vehiculo): " << sizeof(Vehiculo) << " bytes" << std::endl;
    std::cout << "  sizeof(Piloto): " << sizeof(Piloto) << " bytes" << std::endl;
    std::cout << "  sizeof(Tarifa): " << sizeof(Tarifa) << " bytes" << std::endl;
    std::cout << "  sizeof(Guia): " << sizeof(Guia) << " bytes" << std::endl;
    assert(sizeof(Usuario) > 0);
    assert(sizeof(Sucursal) > 0);
    assert(sizeof(Cliente) > 0);
    assert(sizeof(Guia) > 0);
    std::cout << "  -> OK: Tamaños constantes comprobados." << std::endl;
}

void probarEscrituraYLectura() {
    std::cout << "[PRUEBA 2] Escritura y Lectura de registros..." << std::endl;
    limpiarArchivosPrueba();

    // 1. Inserción
    Usuario u1;
    u1.id = 1;
    std::strncpy(u1.username, "admin", sizeof(u1.username));
    std::strncpy(u1.password, "123456", sizeof(u1.password));
    std::strncpy(u1.nombre, "Administrador Sistema", sizeof(u1.nombre));
    std::strncpy(u1.rol, "Administrador", sizeof(u1.rol));
    u1.activo = true;

    Usuario u2;
    u2.id = 2;
    std::strncpy(u2.username, "operador1", sizeof(u2.username));
    std::strncpy(u2.password, "op123", sizeof(u2.password));
    std::strncpy(u2.nombre, "Juan Pérez", sizeof(u2.nombre));
    std::strncpy(u2.rol, "Operador", sizeof(u2.rol));
    u2.activo = true;

    bool ok1 = Persistence::insertar(FILE_USUARIOS_TEST, u1);
    bool ok2 = Persistence::insertar(FILE_USUARIOS_TEST, u2);
    assert(ok1 && ok2);

    // 2. Lectura total
    std::vector<Usuario> lista = Persistence::leerTodos<Usuario>(FILE_USUARIOS_TEST, true);
    assert(lista.size() == 2);
    assert(std::strcmp(lista[0].username, "admin") == 0);
    assert(std::strcmp(lista[1].username, "operador1") == 0);

    std::cout << "  -> OK: 2 usuarios insertados y leídos correctamente." << std::endl;
}

void probarBusquedaActualizacionYEliminacion() {
    std::cout << "[PRUEBA 3] Búsqueda, Actualización y Baja Lógica..." << std::endl;

    // 1. Búsqueda por ID existente
    Sucursal s;
    long index = -1;
    Sucursal nuevaS;
    nuevaS.id = 10;
    std::strncpy(nuevaS.nombre, "Sucursal Central", sizeof(nuevaS.nombre));
    std::strncpy(nuevaS.direccion, "Zona 1, Ciudad de Guatemala", sizeof(nuevaS.direccion));
    std::strncpy(nuevaS.telefono, "2222-3333", sizeof(nuevaS.telefono));
    nuevaS.activo = true;

    Persistence::insertar(FILE_SUCURSALES_TEST, nuevaS);

    bool encontrado = Persistence::buscarPorId<Sucursal>(FILE_SUCURSALES_TEST, 10, s, index);
    assert(encontrado);
    assert(index == 0);
    assert(std::strcmp(s.nombre, "Sucursal Central") == 0);

    // 2. Búsqueda por ID inexistente
    Sucursal sInexistente;
    long indexInexistente = -1;
    bool noEncontrado = Persistence::buscarPorId<Sucursal>(FILE_SUCURSALES_TEST, 999, sInexistente, indexInexistente);
    assert(!noEncontrado);

    // 3. Actualización por ID
    std::strncpy(nuevaS.nombre, "Sucursal Central Modificada", sizeof(nuevaS.nombre));
    bool actualizado = Persistence::actualizarPorId<Sucursal>(FILE_SUCURSALES_TEST, 10, nuevaS);
    assert(actualizado);

    Sucursal sMod;
    Persistence::buscarPorId<Sucursal>(FILE_SUCURSALES_TEST, 10, sMod, index);
    assert(std::strcmp(sMod.nombre, "Sucursal Central Modificada") == 0);

    // 4. Baja Lógica
    bool eliminado = Persistence::eliminarLogico<Sucursal>(FILE_SUCURSALES_TEST, 10);
    assert(eliminado);

    // Verificar que las consultas normales (soloActivos=true) ya no retornen la sucursal inactiva
    std::vector<Sucursal> activas = Persistence::leerTodos<Sucursal>(FILE_SUCURSALES_TEST, true);
    assert(activas.size() == 0);

    // Verificar que leyendo todos (soloActivos=false) sí esté guardada con activo = false
    std::vector<Sucursal> todas = Persistence::leerTodos<Sucursal>(FILE_SUCURSALES_TEST, false);
    assert(todas.size() == 1);
    assert(todas[0].activo == false);

    std::cout << "  -> OK: Búsqueda, actualización y baja lógica funcionando perfectamente." << std::endl;
}

void probarPersistenciaTrasReapertura() {
    std::cout << "[PRUEBA 4] Persistencia de datos tras cerrar y reabrir archivo..." << std::endl;

    Cliente c;
    c.id = 101;
    std::strncpy(c.nit, "1234567-8", sizeof(c.nit));
    std::strncpy(c.nombre, "Corporación Ejemplo S.A.", sizeof(c.nombre));
    std::strncpy(c.direccion, "Calzada Roosevelt", sizeof(c.direccion));
    std::strncpy(c.telefono, "2444-5555", sizeof(c.telefono));
    c.activo = true;

    Persistence::insertar(FILE_CLIENTES_TEST, c);

    // Reabrir y leer desde disco
    std::vector<Cliente> leidos = Persistence::leerTodos<Cliente>(FILE_CLIENTES_TEST, true);
    assert(leidos.size() == 1);
    assert(leidos[0].id == 101);
    assert(std::strcmp(leidos[0].nombre, "Corporación Ejemplo S.A.") == 0);

    std::cout << "  -> OK: Datos persistidos correctamente en disco." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DE PERSISTENCIA BINARIA (FASE 2) ===" << std::endl;
    probarTamanosFijos();
    probarEscrituraYLectura();
    probarBusquedaActualizacionYEliminacion();
    probarPersistenciaTrasReapertura();
    limpiarArchivosPrueba();
    std::cout << "\n¡TODAS LAS PRUEBAS DE PERSISTENCIA COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
