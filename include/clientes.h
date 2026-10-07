#ifndef CLIENTES_H
#define CLIENTES_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"

namespace Clientes {

const char* const DEFAULT_FILE_CLIENTES = "data/clientes.dat";

// Obtiene la lista de todos los clientes activos
inline std::vector<Cliente> listarClientes(const char* filepath = DEFAULT_FILE_CLIENTES) {
    return Persistence::leerTodos<Cliente>(filepath, true);
}

// Obtiene un cliente por su ID si está activo
inline bool obtenerClientePorId(const char* filepath, int id, Cliente& outCliente) {
    long index = -1;
    return Persistence::buscarPorId<Cliente>(filepath, id, outCliente, index);
}

// Comprueba si ya existe un cliente con el mismo NIT (excluyendo un ID opcional)
inline bool existeNitCliente(const char* filepath, const std::string& nit, int idExcluir = -1) {
    std::vector<Cliente> lista = Persistence::leerTodos<Cliente>(filepath, true);
    for (const auto& c : lista) {
        if (c.id != idExcluir && nit == c.nit) {
            return true;
        }
    }
    return false;
}

// Crea un nuevo cliente y lo guarda en el archivo binario
inline bool crearCliente(const char* filepath,
                         const std::string& nit,
                         const std::string& nombre,
                         const std::string& direccion,
                         const std::string& telefono,
                         Cliente& outCliente,
                         std::string& errorMsg) {
    if (nit.empty() || nombre.empty() || direccion.empty() || telefono.empty()) {
        errorMsg = "Los campos nit, nombre, direccion y telefono son obligatorios";
        return false;
    }

    if (nit.size() >= 20) {
        errorMsg = "El NIT del cliente excede el límite de 19 caracteres";
        return false;
    }
    if (nombre.size() >= 64) {
        errorMsg = "El nombre del cliente excede el límite de 63 caracteres";
        return false;
    }
    if (direccion.size() >= 128) {
        errorMsg = "La dirección excede el límite de 127 caracteres";
        return false;
    }
    if (telefono.size() >= 20) {
        errorMsg = "El teléfono excede el límite de 19 caracteres";
        return false;
    }

    if (existeNitCliente(filepath, nit)) {
        errorMsg = "Ya existe un cliente registrado con el NIT '" + nit + "'";
        return false;
    }

    Cliente nuevo;
    std::memset(&nuevo, 0, sizeof(Cliente));
    nuevo.id = Persistence::obtenerSiguienteId<Cliente>(filepath);

    std::strncpy(nuevo.nit, nit.c_str(), sizeof(nuevo.nit) - 1);
    std::strncpy(nuevo.nombre, nombre.c_str(), sizeof(nuevo.nombre) - 1);
    std::strncpy(nuevo.direccion, direccion.c_str(), sizeof(nuevo.direccion) - 1);
    std::strncpy(nuevo.telefono, telefono.c_str(), sizeof(nuevo.telefono) - 1);
    nuevo.activo = true;

    if (!Persistence::insertar<Cliente>(filepath, nuevo)) {
        errorMsg = "Error de escritura en el archivo binario de clientes";
        return false;
    }

    outCliente = nuevo;
    return true;
}

// Actualiza los datos de un cliente existente
inline bool actualizarCliente(const char* filepath,
                             int id,
                             const std::string& nit,
                             const std::string& nombre,
                             const std::string& direccion,
                             const std::string& telefono,
                             Cliente& outCliente,
                             std::string& errorMsg) {
    Cliente actual;
    if (!obtenerClientePorId(filepath, id, actual)) {
        errorMsg = "Cliente con ID " + std::to_string(id) + " no encontrado o inactivo";
        return false;
    }

    if (nit.empty() || nombre.empty() || direccion.empty() || telefono.empty()) {
        errorMsg = "Los campos nit, nombre, direccion y telefono son obligatorios";
        return false;
    }

    if (nit.size() >= 20 || nombre.size() >= 64 || direccion.size() >= 128 || telefono.size() >= 20) {
        errorMsg = "Uno de los campos excede la longitud máxima permitida";
        return false;
    }

    if (existeNitCliente(filepath, nit, id)) {
        errorMsg = "Ya existe otro cliente registrado con el NIT '" + nit + "'";
        return false;
    }

    std::memset(actual.nit, 0, sizeof(actual.nit));
    std::memset(actual.nombre, 0, sizeof(actual.nombre));
    std::memset(actual.direccion, 0, sizeof(actual.direccion));
    std::memset(actual.telefono, 0, sizeof(actual.telefono));

    std::strncpy(actual.nit, nit.c_str(), sizeof(actual.nit) - 1);
    std::strncpy(actual.nombre, nombre.c_str(), sizeof(actual.nombre) - 1);
    std::strncpy(actual.direccion, direccion.c_str(), sizeof(actual.direccion) - 1);
    std::strncpy(actual.telefono, telefono.c_str(), sizeof(actual.telefono) - 1);

    if (!Persistence::actualizarPorId<Cliente>(filepath, id, actual)) {
        errorMsg = "Error al actualizar el cliente en el archivo binario";
        return false;
    }

    outCliente = actual;
    return true;
}

// Realiza la baja lógica (activo = false) de un cliente por su ID
inline bool eliminarCliente(const char* filepath, int id, std::string& errorMsg) {
    Cliente actual;
    if (!obtenerClientePorId(filepath, id, actual)) {
        errorMsg = "Cliente con ID " + std::to_string(id) + " no encontrado o ya inactivo";
        return false;
    }

    if (!Persistence::eliminarLogico<Cliente>(filepath, id)) {
        errorMsg = "Error al realizar la baja lógica del cliente en disco";
        return false;
    }

    return true;
}

} // namespace Clientes

#endif // CLIENTES_H
