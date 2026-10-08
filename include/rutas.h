#ifndef RUTAS_H
#define RUTAS_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "sucursales.h"

namespace Rutas {

const char* const DEFAULT_FILE_RUTAS = "data/rutas.dat";

// Obtiene la lista de todas las rutas activas
inline std::vector<Ruta> listarRutas(const char* filepath = DEFAULT_FILE_RUTAS) {
    return Persistence::leerTodos<Ruta>(filepath, true);
}

// Obtiene una ruta por su ID si está activa
inline bool obtenerRutaPorId(const char* filepath, int id, Ruta& outRuta) {
    long index = -1;
    return Persistence::buscarPorId<Ruta>(filepath, id, outRuta, index);
}

// Comprueba si ya existe una ruta activa entre el mismo origen y destino
inline bool existeRutaEntreSucursales(const char* filepath, int idOrigen, int idDestino, int idExcluir = -1) {
    std::vector<Ruta> lista = Persistence::leerTodos<Ruta>(filepath, true);
    for (const auto& r : lista) {
        if (r.id != idExcluir && r.idSucursalOrigen == idOrigen && r.idSucursalDestino == idDestino) {
            return true;
        }
    }
    return false;
}

// Crea una nueva ruta validando la existencia de las sucursales de origen y destino
inline bool crearRuta(const char* filepath,
                      const char* sucursalesFilepath,
                      int idSucursalOrigen,
                      int idSucursalDestino,
                      double distanciaKm,
                      double tarifaBase,
                      Ruta& outRuta,
                      std::string& errorMsg) {
    if (idSucursalOrigen <= 0 || idSucursalDestino <= 0) {
        errorMsg = "Los IDs de las sucursales de origen y destino deben ser válidos y mayores a 0";
        return false;
    }

    if (idSucursalOrigen == idSucursalDestino) {
        errorMsg = "La sucursal de origen y destino no pueden ser la misma";
        return false;
    }

    if (distanciaKm <= 0.0) {
        errorMsg = "La distancia en kilómetros debe ser mayor a 0";
        return false;
    }

    if (tarifaBase < 0.0) {
        errorMsg = "La tarifa base no puede ser negativa";
        return false;
    }

    // Validar existencia de sucursal origen
    Sucursal orig;
    if (!Sucursales::obtenerSucursalPorId(sucursalesFilepath, idSucursalOrigen, orig)) {
        errorMsg = "La sucursal de origen con ID " + std::to_string(idSucursalOrigen) + " no existe o está inactiva";
        return false;
    }

    // Validar existencia de sucursal destino
    Sucursal dest;
    if (!Sucursales::obtenerSucursalPorId(sucursalesFilepath, idSucursalDestino, dest)) {
        errorMsg = "La sucursal de destino con ID " + std::to_string(idSucursalDestino) + " no existe o está inactiva";
        return false;
    }

    // Validar ruta duplicada (mismo origen -> destino)
    if (existeRutaEntreSucursales(filepath, idSucursalOrigen, idSucursalDestino)) {
        errorMsg = "Ya existe una ruta activa registrada entre la sucursal " + std::to_string(idSucursalOrigen) + " y " + std::to_string(idSucursalDestino);
        return false;
    }

    Ruta nueva;
    std::memset(&nueva, 0, sizeof(Ruta));
    nueva.id = Persistence::obtenerSiguienteId<Ruta>(filepath);
    nueva.idSucursalOrigen = idSucursalOrigen;
    nueva.idSucursalDestino = idSucursalDestino;
    nueva.distanciaKm = distanciaKm;
    nueva.tarifaBase = tarifaBase;
    nueva.activo = true;

    if (!Persistence::insertar<Ruta>(filepath, nueva)) {
        errorMsg = "Error de escritura en el archivo binario de rutas";
        return false;
    }

    outRuta = nueva;
    return true;
}

// Actualiza los datos de una ruta existente
inline bool actualizarRuta(const char* filepath,
                         const char* sucursalesFilepath,
                         int id,
                         int idSucursalOrigen,
                         int idSucursalDestino,
                         double distanciaKm,
                         double tarifaBase,
                         Ruta& outRuta,
                         std::string& errorMsg) {
    Ruta actual;
    if (!obtenerRutaPorId(filepath, id, actual)) {
        errorMsg = "Ruta con ID " + std::to_string(id) + " no encontrada o inactiva";
        return false;
    }

    if (idSucursalOrigen <= 0 || idSucursalDestino <= 0) {
        errorMsg = "Los IDs de las sucursales de origen y destino deben ser válidos";
        return false;
    }

    if (idSucursalOrigen == idSucursalDestino) {
        errorMsg = "La sucursal de origen y destino no pueden ser la misma";
        return false;
    }

    if (distanciaKm <= 0.0) {
        errorMsg = "La distancia en kilómetros debe ser mayor a 0";
        return false;
    }

    if (tarifaBase < 0.0) {
        errorMsg = "La tarifa base no puede ser negativa";
        return false;
    }

    Sucursal orig;
    if (!Sucursales::obtenerSucursalPorId(sucursalesFilepath, idSucursalOrigen, orig)) {
        errorMsg = "La sucursal de origen con ID " + std::to_string(idSucursalOrigen) + " no existe o está inactiva";
        return false;
    }

    Sucursal dest;
    if (!Sucursales::obtenerSucursalPorId(sucursalesFilepath, idSucursalDestino, dest)) {
        errorMsg = "La sucursal de destino con ID " + std::to_string(idSucursalDestino) + " no existe o está inactiva";
        return false;
    }

    if (existeRutaEntreSucursales(filepath, idSucursalOrigen, idSucursalDestino, id)) {
        errorMsg = "Ya existe otra ruta registrada entre las sucursales " + std::to_string(idSucursalOrigen) + " y " + std::to_string(idSucursalDestino);
        return false;
    }

    actual.idSucursalOrigen = idSucursalOrigen;
    actual.idSucursalDestino = idSucursalDestino;
    actual.distanciaKm = distanciaKm;
    actual.tarifaBase = tarifaBase;

    if (!Persistence::actualizarPorId<Ruta>(filepath, id, actual)) {
        errorMsg = "Error al actualizar la ruta en el archivo binario";
        return false;
    }

    outRuta = actual;
    return true;
}

// Realiza la baja lógica de una ruta por ID
inline bool eliminarRuta(const char* filepath, int id, std::string& errorMsg) {
    Ruta actual;
    if (!obtenerRutaPorId(filepath, id, actual)) {
        errorMsg = "Ruta con ID " + std::to_string(id) + " no encontrada o ya inactiva";
        return false;
    }

    if (!Persistence::eliminarLogico<Ruta>(filepath, id)) {
        errorMsg = "Error al realizar la baja lógica de la ruta en disco";
        return false;
    }

    return true;
}

} // namespace Rutas

#endif // RUTAS_H
