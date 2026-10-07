#ifndef SUCURSALES_H
#define SUCURSALES_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"

namespace Sucursales {

const char* const DEFAULT_FILE_SUCURSALES = "data/sucursales.dat";

// Obtiene la lista de todas las sucursales activas
inline std::vector<Sucursal> listarSucursales(const char* filepath = DEFAULT_FILE_SUCURSALES) {
    return Persistence::leerTodos<Sucursal>(filepath, true);
}

// Obtiene una sucursal por su ID si se encuentra activa
inline bool obtenerSucursalPorId(const char* filepath, int id, Sucursal& outSucursal) {
    long index = -1;
    return Persistence::buscarPorId<Sucursal>(filepath, id, outSucursal, index);
}

// Comprueba si ya existe una sucursal con el mismo nombre (excluyendo un ID opcional)
inline bool existeNombreSucursal(const char* filepath, const std::string& nombre, int idExcluir = -1) {
    std::vector<Sucursal> lista = Persistence::leerTodos<Sucursal>(filepath, true);
    for (const auto& s : lista) {
        if (s.id != idExcluir && nombre == s.nombre) {
            return true;
        }
    }
    return false;
}

// Crea una nueva sucursal y la guarda en el archivo binario
inline bool crearSucursal(const char* filepath,
                          const std::string& nombre,
                          const std::string& direccion,
                          const std::string& telefono,
                          Sucursal& outSucursal,
                          std::string& errorMsg) {
    if (nombre.empty() || direccion.empty() || telefono.empty()) {
        errorMsg = "Los campos nombre, direccion y telefono son obligatorios";
        return false;
    }

    if (nombre.size() >= 64) {
        errorMsg = "El nombre de la sucursal excede el límite de 63 caracteres";
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

    if (existeNombreSucursal(filepath, nombre)) {
        errorMsg = "Ya existe una sucursal registrada con el nombre '" + nombre + "'";
        return false;
    }

    Sucursal nueva;
    std::memset(&nueva, 0, sizeof(Sucursal));
    nueva.id = Persistence::obtenerSiguienteId<Sucursal>(filepath);

    std::strncpy(nueva.nombre, nombre.c_str(), sizeof(nueva.nombre) - 1);
    std::strncpy(nueva.direccion, direccion.c_str(), sizeof(nueva.direccion) - 1);
    std::strncpy(nueva.telefono, telefono.c_str(), sizeof(nueva.telefono) - 1);
    nueva.activo = true;

    if (!Persistence::insertar<Sucursal>(filepath, nueva)) {
        errorMsg = "Error de escritura en el archivo binario de sucursales";
        return false;
    }

    outSucursal = nueva;
    return true;
}

// Actualiza los datos de una sucursal existente
inline bool actualizarSucursal(const char* filepath,
                              int id,
                              const std::string& nombre,
                              const std::string& direccion,
                              const std::string& telefono,
                              Sucursal& outSucursal,
                              std::string& errorMsg) {
    Sucursal actual;
    if (!obtenerSucursalPorId(filepath, id, actual)) {
        errorMsg = "Sucursal con ID " + std::to_string(id) + " no encontrada o inactiva";
        return false;
    }

    if (nombre.empty() || direccion.empty() || telefono.empty()) {
        errorMsg = "Los campos nombre, direccion y telefono son obligatorios";
        return false;
    }

    if (nombre.size() >= 64 || direccion.size() >= 128 || telefono.size() >= 20) {
        errorMsg = "Uno de los campos excede la longitud máxima permitida";
        return false;
    }

    if (existeNombreSucursal(filepath, nombre, id)) {
        errorMsg = "Ya existe otra sucursal registrada con el nombre '" + nombre + "'";
        return false;
    }

    std::memset(actual.nombre, 0, sizeof(actual.nombre));
    std::memset(actual.direccion, 0, sizeof(actual.direccion));
    std::memset(actual.telefono, 0, sizeof(actual.telefono));

    std::strncpy(actual.nombre, nombre.c_str(), sizeof(actual.nombre) - 1);
    std::strncpy(actual.direccion, direccion.c_str(), sizeof(actual.direccion) - 1);
    std::strncpy(actual.telefono, telefono.c_str(), sizeof(actual.telefono) - 1);

    if (!Persistence::actualizarPorId<Sucursal>(filepath, id, actual)) {
        errorMsg = "Error al actualizar la sucursal en el archivo binario";
        return false;
    }

    outSucursal = actual;
    return true;
}

// Realiza la baja lógica (activo = false) de una sucursal por su ID
inline bool eliminarSucursal(const char* filepath, int id, std::string& errorMsg) {
    Sucursal actual;
    if (!obtenerSucursalPorId(filepath, id, actual)) {
        errorMsg = "Sucursal con ID " + std::to_string(id) + " no encontrada o ya inactiva";
        return false;
    }

    if (!Persistence::eliminarLogico<Sucursal>(filepath, id)) {
        errorMsg = "Error al realizar la baja lógica de la sucursal en disco";
        return false;
    }

    return true;
}

} // namespace Sucursales

#endif // SUCURSALES_H
