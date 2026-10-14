#ifndef AUTH_H
#define AUTH_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <cstdlib>

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include <stdlib.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <random>
#endif

#include "crow.h"
#include "structs.h"
#include "persistence.h"

namespace Auth {

const char* const DEFAULT_FILE_USUARIOS = "data/usuarios.dat";
const int DEFAULT_SESSION_TTL_SECONDS = 86400; // 24 horas (86400 segundos)

// Roles soportados por el sistema
inline bool esRolValido(const std::string& rol) {
    return (rol == "Administrador" || rol == "Operador" || rol == "Auditor");
}

// =============================================================================
// Criptografía y Seguridad (SHA-256 FIPS 180-4 con Estiramiento de Clave / Sal)
// =============================================================================
namespace Crypto {

inline std::string sha256(const std::string& input) {
    uint32_t h0 = 0x6a09e667, h1 = 0xbb67ae85, h2 = 0x3c6ef372, h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f, h5 = 0x9b05688c, h6 = 0x1f83d9ab, h7 = 0x5be0cd19;

    static const uint32_t k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    uint64_t bitlen = input.size() * 8;
    std::vector<uint8_t> msg(input.begin(), input.end());
    msg.push_back(0x80);
    while ((msg.size() % 64) != 56) {
        msg.push_back(0x00);
    }
    for (int i = 7; i >= 0; --i) {
        msg.push_back((bitlen >> (i * 8)) & 0xff);
    }

    auto rotr = [](uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); };

    for (size_t chunk = 0; chunk < msg.size(); chunk += 64) {
        uint32_t w[64];
        for (int i = 0; i < 16; ++i) {
            w[i] = (msg[chunk + i * 4] << 24) |
                   (msg[chunk + i * 4 + 1] << 16) |
                   (msg[chunk + i * 4 + 2] << 8) |
                   (msg[chunk + i * 4 + 3]);
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4, f = h5, g = h6, h = h7;

        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + k[i] + w[i];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        h0 += a; h1 += b; h2 += c; h3 += d;
        h4 += e; h5 += f; h6 += g; h7 += h;
    }

    std::stringstream ss;
    uint32_t hs[8] = {h0, h1, h2, h3, h4, h5, h6, h7};
    for (int i = 0; i < 8; ++i) {
        ss << std::hex << std::setw(8) << std::setfill('0') << hs[i];
    }
    return ss.str();
}

// Estiramiento de clave con sal para contraseñas (1,000 iteraciones SHA-256)
inline std::string hashConSal(const std::string& password, const std::string& username) {
    std::string salt = "UMG_EXPRESS_SALT_" + username + "_SECURE_V1";
    std::string h = sha256(salt + password);
    for (int iter = 0; iter < 1000; ++iter) {
        h = sha256(h + salt);
    }
    // Prefijo $pbk$ (5 chars) + 55 chars hex = 60 chars. Cabe perfectamente en char password[64].
    return "$pbk$" + h.substr(0, 55);
}

} // namespace Crypto

inline std::string hashearPassword(const std::string& password, const std::string& username) {
    return Crypto::hashConSal(password, username);
}

// Verifica contraseña con compatibilidad dual:
// 1. Contraseñas protegidas mediante hash y sal ($pbk$).
// 2. Contraseñas heredadas en texto plano presentes en la base existente.
inline bool verificarPassword(const std::string& rawPassword, const std::string& storedPassword, const std::string& username) {
    if (rawPassword.empty() || storedPassword.empty()) return false;
    // Si coincide con el hash esperado con estiramiento de clave
    if (storedPassword == hashearPassword(rawPassword, username)) return true;
    // Retrocompatibilidad segura con usuarios existentes sin alterar sus datos reales
    if (storedPassword == rawPassword) return true;
    return false;
}

// Generador criptográficamente seguro de tokens usando entropía del sistema operativo.
inline std::string generarTokenSeguro(size_t numBytes = 32) {
    std::vector<unsigned char> buffer(numBytes, 0);
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    arc4random_buf(buffer.data(), numBytes);
#else
    FILE* f = fopen("/dev/urandom", "rb");
    if (f) {
        size_t leidos = fread(buffer.data(), 1, numBytes, f);
        fclose(f);
        if (leidos != numBytes) {
            std::random_device rd;
            for (size_t i = 0; i < numBytes; ++i) buffer[i] = static_cast<unsigned char>(rd());
        }
    } else {
        std::random_device rd;
        for (size_t i = 0; i < numBytes; ++i) buffer[i] = static_cast<unsigned char>(rd());
    }
#endif
    std::stringstream ss;
    for (unsigned char b : buffer) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    return ss.str();
}

// =============================================================================
// Estructura y Gestor de Sesiones en Memoria (In-Memory Session Store)
// =============================================================================
struct Sesion {
    std::string token;
    int usuarioId;
    std::string username;
    std::string nombre;
    std::string rol;
    std::chrono::system_clock::time_point creadoEn;
    std::chrono::system_clock::time_point expiraEn;
    bool activa;
};

class SessionManager {
public:
    static SessionManager& getInstance() {
        static SessionManager instance;
        return instance;
    }

