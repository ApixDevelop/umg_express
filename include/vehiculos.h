#ifndef VEHICULOS_H
#define VEHICULOS_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"

namespace Vehiculos {

const char* const DEFAULT_FILE_VEHICULOS = "data/vehiculos.dat";

// Estados permitidos para un vehículo
inline bool esEstadoValido(const std::string& estado) {
    return (estado == "Disponible" || estado == "En ruta" || estado == "En taller");
}

// Obtiene la lista de todos los vehículos activos
inline std::vector<Vehiculo> listarVehiculos(const char* filepath = DEFAULT_FILE_VEHICULOS) {
    return Persistence::leerTodos<Vehiculo>(filepath, true);
}

// Obtiene un vehículo por su ID si se encuentra activo
inline bool obtenerVehiculoPorId(const char* filepath, int id, Vehiculo& outVehiculo) {
    long index = -1;
    return Persistence::buscarPorId<Vehiculo>(filepath, id, outVehiculo, index);
}

// Comprueba si ya existe un vehículo registrado con la misma placa (excluyendo un ID opcional)
inline bool existePlaca(const char* filepath, const std::string& placa, int idExcluir = -1) {
    std::vector<Vehiculo> lista = Persistence::leerTodos<Vehiculo>(filepath, true);
    for (const auto& v : lista) {
        if (v.id != idExcluir && placa == v.placa) {
            return true;
        }
    }
    return false;
}

// Crea un nuevo vehículo y lo guarda en el archivo binario
inline bool crearVehiculo(const char* filepath,
                          const std::string& placa,
                          const std::string& marcaModelo,
                          double capacidadPesoKg,
                          double capacidadVolumenM3,
                          const std::string& estado,
                          Vehiculo& outVehiculo,
                          std::string& errorMsg) {
    if (placa.empty() || marcaModelo.empty() || estado.empty()) {
        errorMsg = "Los campos placa, marcaModelo y estado son obligatorios";
        return false;
    }

    if (placa.size() >= 15) {
        errorMsg = "La placa excede el límite de 14 caracteres";
        return false;
    }
    if (marcaModelo.size() >= 50) {
        errorMsg = "La marca y modelo exceden el límite de 49 caracteres";
        return false;
    }
    if (estado.size() >= 20) {
        errorMsg = "El estado excede el límite de 19 caracteres";
        return false;
    }

    if (capacidadPesoKg <= 0.0) {
        errorMsg = "La capacidad de peso debe ser mayor a 0 kg";
        return false;
    }

    if (capacidadVolumenM3 <= 0.0) {
        errorMsg = "La capacidad de volumen debe ser mayor a 0 m3";
        return false;
    }

    if (!esEstadoValido(estado)) {
        errorMsg = "Estado de vehículo no válido. Debe ser: Disponible, En ruta o En taller";
        return false;
    }

    if (existePlaca(filepath, placa)) {
        errorMsg = "Ya existe un vehículo registrado con la placa '" + placa + "'";
        return false;
    }

    Vehiculo nuevo;
    std::memset(&nuevo, 0, sizeof(Vehiculo));
    nuevo.id = Persistence::obtenerSiguienteId<Vehiculo>(filepath);

    std::strncpy(nuevo.placa, placa.c_str(), sizeof(nuevo.placa) - 1);
    std::strncpy(nuevo.marcaModelo, marcaModelo.c_str(), sizeof(nuevo.marcaModelo) - 1);
    std::strncpy(nuevo.estado, estado.c_str(), sizeof(nuevo.estado) - 1);
    nuevo.capacidadPesoKg = capacidadPesoKg;
    nuevo.capacidadVolumenM3 = capacidadVolumenM3;
    nuevo.activo = true;

    if (!Persistence::insertar<Vehiculo>(filepath, nuevo)) {
        errorMsg = "Error de escritura en el archivo binario de vehículos";
        return false;
    }

    outVehiculo = nuevo;
    return true;
}

// Actualiza los datos de un vehículo existente
inline bool actualizarVehiculo(const char* filepath,
                              int id,
                              const std::string& placa,
                              const std::string& marcaModelo,
                              double capacidadPesoKg,
                              double capacidadVolumenM3,
                              const std::string& estado,
                              Vehiculo& outVehiculo,
                              std::string& errorMsg) {
    Vehiculo actual;
    if (!obtenerVehiculoPorId(filepath, id, actual)) {
        errorMsg = "Vehículo con ID " + std::to_string(id) + " no encontrado o inactivo";
        return false;
    }

    if (placa.empty() || marcaModelo.empty() || estado.empty()) {
        errorMsg = "Los campos placa, marcaModelo y estado son obligatorios";
        return false;
    }

    if (placa.size() >= 15 || marcaModelo.size() >= 50 || estado.size() >= 20) {
        errorMsg = "Uno de los campos excede la longitud máxima permitida";
        return false;
    }

    if (capacidadPesoKg <= 0.0 || capacidadVolumenM3 <= 0.0) {
        errorMsg = "Las capacidades de peso y volumen deben ser mayores a 0";
        return false;
    }

    if (!esEstadoValido(estado)) {
        errorMsg = "Estado de vehículo no válido. Debe ser: Disponible, En ruta o En taller";
        return false;
    }

    if (existePlaca(filepath, placa, id)) {
        errorMsg = "Ya existe otro vehículo registrado con la placa '" + placa + "'";
        return false;
    }

    std::memset(actual.placa, 0, sizeof(actual.placa));
    std::memset(actual.marcaModelo, 0, sizeof(actual.marcaModelo));
    std::memset(actual.estado, 0, sizeof(actual.estado));

    std::strncpy(actual.placa, placa.c_str(), sizeof(actual.placa) - 1);
    std::strncpy(actual.marcaModelo, marcaModelo.c_str(), sizeof(actual.marcaModelo) - 1);
    std::strncpy(actual.estado, estado.c_str(), sizeof(actual.estado) - 1);
    actual.capacidadPesoKg = capacidadPesoKg;
    actual.capacidadVolumenM3 = capacidadVolumenM3;

    if (!Persistence::actualizarPorId<Vehiculo>(filepath, id, actual)) {
        errorMsg = "Error al actualizar el vehículo en el archivo binario";
        return false;
    }

    outVehiculo = actual;
    return true;
}

// Realiza la baja lógica (activo = false) de un vehículo por su ID
inline bool eliminarVehiculo(const char* filepath, int id, std::string& errorMsg) {
    Vehiculo actual;
    if (!obtenerVehiculoPorId(filepath, id, actual)) {
        errorMsg = "Vehículo con ID " + std::to_string(id) + " no encontrado o ya inactivo";
        return false;
    }

    if (!Persistence::eliminarLogico<Vehiculo>(filepath, id)) {
        errorMsg = "Error al realizar la baja lógica del vehículo en disco";
        return false;
    }

    return true;
}

} // namespace Vehiculos

#endif // VEHICULOS_H
