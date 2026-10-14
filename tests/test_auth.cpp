#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "auth.h"

const char* FILE_USUARIOS_AUTH_TEST = "data/test_auth_usuarios.dat";

void limpiarArchivoTest() {
    std::remove(FILE_USUARIOS_AUTH_TEST);
}

void probarRegistroYBusqueda() {
    std::cout << "[PRUEBA 1] Registro de usuario y validaciones..." << std::endl;
    limpiarArchivoTest();

    Usuario uOut;
    std::string err;

    // 1. Registro exitoso de operador
    bool ok = Auth::registrarUsuario(FILE_USUARIOS_AUTH_TEST, "op_juan", "pass123", "Juan Pérez", "Operador", uOut, err);
    assert(ok);
    assert(uOut.id == 1);
    assert(std::strcmp(uOut.username, "op_juan") == 0);
    assert(std::strcmp(uOut.rol, "Operador") == 0);

    // 2. Intento de registro con username duplicado
    Usuario uOutDup;
    bool okDup = Auth::registrarUsuario(FILE_USUARIOS_AUTH_TEST, "op_juan", "OTRA_PASS", "Juan Clon", "Operador", uOutDup, err);
    assert(!okDup);
    assert(!err.empty());
    std::cout << "  -> OK: Duplicado rechazado con mensaje: '" << err << "'" << std::endl;

    // 3. Intento con rol inválido
    Usuario uOutRol;
    bool okRol = Auth::registrarUsuario(FILE_USUARIOS_AUTH_TEST, "user_invalid", "pass", "Nombre", "SuperAdmin", uOutRol, err);
    assert(!okRol);
    std::cout << "  -> OK: Rol inválido rechazado con mensaje: '" << err << "'" << std::endl;

    // 4. Registro exitoso de auditor
    Usuario uAuditor;
    bool okAud = Auth::registrarUsuario(FILE_USUARIOS_AUTH_TEST, "auditor1", "auditorPass", "Maria Lopez", "Auditor", uAuditor, err);
    assert(okAud);
    assert(uAuditor.id == 2);
}

void probarLoginYAutenticacion() {
    std::cout << "[PRUEBA 2] Pruebas de Login y Autenticación..." << std::endl;

    Usuario uLogin;
    std::string err;

    // 1. Login exitoso
    bool ok1 = Auth::autenticarUsuario(FILE_USUARIOS_AUTH_TEST, "op_juan", "pass123", uLogin, err);
    assert(ok1);
    assert(uLogin.id == 1);
    assert(std::strcmp(uLogin.nombre, "Juan Pérez") == 0);

    // 2. Login con contraseña incorrecta
    bool ok2 = Auth::autenticarUsuario(FILE_USUARIOS_AUTH_TEST, "op_juan", "PASS_ERRONEA", uLogin, err);
    assert(!ok2);
    assert(err.find("Credenciales inválidas") != std::string::npos);

    // 3. Login con usuario inexistente
    bool ok3 = Auth::autenticarUsuario(FILE_USUARIOS_AUTH_TEST, "no_existo", "pass", uLogin, err);
    assert(!ok3);
    assert(err.find("no encontrado") != std::string::npos);

    std::cout << "  -> OK: Autenticación exitosa y rechazos comprobados." << std::endl;
}

void probarUsuarioInactivo() {
    std::cout << "[PRUEBA 3] Login con usuario inactivo..." << std::endl;

    // Registrar un usuario
    Usuario uTest;
    std::string err;
    Auth::registrarUsuario(FILE_USUARIOS_AUTH_TEST, "user_inactivo", "pass1", "Usuario Inactivo", "Operador", uTest, err);

    // Desactivar el usuario mediante baja lógica
    bool elim = Persistence::eliminarLogico<Usuario>(FILE_USUARIOS_AUTH_TEST, uTest.id);
    assert(elim);

    // Intentar login con el usuario desactivado
    Usuario uInact;
    bool okLogin = Auth::autenticarUsuario(FILE_USUARIOS_AUTH_TEST, "user_inactivo", "pass1", uInact, err);
    assert(!okLogin);
    assert(err.find("inactivo") != std::string::npos);

    std::cout << "  -> OK: Login de usuario inactivo rechazado correctamente." << std::endl;
}