    Sesion crearSesion(const Usuario& u, int ttlSegundos = DEFAULT_SESSION_TTL_SECONDS) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::system_clock::now();
        Sesion s;
        s.token = generarTokenSeguro(32);
        s.usuarioId = u.id;
        s.username = u.username;
        s.nombre = u.nombre;
        s.rol = u.rol;
        s.creadoEn = now;
        s.expiraEn = now + std::chrono::seconds(ttlSegundos);
        s.activa = true;

        sesiones_[s.token] = s;
        return s;
    }

    bool validarToken(const std::string& token, Usuario& outUsuario, std::string& errorMsg, const char* filepathUsuarios = DEFAULT_FILE_USUARIOS) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (token.empty()) {
            errorMsg = "Token de acceso no proporcionado";
            return false;
        }

        auto it = sesiones_.find(token);
        if (it == sesiones_.end()) {
            errorMsg = "Token de acceso inexistente o inválido";
            return false;
        }

        const Sesion& s = it->second;
        if (!s.activa) {
            errorMsg = "Token revocado (la sesión fue cerrada previamente)";
            return false;
        }

        auto now = std::chrono::system_clock::now();
        if (now >= s.expiraEn) {
            errorMsg = "Token de acceso expirado";
            return false;
        }

        // Verificar vigencia real del usuario en el archivo binario
        FILE* fUser = fopen(filepathUsuarios, "rb");
        if (!fUser) {
            errorMsg = "No se pudo verificar el archivo de usuarios";
            return false;
        }
        Usuario uPersistente;
        bool encontrado = false;
        while (fread(&uPersistente, sizeof(Usuario), 1, fUser) == 1) {
            if (uPersistente.id == s.usuarioId) {
                encontrado = true;
                break;
            }
        }
        fclose(fUser);

        if (!encontrado) {
            errorMsg = "Usuario asociado al token no encontrado en el sistema";
            return false;
        }

        if (!uPersistente.activo) {
            errorMsg = "El usuario se encuentra inactivo o dado de baja";
            return false;
        }

        outUsuario = uPersistente;
        return true;
    }

    bool revocarToken(const std::string& token, std::string& errorMsg) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sesiones_.find(token);
        if (it == sesiones_.end()) {
            errorMsg = "Token no encontrado";
            return false;
        }
        it->second.activa = false;
        return true;
    }

    void limpiarExpiradas() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::system_clock::now();
        for (auto it = sesiones_.begin(); it != sesiones_.end(); ) {
            if (!it->second.activa || now >= it->second.expiraEn) {
                it = sesiones_.erase(it);
            } else {
                ++it;
            }
        }
    }

    void limpiarTodas() {
        std::lock_guard<std::mutex> lock(mutex_);
        sesiones_.clear();
    }

    size_t totalSesiones() {
        std::lock_guard<std::mutex> lock(mutex_);
        return sesiones_.size();
    }

private:
    SessionManager() = default;
    std::unordered_map<std::string, Sesion> sesiones_;
    std::mutex mutex_;
};

// =============================================================================
// Persistencia y Operaciones de Usuarios
// =============================================================================

inline bool buscarPorUsername(const char* filepath, const std::string& username, Usuario& outUsuario, long& outIndex) {
    FILE* f = fopen(filepath, "rb");
    if (!f) return false;

    Usuario u;
    long index = 0;
    while (fread(&u, sizeof(Usuario), 1, f) == 1) {
        if (u.activo && username == u.username) {
            outUsuario = u;
            outIndex = index;
            fclose(f);
            return true;
        }
        index++;
    }
    fclose(f);
    return false;
}

// Obtiene la lista de todos los usuarios activos
inline std::vector<Usuario> listarUsuarios(const char* filepath = DEFAULT_FILE_USUARIOS) {
    return Persistence::leerTodos<Usuario>(filepath, true);
}

// Obtiene un usuario activo por su ID
inline bool obtenerUsuarioPorId(const char* filepath, int id, Usuario& outUsuario) {
    long index = -1;
    return Persistence::buscarPorId<Usuario>(filepath, id, outUsuario, index);
}

