#ifndef TARIFAS_H
#define TARIFAS_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"

namespace Tarifas {

const char* const DEFAULT_FILE_TARIFAS = "data/tarifas.dat";

// Obtiene la lista de todas las tarifas activas
inline std::vector<Tarifa> listarTarifas(const char* filepath = DEFAULT_FILE_TARIFAS) {
    return Persistence::leerTodos<Tarifa>(filepath, true);
}

// Obtiene una tarifa por su ID si está activa
inline bool obtenerTarifaPorId(const char* filepath, int id, Tarifa& outTarifa) {
    long index = -1;
    return Persistence::buscarPorId<Tarifa>(filepath, id, outTarifa, index);
}

// Comprueba si ya existe una tarifa activa para el mismo tipo de carga (excluyendo un ID opcional)
inline bool existeTipoCarga(const char* filepath, const std::string& tipoCarga, int idExcluir = -1) {
    std::vector<Tarifa> lista = Persistence::leerTodos<Tarifa>(filepath, true);
    for (const auto& t : lista) {
        if (t.id != idExcluir && tipoCarga == t.tipoCarga) {
            return true;
        }
    }
    return false;
}

// Crea una nueva tarifa y la guarda en el archivo binario
inline bool crearTarifa(const char* filepath,
                         const std::string& tipoCarga,
                         double precioPorKg,
                         double precioPorM3,
                         double recargoEspecial,
                         Tarifa& outTarifa,
                         std::string& errorMsg) {
    if (tipoCarga.empty()) {
        errorMsg = "El tipo de carga es un campo obligatorio";
        return false;
    }

    if (tipoCarga.size() >= 32) {
        errorMsg = "El tipo de carga excede el límite de 31 caracteres";
        return false;
    }

    if (precioPorKg < 0.0 || precioPorM3 < 0.0 || recargoEspecial < 0.0) {
        errorMsg = "Los precios y recargos no pueden ser valores negativos";
        return false;
    }

    if (existeTipoCarga(filepath, tipoCarga)) {
        errorMsg = "Ya existe una tarifa registrada para el tipo de carga '" + tipoCarga + "'";
        return false;
    }

    Tarifa nueva;
    std::memset(&nueva, 0, sizeof(Tarifa));
    nueva.id = Persistence::obtenerSiguienteId<Tarifa>(filepath);

    std::strncpy(nueva.tipoCarga, tipoCarga.c_str(), sizeof(nueva.tipoCarga) - 1);
    nueva.precioPorKg = precioPorKg;
    nueva.precioPorM3 = precioPorM3;
    nueva.recargoEspecial = recargoEspecial;
    nueva.activo = true;

    if (!Persistence::insertar<Tarifa>(filepath, nueva)) {
        errorMsg = "Error de escritura en el archivo binario de tarifas";
        return false;
    }

    outTarifa = nueva;
    return true;
}

// Actualiza los datos de una tarifa existente
inline bool actualizarTarifa(const char* filepath,
                            int id,
                            const std::string& tipoCarga,
                            double precioPorKg,
                            double precioPorM3,
                            double recargoEspecial,
                            Tarifa& outTarifa,
                            std::string& errorMsg) {
    Tarifa actual;
    if (!obtenerTarifaPorId(filepath, id, actual)) {
        errorMsg = "Tarifa con ID " + std::to_string(id) + " no encontrada o inactiva";
        return false;
    }

    if (tipoCarga.empty()) {
        errorMsg = "El tipo de carga es un campo obligatorio";
        return false;
    }

    if (tipoCarga.size() >= 32) {
        errorMsg = "El tipo de carga excede la longitud máxima de 31 caracteres";
        return false;
    }

    if (precioPorKg < 0.0 || precioPorM3 < 0.0 || recargoEspecial < 0.0) {
        errorMsg = "Los precios y recargos no pueden ser valores negativos";
        return false;
    }

    if (existeTipoCarga(filepath, tipoCarga, id)) {
        errorMsg = "Ya existe otra tarifa registrada para el tipo de carga '" + tipoCarga + "'";
        return false;
    }

    std::memset(actual.tipoCarga, 0, sizeof(actual.tipoCarga));
    std::strncpy(actual.tipoCarga, tipoCarga.c_str(), sizeof(actual.tipoCarga) - 1);
    actual.precioPorKg = precioPorKg;
    actual.precioPorM3 = precioPorM3;
    actual.recargoEspecial = recargoEspecial;

    if (!Persistence::actualizarPorId<Tarifa>(filepath, id, actual)) {
        errorMsg = "Error al actualizar la tarifa en el archivo binario";
        return false;
    }

    outTarifa = actual;
    return true;
}

// Realiza la baja lógica (activo = false) de una tarifa por su ID
inline bool eliminarTarifa(const char* filepath, int id, std::string& errorMsg) {
    Tarifa actual;
    if (!obtenerTarifaPorId(filepath, id, actual)) {
        errorMsg = "Tarifa con ID " + std::to_string(id) + " no encontrada o ya inactiva";
        return false;
    }

    if (!Persistence::eliminarLogico<Tarifa>(filepath, id)) {
        errorMsg = "Error al realizar la baja lógica de la tarifa en disco";
        return false;
    }

    return true;
}

} // namespace Tarifas

#endif // TARIFAS_H
