#ifndef AUTH_H
#define AUTH_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"

namespace Auth {

const char* const DEFAULT_FILE_USUARIOS = "data/usuarios.dat";

// Roles soportados por el sistema
inline bool esRolValido(const std::string& rol) {
    return (rol == "Administrador" || rol == "Operador" || rol == "Auditor");
}

// Busca un usuario activo por username
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

// Comprueba si un nombre de usuario ya está registrado y activo
inline bool existeUsername(const char* filepath, const std::string& username) {
    Usuario u;
    long idx = -1;
    return buscarPorUsername(filepath, username, u, idx);
}

// Registra un nuevo usuario en la base binaria
inline bool registrarUsuario(const char* filepath,
                             const std::string& username,
                             const std::string& password,
                             const std::string& nombre,
                             const std::string& rol,
                             Usuario& outUsuario,
                             std::string& errorMsg) {
    // 1. Validar campos no vacíos
    if (username.empty() || password.empty() || nombre.empty() || rol.empty()) {
        errorMsg = "Todos los campos (username, password, nombre, rol) son obligatorios";
        return false;
    }

    // 2. Validar tamaño máximo de campos
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

    // 3. Validar rol
    if (!esRolValido(rol)) {
        errorMsg = "Rol no válido. Debe ser: Administrador, Operador o Auditor";
        return false;
    }

    // 4. Validar duplicados por username
    if (existeUsername(filepath, username)) {
        errorMsg = "El nombre de usuario '" + username + "' ya existe en el sistema";
        return false;
    }

    // 5. Preparar estructura
    Usuario nuevo;
    std::memset(&nuevo, 0, sizeof(Usuario));
    nuevo.id = Persistence::obtenerSiguienteId<Usuario>(filepath);

    std::strncpy(nuevo.username, username.c_str(), sizeof(nuevo.username) - 1);
    std::strncpy(nuevo.password, password.c_str(), sizeof(nuevo.password) - 1);
    std::strncpy(nuevo.nombre, nombre.c_str(), sizeof(nuevo.nombre) - 1);
    std::strncpy(nuevo.rol, rol.c_str(), sizeof(nuevo.rol) - 1);
    nuevo.activo = true;

    // 6. Guardar en archivo binario
    if (!Persistence::insertar<Usuario>(filepath, nuevo)) {
        errorMsg = "Error de escritura en el archivo binario de usuarios";
        return false;
    }

    outUsuario = nuevo;
    return true;
}

// Autentica credenciales de usuario
inline bool autenticarUsuario(const char* filepath,
                              const std::string& username,
                              const std::string& password,
                              Usuario& outUsuario,
                              std::string& errorMsg) {
    if (username.empty() || password.empty()) {
        errorMsg = "Se requiere username y password para autenticar";
        return false;
    }

    // Buscar en archivo binario
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
        errorMsg = "Usuario no encontrado";
        return false;
    }

    if (!u.activo) {
        errorMsg = "El usuario se encuentra inactivo en el sistema";
        return false;
    }

    if (password != u.password) {
        errorMsg = "Credenciales inválidas (contraseña incorrecta)";
        return false;
    }

    outUsuario = u;
    return true;
}

// Si no existe ningún usuario activo, se crea el usuario por defecto 'admin'
inline void inicializarUsuarioAdminPorDefecto(const char* filepath = DEFAULT_FILE_USUARIOS) {
    Persistence::asegurarArchivo(filepath);
    std::vector<Usuario> lista = Persistence::leerTodos<Usuario>(filepath, true);
    if (lista.empty()) {
        Usuario outU;
        std::string err;
        registrarUsuario(filepath, "admin", "admin123", "Administrador Inicial", "Administrador", outU, err);
        std::cout << "[AUTH] Usuario por defecto 'admin' creado exitosamente." << std::endl;
    }
}

} // namespace Auth

#endif // AUTH_H
