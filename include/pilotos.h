#ifndef PILOTOS_H
#define PILOTOS_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"

namespace Pilotos {

const char* const DEFAULT_FILE_PILOTOS = "data/pilotos.dat";

// Obtiene la lista de todos los pilotos activos
inline std::vector<Piloto> listarPilotos(const char* filepath = DEFAULT_FILE_PILOTOS) {
    return Persistence::leerTodos<Piloto>(filepath, true);
}

// Obtiene un piloto por su ID si se encuentra activo
inline bool obtenerPilotoPorId(const char* filepath, int id, Piloto& outPiloto) {
    long index = -1;
    return Persistence::buscarPorId<Piloto>(filepath, id, outPiloto, index);
}

// Comprueba si ya existe un piloto registrado con el mismo número de licencia (excluyendo un ID opcional)
inline bool existeLicencia(const char* filepath, const std::string& licencia, int idExcluir = -1) {
    std::vector<Piloto> lista = Persistence::leerTodos<Piloto>(filepath, true);
    for (const auto& p : lista) {
        if (p.id != idExcluir && licencia == p.licencia) {
            return true;
        }
    }
    return false;
}

// Crea un nuevo piloto y lo guarda en el archivo binario
inline bool crearPiloto(const char* filepath,
                        const std::string& nombre,
                        const std::string& licencia,
                        const std::string& telefono,
                        Piloto& outPiloto,
                        std::string& errorMsg) {
    if (nombre.empty() || licencia.empty() || telefono.empty()) {
        errorMsg = "Los campos nombre, licencia y telefono son obligatorios";
        return false;
    }

    if (nombre.size() >= 64) {
        errorMsg = "El nombre del piloto excede el límite de 63 caracteres";
        return false;
    }
    if (licencia.size() >= 20) {
        errorMsg = "La licencia excede el límite de 19 caracteres";
        return false;
    }
    if (telefono.size() >= 20) {
        errorMsg = "El teléfono excede el límite de 19 caracteres";
        return false;
    }

    if (existeLicencia(filepath, licencia)) {
        errorMsg = "Ya existe un piloto registrado con el número de licencia '" + licencia + "'";
        return false;
    }

    Piloto nuevo;
    std::memset(&nuevo, 0, sizeof(Piloto));
    nuevo.id = Persistence::obtenerSiguienteId<Piloto>(filepath);

    std::strncpy(nuevo.nombre, nombre.c_str(), sizeof(nuevo.nombre) - 1);
    std::strncpy(nuevo.licencia, licencia.c_str(), sizeof(nuevo.licencia) - 1);
    std::strncpy(nuevo.telefono, telefono.c_str(), sizeof(nuevo.telefono) - 1);
    nuevo.activo = true;

    if (!Persistence::insertar<Piloto>(filepath, nuevo)) {
        errorMsg = "Error de escritura en el archivo binario de pilotos";
        return false;
    }

    outPiloto = nuevo;
    return true;
}

// Actualiza los datos de un piloto existente
inline bool actualizarPiloto(const char* filepath,
                            int id,
                            const std::string& nombre,
                            const std::string& licencia,
                            const std::string& telefono,
                            Piloto& outPiloto,
                            std::string& errorMsg) {
    Piloto actual;
    if (!obtenerPilotoPorId(filepath, id, actual)) {
        errorMsg = "Piloto con ID " + std::to_string(id) + " no encontrado o inactivo";
        return false;
    }

    if (nombre.empty() || licencia.empty() || telefono.empty()) {
        errorMsg = "Los campos nombre, licencia y telefono son obligatorios";
        return false;
    }

    if (nombre.size() >= 64 || licencia.size() >= 20 || telefono.size() >= 20) {
        errorMsg = "Uno de los campos excede la longitud máxima permitida";
        return false;
    }

    if (existeLicencia(filepath, licencia, id)) {
        errorMsg = "Ya existe otro piloto registrado con el número de licencia '" + licencia + "'";
        return false;
    }

    std::memset(actual.nombre, 0, sizeof(actual.nombre));
    std::memset(actual.licencia, 0, sizeof(actual.licencia));
    std::memset(actual.telefono, 0, sizeof(actual.telefono));

    std::strncpy(actual.nombre, nombre.c_str(), sizeof(actual.nombre) - 1);
    std::strncpy(actual.licencia, licencia.c_str(), sizeof(actual.licencia) - 1);
    std::strncpy(actual.telefono, telefono.c_str(), sizeof(actual.telefono) - 1);

    if (!Persistence::actualizarPorId<Piloto>(filepath, id, actual)) {
        errorMsg = "Error al actualizar el piloto en el archivo binario";
        return false;
    }

    outPiloto = actual;
    return true;
}

// Realiza la baja lógica (activo = false) de un piloto por su ID
inline bool eliminarPiloto(const char* filepath, int id, std::string& errorMsg) {
    Piloto actual;
    if (!obtenerPilotoPorId(filepath, id, actual)) {
        errorMsg = "Piloto con ID " + std::to_string(id) + " no encontrado o ya inactivo";
        return false;
    }

    if (!Persistence::eliminarLogico<Piloto>(filepath, id)) {
        errorMsg = "Error al realizar la baja lógica del piloto en disco";
        return false;
    }

    return true;
}

} // namespace Pilotos

#endif // PILOTOS_H
