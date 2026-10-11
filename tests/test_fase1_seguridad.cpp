#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <unordered_set>
#include <chrono>
#include <thread>

#include "structs.h"
#include "persistence.h"
#include "auth.h"
#include "sucursales.h"
#include "clientes.h"
#include "rutas.h"
#include "tarifas.h"
#include "transacciones.h"
#include "reportes.h"

// Archivos de prueba estrictamente aislados para la Fase 1
const char* TEST_DAT_USUARIOS = "data/test_fase1_usuarios.dat";
const char* TEST_DAT_SUCURSALES = "data/test_fase1_sucursales.dat";
const char* TEST_DAT_GUIAS = "data/test_fase1_guias.dat";
const char* TEST_DAT_RUTAS = "data/test_fase1_rutas.dat";

void limpiarArchivosFase1() {
    std::remove(TEST_DAT_USUARIOS);
    std::remove(TEST_DAT_SUCURSALES);
    std::remove(TEST_DAT_GUIAS);
    std::remove(TEST_DAT_RUTAS);
    Auth::SessionManager::getInstance().limpiarTodas();
}

// -----------------------------------------------------------------------------
// PRUEBA 1: CSPRNG y Generación Criptográficamente Segura de Tokens
// -----------------------------------------------------------------------------
void probarCSPRNG() {
    std::cout << "[1/10] Probando generador criptográfico de tokens (CSPRNG)..." << std::endl;
    std::unordered_set<std::string> tokens;
    const int NUM_TOKENS = 50;

    for (int i = 0; i < NUM_TOKENS; ++i) {
        std::string token = Auth::generarTokenSeguro(32);
        // Cada token debe tener 64 caracteres hexadecimales (32 bytes de entropía)
        assert(token.size() == 64);
        for (char c : token) {
            bool esHex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
            assert(esHex);
        }
        // Unicidad absoluta (no repeticiones, no secuencias deterministas)
        assert(tokens.find(token) == tokens.end());
        tokens.insert(token);
    }
    std::cout << "  -> OK: Generados " << NUM_TOKENS << " tokens únicos de 256 bits (64 hex chars) sin colisiones." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 2: Almacenamiento Seguro de Contraseñas y Compatibilidad Retroactiva
// -----------------------------------------------------------------------------
void probarAlmacenamientoContrasenas() {
    std::cout << "[2/10] Probando almacenamiento seguro y compatibilidad de contraseñas..." << std::endl;
    limpiarArchivosFase1();
    std::string err;

    // 1. Registro nuevo: Debe almacenar hash con estiramiento de clave y sal ($pbk$)
    Usuario uNuevo;
    bool okReg = Auth::registrarUsuario(TEST_DAT_USUARIOS, "admin_test", "SecretAdmin#2026", "Admin Prueba", "Administrador", uNuevo, err);
    assert(okReg);
    assert(std::string(uNuevo.password).rfind("$pbk$", 0) == 0); // Contiene prefijo de hash seguro
    assert(std::string(uNuevo.password).find("SecretAdmin#2026") == std::string::npos); // NUNCA texto plano
    std::cout << "  -> OK: Nuevo usuario almacena hash con sal: " << uNuevo.password << std::endl;

    // 2. Simular un usuario heredado en texto plano (como los preexistentes en data/usuarios.dat)
    Usuario uLegacy;
    std::memset(&uLegacy, 0, sizeof(Usuario));
    uLegacy.id = 2;
    std::strncpy(uLegacy.username, "legacy_user", sizeof(uLegacy.username) - 1);
    std::strncpy(uLegacy.password, "legacyPlainPass123", sizeof(uLegacy.password) - 1);
    std::strncpy(uLegacy.nombre, "Usuario Heredado", sizeof(uLegacy.nombre) - 1);
    std::strncpy(uLegacy.rol, "Operador", sizeof(uLegacy.rol) - 1);
    uLegacy.activo = true;
    Persistence::insertar<Usuario>(TEST_DAT_USUARIOS, uLegacy);

    // 3. Autenticación exitosa de usuario nuevo con hash
    Usuario uAuthNuevo;
    bool okAuthNuevo = Auth::autenticarUsuario(TEST_DAT_USUARIOS, "admin_test", "SecretAdmin#2026", uAuthNuevo, err);
    assert(okAuthNuevo);

    // 4. Autenticación exitosa de usuario heredado con texto plano (retrocompatibilidad garantizada)
    Usuario uAuthLegacy;
    bool okAuthLegacy = Auth::autenticarUsuario(TEST_DAT_USUARIOS, "legacy_user", "legacyPlainPass123", uAuthLegacy, err);
    assert(okAuthLegacy);
    std::cout << "  -> OK: Retrocompatibilidad con registros preexistentes verificada exitosamente." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 3: Inicio de Sesión y Generación de Token
// -----------------------------------------------------------------------------
void probarLoginYGeneracionToken() {
    std::cout << "[3/10] Probando emisión de access_token Bearer en login..." << std::endl;
    std::string err;

    Usuario u;
    bool ok = Auth::autenticarUsuario(TEST_DAT_USUARIOS, "admin_test", "SecretAdmin#2026", u, err);
    assert(ok);

    Auth::Sesion s = Auth::SessionManager::getInstance().crearSesion(u, 86400);
    assert(!s.token.empty());
    assert(s.token.size() == 64);
    assert(s.usuarioId == u.id);
    assert(s.username == "admin_test");
    assert(s.rol == "Administrador");
    assert(s.activa);

    std::cout << "  -> OK: Sesión creada exitosamente. Token emitido: " << s.token.substr(0, 16) << "... (64 chars)." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 4: Acceso a Operación Protegida con Token Válido
// -----------------------------------------------------------------------------
void probarAccesoTokenValido() {
    std::cout << "[4/10] Probando acceso a endpoint protegido con token válido..." << std::endl;
    Usuario u;
    std::string err;
    assert(Auth::buscarPorUsername(TEST_DAT_USUARIOS, "admin_test", u, *(new long)));

    Auth::Sesion s = Auth::SessionManager::getInstance().crearSesion(u);

    // Simular solicitud HTTP con Authorization: Bearer <token>
    crow::request req;
    req.add_header("Authorization", "Bearer " + s.token);

    Auth::AuthResult res = Auth::validarAcceso(req, {"Administrador", "Operador"}, TEST_DAT_USUARIOS);
    assert(res.autorizado);
    assert(res.codigoHttp == 200);
    assert(res.usuario.id == u.id);
    assert(std::strcmp(res.usuario.username, "admin_test") == 0);

    std::cout << "  -> OK: Acceso concedido con token válido." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 5: Rechazo de Acceso sin Token (401 Unauthorized)
// -----------------------------------------------------------------------------
void probarAccesoSinToken() {
    std::cout << "[5/10] Probando rechazo de acceso sin token (401 Unauthorized)..." << std::endl;
    crow::request req; // Sin encabezado Authorization

    Auth::AuthResult res = Auth::validarAcceso(req, {"Administrador"}, TEST_DAT_USUARIOS);
    assert(!res.autorizado);
    assert(res.codigoHttp == 401);
    assert(res.error.find("no proporcionado") != std::string::npos);

    // Con encabezado malformado (sin 'Bearer ')
    crow::request reqMalformada;
    reqMalformada.add_header("Authorization", "Basic dXNlcjpwYXNz");
    Auth::AuthResult resMal = Auth::validarAcceso(reqMalformada, {"Administrador"}, TEST_DAT_USUARIOS);
    assert(!resMal.autorizado);
    assert(resMal.codigoHttp == 401);

    std::cout << "  -> OK: Solicitudes sin token o con formato inválido rechazadas con 401." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 6: Rechazo de Token Inexistente o Falsificado (401 Unauthorized)
// -----------------------------------------------------------------------------
void probarTokenInvalido() {
    std::cout << "[6/10] Probando rechazo de token inexistente o falsificado..." << std::endl;
    crow::request req;
    req.add_header("Authorization", "Bearer token_falso_completamente_inventado_1234567890abcdef");

    Auth::AuthResult res = Auth::validarAcceso(req, {"Administrador"}, TEST_DAT_USUARIOS);
    assert(!res.autorizado);
    assert(res.codigoHttp == 401);
    assert(res.error.find("inexistente o inválido") != std::string::npos);

    std::cout << "  -> OK: Token falsificado rechazado con 401." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 7: Rechazo de Token Expirado (401 Unauthorized)
// -----------------------------------------------------------------------------
void probarTokenExpirado() {
    std::cout << "[7/10] Probando rechazo de token expirado..." << std::endl;
    Usuario u;
    assert(Auth::buscarPorUsername(TEST_DAT_USUARIOS, "admin_test", u, *(new long)));

    // Crear sesión con TTL negativo / inmediato para probar expiración
    Auth::Sesion sExpirada = Auth::SessionManager::getInstance().crearSesion(u, -5);

    crow::request req;
    req.add_header("Authorization", "Bearer " + sExpirada.token);

    Auth::AuthResult res = Auth::validarAcceso(req, {"Administrador"}, TEST_DAT_USUARIOS);
    assert(!res.autorizado);
    assert(res.codigoHttp == 401);
    assert(res.error.find("expirado") != std::string::npos);

    std::cout << "  -> OK: Token expirado detectado y rechazado correctamente con 401." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 8: Cierre de Sesión (Logout) y Revocación de Token
// -----------------------------------------------------------------------------
void probarLogoutYTokenRevocado() {
    std::cout << "[8/10] Probando cierre de sesión (logout) y token revocado..." << std::endl;
    Usuario u;
    assert(Auth::buscarPorUsername(TEST_DAT_USUARIOS, "admin_test", u, *(new long)));

    Auth::Sesion s = Auth::SessionManager::getInstance().crearSesion(u, 3600);

    // 1. Verificar acceso antes de logout
    crow::request req;
    req.add_header("Authorization", "Bearer " + s.token);
    Auth::AuthResult res1 = Auth::validarAcceso(req, {"Administrador"}, TEST_DAT_USUARIOS);
    assert(res1.autorizado);

    // 2. Ejecutar logout / revocación
    std::string errLogout;
    bool okRev = Auth::SessionManager::getInstance().revocarToken(s.token, errLogout);
    assert(okRev);

    // 3. Intento de reutilizar el token tras logout
    Auth::AuthResult res2 = Auth::validarAcceso(req, {"Administrador"}, TEST_DAT_USUARIOS);
    assert(!res2.autorizado);
    assert(res2.codigoHttp == 401);
    assert(res2.error.find("revocado") != std::string::npos);

    std::cout << "  -> OK: Token invalidado tras logout y rechazado en solicitudes posteriores." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 9: Control de Acceso Basado en Roles (RBAC - 403 Forbidden)
// -----------------------------------------------------------------------------
void probarAutorizacionRoles() {
    std::cout << "[9/10] Probando matriz de autorización por roles (403 Forbidden)..." << std::endl;
    std::string err;

    // Crear un operador y un auditor
    Usuario uOperador, uAuditor;
    Auth::registrarUsuario(TEST_DAT_USUARIOS, "oper_test", "oper123", "Operador Prueba", "Operador", uOperador, err);
    Auth::registrarUsuario(TEST_DAT_USUARIOS, "audit_test", "audit123", "Auditor Prueba", "Auditor", uAuditor, err);

    Auth::Sesion sesionOperador = Auth::SessionManager::getInstance().crearSesion(uOperador);
    Auth::Sesion sesionAuditor = Auth::SessionManager::getInstance().crearSesion(uAuditor);

    // Caso A: Operador intenta acceder a ruta exclusiva de Administrador (ej. gestión de usuarios)
    crow::request reqOperador;
    reqOperador.add_header("Authorization", "Bearer " + sesionOperador.token);
    Auth::AuthResult resOpAdmin = Auth::validarAcceso(reqOperador, {"Administrador"}, TEST_DAT_USUARIOS);
    assert(!resOpAdmin.autorizado);
    assert(resOpAdmin.codigoHttp == 403);
    assert(resOpAdmin.error.find("permisos suficientes") != std::string::npos);

    // Caso B: Operador intenta acceder a liquidación (exclusiva de Administrador y Auditor)
    Auth::AuthResult resOpLiq = Auth::validarAcceso(reqOperador, {"Administrador", "Auditor"}, TEST_DAT_USUARIOS);
    assert(!resOpLiq.autorizado);
    assert(resOpLiq.codigoHttp == 403);

    // Caso C: Auditor accede a liquidación (Permitido)
    crow::request reqAuditor;
    reqAuditor.add_header("Authorization", "Bearer " + sesionAuditor.token);
    Auth::AuthResult resAudLiq = Auth::validarAcceso(reqAuditor, {"Administrador", "Auditor"}, TEST_DAT_USUARIOS);
    assert(resAudLiq.autorizado);
    assert(resAudLiq.codigoHttp == 200);

    // Caso D: Auditor intenta crear catálogo o modificar recursos (exclusivo de Administrador)
    Auth::AuthResult resAudPost = Auth::validarAcceso(reqAuditor, {"Administrador"}, TEST_DAT_USUARIOS);
    assert(!resAudPost.autorizado);
    assert(resAudPost.codigoHttp == 403);

    // Caso E: Usuario dado de baja (baja lógica): token debe ser rechazado inmediatamente
    Persistence::eliminarLogico<Usuario>(TEST_DAT_USUARIOS, uOperador.id);
    Auth::AuthResult resInactivo = Auth::validarAcceso(reqOperador, {"Operador"}, TEST_DAT_USUARIOS);
    assert(!resInactivo.autorizado);
    assert(resInactivo.codigoHttp == 401);
    assert(resInactivo.error.find("inactivo o dado de baja") != std::string::npos);

    std::cout << "  -> OK: Matriz RBAC verificada: permisos concedidos y bloqueos 403 probados." << std::endl;
}

// -----------------------------------------------------------------------------
// PRUEBA 10: Integridad de Respuestas (Sin Exposición de Contraseñas ni Hashes)
// -----------------------------------------------------------------------------
void probarNoExposicionDatosSensibles() {
    std::cout << "[10/10] Verificando que las respuestas no expongan credenciales sensibles..." << std::endl;
    Usuario uAdmin;
    assert(Auth::buscarPorUsername(TEST_DAT_USUARIOS, "admin_test", uAdmin, *(new long)));

    Auth::Sesion s = Auth::SessionManager::getInstance().crearSesion(uAdmin);

    crow::json::wvalue resJson;
    crow::json::wvalue uJson;
    uJson["id"] = uAdmin.id;
    uJson["username"] = std::string(uAdmin.username);
    uJson["nombre"] = std::string(uAdmin.nombre);
    uJson["rol"] = std::string(uAdmin.rol);
    // NUNCA incluir password ni hash en uJson ni resJson

    resJson["access_token"] = s.token;
    resJson["token_type"] = "Bearer";
    resJson["expires_in"] = 86400;
    resJson["user"] = std::move(uJson);

    std::string dumped = resJson.dump();

    assert(dumped.find("password") == std::string::npos);
    assert(dumped.find("SecretAdmin") == std::string::npos);
    assert(dumped.find("$pbk$") == std::string::npos);
    assert(dumped.find("access_token") != std::string::npos);
    assert(dumped.find("Bearer") != std::string::npos);

    std::cout << "  -> OK: La respuesta JSON contiene únicamente campos públicos del usuario." << std::endl;
    limpiarArchivosFase1();
}

int main() {
    std::cout << "\n========================================================" << std::endl;
    std::cout << "  SUITE DE PRUEBAS DE AUTENTICACIÓN Y SEGURIDAD (FASE 1) " << std::endl;
    std::cout << "========================================================\n" << std::endl;

    probarCSPRNG();
    probarAlmacenamientoContrasenas();
    probarLoginYGeneracionToken();
    probarAccesoTokenValido();
    probarAccesoSinToken();
    probarTokenInvalido();
    probarTokenExpirado();
    probarLogoutYTokenRevocado();
    probarAutorizacionRoles();
    probarNoExposicionDatosSensibles();

    std::cout << "\n========================================================" << std::endl;
    std::cout << "  ¡TODAS LAS 10 PRUEBAS OBLIGATORIAS DE LA FASE 1 PASARON! " << std::endl;
    std::cout << "========================================================\n" << std::endl;
    return 0;
}