void probarPersistenciaEnDisco() {
    std::cout << "[PRUEBA 4] Persistencia de usuarios tras simular reinicio..." << std::endl;

    std::vector<Usuario> leidos = Persistence::leerTodos<Usuario>(FILE_USUARIOS_AUTH_TEST, true);
    assert(leidos.size() >= 2);

    std::cout << "  -> OK: " << leidos.size() << " usuarios leídos desde el archivo .dat." << std::endl;
}

void probarActualizarYEliminarUsuario() {
    std::cout << "[PRUEBA 5] Pruebas de Listar, Buscar por ID, Actualizar y Eliminar..." << std::endl;

    std::string err;
    // 1. Listar usuarios activos
    std::vector<Usuario> lista = Auth::listarUsuarios(FILE_USUARIOS_AUTH_TEST);
    assert(lista.size() >= 2);

    // 2. Buscar por ID existente
    Usuario uFind;
    bool okFind = Auth::obtenerUsuarioPorId(FILE_USUARIOS_AUTH_TEST, 1, uFind);
    assert(okFind);
    assert(uFind.id == 1);
    assert(std::strcmp(uFind.username, "op_juan") == 0);

    // 3. Buscar por ID inexistente
    Usuario uNo;
    bool okNo = Auth::obtenerUsuarioPorId(FILE_USUARIOS_AUTH_TEST, 999, uNo);
    assert(!okNo);

    // 4. Actualizar usuario (cambiar nombre y rol)
    Usuario uAct;
    bool okAct = Auth::actualizarUsuario(FILE_USUARIOS_AUTH_TEST, 1, "op_juan", "", "Juan Pérez Actualizado", "Auditor", uAct, err);
    assert(okAct);
    assert(std::strcmp(uAct.nombre, "Juan Pérez Actualizado") == 0);
    assert(std::strcmp(uAct.rol, "Auditor") == 0);

    // 5. Intento de actualizar con username duplicado de otro usuario (id 2 tiene 'auditor1')
    Usuario uDup;
    bool okDup = Auth::actualizarUsuario(FILE_USUARIOS_AUTH_TEST, 1, "auditor1", "", "Juan", "Auditor", uDup, err);
    assert(!okDup);
    assert(err.find("ya existe") != std::string::npos);

    // 6. Registrar usuario admin y verificar que no se puede eliminar
    Usuario uAdmin;
    Auth::registrarUsuario(FILE_USUARIOS_AUTH_TEST, "admin", "adminPass", "Admin Principal", "Administrador", uAdmin, err);
    bool okDelAdmin = Auth::eliminarUsuario(FILE_USUARIOS_AUTH_TEST, uAdmin.id, 2, err);
    assert(!okDelAdmin);
    assert(err.find("administrador principal") != std::string::npos);

    // 7. Auto-eliminación no permitida
    bool okSelfDel = Auth::eliminarUsuario(FILE_USUARIOS_AUTH_TEST, 2, 2, err);
    assert(!okSelfDel);
    assert(err.find("propia cuenta") != std::string::npos);

    // 8. Eliminación exitosa de usuario ID 2 por sesión ID 1
    bool okDel2 = Auth::eliminarUsuario(FILE_USUARIOS_AUTH_TEST, 2, 1, err);
    assert(okDel2);

    // Verificar que ya no aparece en listarUsuarios
    Usuario uInact;
    assert(!Auth::obtenerUsuarioPorId(FILE_USUARIOS_AUTH_TEST, 2, uInact));

    std::cout << "  -> OK: Listado, búsqueda, actualización y eliminación lógica verificados correctamente." << std::endl;
}

int main() {
    std::cout << "=== PRUEBAS DEL MÓDULO DE AUTENTICACIÓN (FASE 3) ===" << std::endl;
    probarRegistroYBusqueda();
    probarLoginYAutenticacion();
    probarUsuarioInactivo();
    probarPersistenciaEnDisco();
    probarActualizarYEliminarUsuario();
    limpiarArchivoTest();
    std::cout << "\n¡TODAS LAS PRUEBAS DE AUTENTICACIÓN COMPLETADAS EXITOSAMENTE!" << std::endl;
    return 0;
}