// Comprueba si un username ya existe (activo), opcionalmente excluyendo un ID específico
inline bool existeUsername(const char* filepath, const std::string& username, int idExcluir = -1) {
    FILE* f = fopen(filepath, "rb");
    if (!f) return false;

    Usuario u;
    while (fread(&u, sizeof(Usuario), 1, f) == 1) {
        if (u.activo && (idExcluir == -1 || u.id != idExcluir) && username == u.username) {
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

// Actualiza los datos de un usuario existente
inline bool actualizarUsuario(const char* filepath,
                              int id,
                              const std::string& username,
                              const std::string& password,
                              const std::string& nombre,
                              const std::string& rol,
                              Usuario& outUsuario,
                              std::string& errorMsg) {
    Usuario actual;
    if (!obtenerUsuarioPorId(filepath, id, actual)) {
        errorMsg = "Usuario con ID " + std::to_string(id) + " no encontrado o inactivo";
        return false;
    }

    if (username.empty() || nombre.empty() || rol.empty()) {
        errorMsg = "Los campos username, nombre y rol son obligatorios";
        return false;
    }
    if (username.size() >= 32) {
        errorMsg = "El nombre de usuario excede el límite de 31 caracteres";
        return false;
    }
    if (nombre.size() >= 64) {
        errorMsg = "El nombre excede el límite de 63 caracteres";
        return false;
    }
    if (!esRolValido(rol)) {
        errorMsg = "Rol no válido. Debe ser: Administrador, Operador o Auditor";
        return false;
    }
    if (existeUsername(filepath, username, id)) {
        errorMsg = "El nombre de usuario '" + username + "' ya existe en el sistema";
        return false;
    }

    if (std::string(actual.username) == "admin" && rol != "Administrador") {
        errorMsg = "El usuario administrador principal ('admin') no puede perder el rol Administrador";
        return false;
    }

    std::memset(actual.username, 0, sizeof(actual.username));
    std::memset(actual.nombre, 0, sizeof(actual.nombre));
    std::memset(actual.rol, 0, sizeof(actual.rol));

    std::strncpy(actual.username, username.c_str(), sizeof(actual.username) - 1);
    std::strncpy(actual.nombre, nombre.c_str(), sizeof(actual.nombre) - 1);
    std::strncpy(actual.rol, rol.c_str(), sizeof(actual.rol) - 1);

    if (!password.empty()) {
        if (password.size() >= 64) {
            errorMsg = "La contraseña excede el límite de 63 caracteres";
            return false;
        }
        std::string passHash = hashearPassword(password, username);
        std::memset(actual.password, 0, sizeof(actual.password));
        std::strncpy(actual.password, passHash.c_str(), sizeof(actual.password) - 1);
    }

    if (!Persistence::actualizarPorId<Usuario>(filepath, id, actual)) {
        errorMsg = "Error al actualizar el usuario en el archivo binario";
        return false;
    }

    outUsuario = actual;
    return true;
}

// Realiza la baja lógica (activo = false) de un usuario
inline bool eliminarUsuario(const char* filepath, int id, int idUsuarioSesion, std::string& errorMsg) {
    Usuario actual;
    if (!obtenerUsuarioPorId(filepath, id, actual)) {
        errorMsg = "Usuario con ID " + std::to_string(id) + " no encontrado o ya inactivo";
        return false;
    }

    if (std::string(actual.username) == "admin") {
        errorMsg = "No se puede eliminar al usuario administrador principal del sistema";
        return false;
    }
    if (id == idUsuarioSesion) {
        errorMsg = "No puede eliminar su propia cuenta de usuario en sesión";
        return false;
    }

    if (!Persistence::eliminarLogico<Usuario>(filepath, id)) {
        errorMsg = "Error al realizar la baja lógica del usuario en disco";
        return false;
    }

    return true;
}

inline bool registrarUsuario(const char* filepath,
                             const std::string& username,
                             const std::string& password,
                             const std::string& nombre,
                             const std::string& rol,
                             Usuario& outUsuario,
                             std::string& errorMsg) {
    if (username.empty() || password.empty() || nombre.empty() || rol.empty()) {
        errorMsg = "Todos los campos (username, password, nombre, rol) son obligatorios";
        return false;
    }
    if (username.size() >= 32) {
        errorMsg = "El nombre de usuario excede el límite de 31 caracteres";
        return false;
    }
    if (password.size() >= 64) {
        errorMsg = "La contraseña excede el límite de 63 caracteres";
        return false;
    }
    if (nombre.size() >= 64) {
        errorMsg = "El nombre excede el límite de 63 caracteres";
        return false;
    }
    if (!esRolValido(rol)) {
        errorMsg = "Rol no válido. Debe ser: Administrador, Operador o Auditor";
        return false;
    }
    if (existeUsername(filepath, username)) {
        errorMsg = "El nombre de usuario '" + username + "' ya existe en el sistema";
        return false;
    }

    Usuario nuevo;
    std::memset(&nuevo, 0, sizeof(Usuario));
    nuevo.id = Persistence::obtenerSiguienteId<Usuario>(filepath);

    std::strncpy(nuevo.username, username.c_str(), sizeof(nuevo.username) - 1);
    
    // Almacenamiento seguro mediante algoritmo con sal
    std::string passHash = hashearPassword(password, username);
    std::strncpy(nuevo.password, passHash.c_str(), sizeof(nuevo.password) - 1);
    
    std::strncpy(nuevo.nombre, nombre.c_str(), sizeof(nuevo.nombre) - 1);
    std::strncpy(nuevo.rol, rol.c_str(), sizeof(nuevo.rol) - 1);
    nuevo.activo = true;

    if (!Persistence::insertar<Usuario>(filepath, nuevo)) {
        errorMsg = "Error de escritura en el archivo binario de usuarios";
        return false;
    }

    outUsuario = nuevo;
    return true;
}

inline bool autenticarUsuario(const char* filepath,
                              const std::string& username,
                              const std::string& password,
                              Usuario& outUsuario,
                              std::string& errorMsg) {
    if (username.empty() || password.empty()) {
        errorMsg = "Se requiere username y password para autenticar";
        return false;
    }

    FILE* f = fopen(filepath, "rb");
    if (!f) {
        errorMsg = "No se pudo abrir el archivo de usuarios";
        return false;
    }

    Usuario u;
    bool encontrado = false;
    while (fread(&u, sizeof(Usuario), 1, f) == 1) {
        if (username == u.username) {
            encontrado = true;
            break;
        }
    }
    fclose(f);

    if (!encontrado) {
        errorMsg = "Credenciales inválidas (usuario no encontrado)";
        return false;
    }

    if (!u.activo) {
        errorMsg = "El usuario se encuentra inactivo en el sistema";
        return false;
    }

    if (!verificarPassword(password, u.password, username)) {
        errorMsg = "Credenciales inválidas";
        return false;
    }

    outUsuario = u;
    return true;
}

inline void inicializarUsuarioAdminPorDefecto(const char* filepath = DEFAULT_FILE_USUARIOS) {
    Persistence::asegurarArchivo(filepath);
    std::vector<Usuario> lista = Persistence::leerTodos<Usuario>(filepath, true);
    if (lista.empty()) {
        const char* envAdminPass = std::getenv("ADMIN_DEFAULT_PASSWORD");
        std::string adminPass = (envAdminPass && envAdminPass[0] != '\0') ? envAdminPass : "admin123";
        Usuario outU;
        std::string err;
        registrarUsuario(filepath, "admin", adminPass, "Administrador Inicial", "Administrador", outU, err);
        std::cout << "[AUTH] Usuario por defecto 'admin' inicializado exitosamente." << std::endl;
    }
}

// =============================================================================
// Helper de Extracción y Validación de Tokens Bearer y Autorización RBAC
// =============================================================================

inline std::string extraerBearerToken(const crow::request& req) {
    std::string authHeader = req.get_header_value("Authorization");
    if (authHeader.empty()) {
        return "";
    }
    const std::string prefix = "Bearer ";
    if (authHeader.size() > prefix.size() && authHeader.compare(0, prefix.size(), prefix) == 0) {
        return authHeader.substr(prefix.size());
    }
    return "";
}

struct AuthResult {
    bool autorizado;
    int codigoHttp; // 200, 401, 403
    std::string error;
    Usuario usuario;
};

inline AuthResult validarAcceso(const crow::request& req,
                               const std::vector<std::string>& rolesPermitidos,
                               const char* filepathUsuarios = DEFAULT_FILE_USUARIOS) {
    std::string token = extraerBearerToken(req);
    if (token.empty()) {
        return {false, 401, "Token de autenticación no proporcionado (se requiere Authorization: Bearer <token>)", Usuario{}};
    }

    Usuario u;
    std::string err;
    if (!SessionManager::getInstance().validarToken(token, u, err, filepathUsuarios)) {
        return {false, 401, err, Usuario{}};
    }

    if (!rolesPermitidos.empty()) {
        bool rolPermitido = false;
        for (const auto& r : rolesPermitidos) {
            if (r == u.rol) {
                rolPermitido = true;
                break;
            }
        }
        if (!rolPermitido) {
            return {false, 403, "Acceso denegado: el rol '" + std::string(u.rol) + "' no cuenta con permisos suficientes para esta operación", u};
        }
    }

    return {true, 200, "", u};
}

} // namespace Auth

#endif // AUTH_H
