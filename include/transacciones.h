#ifndef TRANSACCIONES_H
#define TRANSACCIONES_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include "structs.h"
#include "persistence.h"
#include "clientes.h"
#include "rutas.h"
#include "tarifas.h"

namespace Guias {

const char* const DEFAULT_FILE_GUIAS = "data/guias.dat";

// -----------------------------------------------------------------------------
// Funciones Matemáticas de Cálculo (Fase 5B)
// -----------------------------------------------------------------------------

// Calcula el peso volumétrico en kg: (largo * ancho * alto) / 5000.0
inline double calcularPesoVolumetrico(double largoCm, double anchoCm, double altoCm) {
    if (largoCm <= 0.0 || anchoCm <= 0.0 || altoCm <= 0.0) return 0.0;
    return (largoCm * anchoCm * altoCm) / 5000.0;
}

// Determina el peso facturable en kg: el mayor entre peso real y peso volumétrico
inline double calcularPesoFacturable(double pesoRealKg, double pesoVolumetricoKg) {
    return std::max(pesoRealKg, pesoVolumetricoKg);
}

// Calcula el volumen en m3: (largo * ancho * alto) / 1,000,000.0
inline double calcularVolumenM3(double largoCm, double anchoCm, double altoCm) {
    if (largoCm <= 0.0 || anchoCm <= 0.0 || altoCm <= 0.0) return 0.0;
    return (largoCm * anchoCm * altoCm) / 1000000.0;
}

// Calcula el costo total del envío considerando tarifa base de ruta y tarifa de carga
inline double calcularCostoTotal(double tarifaBaseRuta, double pesoFacturableKg, double volumenM3, const Tarifa* tarifa = nullptr) {
    double costo = tarifaBaseRuta;
    if (tarifa != nullptr && tarifa->activo) {
        costo += (pesoFacturableKg * tarifa->precioPorKg);
        costo += (volumenM3 * tarifa->precioPorM3);
        costo += tarifa->recargoEspecial;
    } else {
        costo += (pesoFacturableKg * 10.0); // Tarifa estándar predeterminada por kg facturable
    }
    return costo;
}

// Obtiene la lista de todas las guías activas
inline std::vector<Guia> listarGuias(const char* filepath = DEFAULT_FILE_GUIAS) {
    return Persistence::leerTodos<Guia>(filepath, true);
}

// Obtiene una guía activa por su número de guía
inline bool obtenerGuiaPorNumero(const char* filepath, int numeroGuia, Guia& outGuia) {
    long index = -1;
    return Persistence::buscarGuiaPorNumero(filepath, numeroGuia, outGuia, index);
}

// Crea una nueva guía calculando peso volumétrico, peso facturable y costo total
inline bool crearGuia(const char* filepath,
                      const char* clientesFilepath,
                      const char* rutasFilepath,
                      const char* tarifasFilepath,
                      int idClienteRemitente,
                      int idClienteDestinatario,
                      int idRuta,
                      double pesoRealKg,
                      double largoCm,
                      double anchoCm,
                      double altoCm,
                      int idTarifa,
                      const std::string& tipoCarga,
                      Guia& outGuia,
                      std::string& errorMsg) {
    if (idClienteRemitente <= 0 || idClienteDestinatario <= 0 || idRuta <= 0) {
        errorMsg = "Los IDs de remitente, destinatario y ruta deben ser válidos y mayores a 0";
        return false;
    }

    if (idClienteRemitente == idClienteDestinatario) {
        errorMsg = "El cliente remitente y destinatario no pueden ser el mismo";
        return false;
    }

    if (pesoRealKg <= 0.0) {
        errorMsg = "El peso real del paquete debe ser mayor a 0 kg";
        return false;
    }

    if (largoCm <= 0.0 || anchoCm <= 0.0 || altoCm <= 0.0) {
        errorMsg = "Las dimensiones del paquete (largo, ancho, alto) deben ser mayores a 0 cm";
        return false;
    }

    if (largoCm > 100000.0 || anchoCm > 100000.0 || altoCm > 100000.0 || pesoRealKg > 100000.0) {
        errorMsg = "Las dimensiones o el peso del paquete exceden los límites máximos permitidos";
        return false;
    }

    // Validar existencia de cliente remitente
    Cliente rem;
    if (!Clientes::obtenerClientePorId(clientesFilepath, idClienteRemitente, rem)) {
        errorMsg = "El cliente remitente con ID " + std::to_string(idClienteRemitente) + " no existe o está inactivo";
        return false;
    }

    // Validar existencia de cliente destinatario
    Cliente dest;
    if (!Clientes::obtenerClientePorId(clientesFilepath, idClienteDestinatario, dest)) {
        errorMsg = "El cliente destinatario con ID " + std::to_string(idClienteDestinatario) + " no existe o está inactivo";
        return false;
    }

    // Validar existencia de la ruta
    Ruta r;
    if (!Rutas::obtenerRutaPorId(rutasFilepath, idRuta, r)) {
        errorMsg = "La ruta con ID " + std::to_string(idRuta) + " no existe o está inactiva";
        return false;
    }

    // Buscar tarifa aplicable si fue especificada
    Tarifa tarifaAplicable;
    bool tieneTarifa = false;
    if (tarifasFilepath != nullptr) {
        if (idTarifa > 0) {
            if (!Tarifas::obtenerTarifaPorId(tarifasFilepath, idTarifa, tarifaAplicable)) {
                errorMsg = "La tarifa con ID " + std::to_string(idTarifa) + " no existe o está inactiva";
                return false;
            }
            tieneTarifa = true;
        } else if (!tipoCarga.empty()) {
            std::vector<Tarifa> todasTarifas = Tarifas::listarTarifas(tarifasFilepath);
            for (const auto& t : todasTarifas) {
                if (tipoCarga == t.tipoCarga) {
                    tarifaAplicable = t;
                    tieneTarifa = true;
                    break;
                }
            }
        }
    }

    // Cálculos matemáticos (Fase 5B)
    double pVol = calcularPesoVolumetrico(largoCm, anchoCm, altoCm);
    double pFact = calcularPesoFacturable(pesoRealKg, pVol);
    double volM3 = calcularVolumenM3(largoCm, anchoCm, altoCm);
    double costoCalculado = calcularCostoTotal(r.tarifaBase, pFact, volM3, tieneTarifa ? &tarifaAplicable : nullptr);

    Guia nueva;
    std::memset(&nueva, 0, sizeof(Guia));
    nueva.numeroGuia = Persistence::obtenerSiguienteNumeroGuia(filepath);
    nueva.idClienteRemitente = idClienteRemitente;
    nueva.idClienteDestinatario = idClienteDestinatario;
    nueva.idRuta = idRuta;
    nueva.pesoRealKg = pesoRealKg;
    nueva.largoCm = largoCm;
    nueva.anchoCm = anchoCm;
    nueva.altoCm = altoCm;
    nueva.pesoVolumetricoKg = pVol;
    nueva.pesoFacturableKg = pFact;
    nueva.costoTotal = costoCalculado;
    std::strncpy(nueva.estado, "Creada", sizeof(nueva.estado) - 1);
    nueva.activo = true;

    if (!Persistence::insertar<Guia>(filepath, nueva)) {
        errorMsg = "Error de escritura en el archivo binario de guías";
        return false;
    }

    outGuia = nueva;
    return true;
}

// Sobrecarga de compatibilidad para crearGuia sin tarifasFilepath explícito
inline bool crearGuia(const char* filepath,
                      const char* clientesFilepath,
                      const char* rutasFilepath,
                      int idClienteRemitente,
                      int idClienteDestinatario,
                      int idRuta,
                      double pesoRealKg,
                      double largoCm,
                      double anchoCm,
                      double altoCm,
                      Guia& outGuia,
                      std::string& errorMsg) {
    return crearGuia(filepath, clientesFilepath, rutasFilepath, nullptr, idClienteRemitente, idClienteDestinatario, idRuta, pesoRealKg, largoCm, anchoCm, altoCm, 0, "", outGuia, errorMsg);
}

// Actualiza el estado de una guía ("Creada", "Despachada", "Entregada")
inline bool actualizarEstadoGuia(const char* filepath,
                                int numeroGuia,
                                const std::string& nuevoEstado,
                                Guia& outGuia,
                                std::string& errorMsg) {
    Guia actual;
    long index = -1;
    if (!Persistence::buscarGuiaPorNumero(filepath, numeroGuia, actual, index)) {
        errorMsg = "Guía número " + std::to_string(numeroGuia) + " no encontrada o inactiva";
        return false;
    }

    if (nuevoEstado.empty()) {
        errorMsg = "El nuevo estado no puede estar vacío";
        return false;
    }

    if (nuevoEstado != "Creada" && nuevoEstado != "Despachada" && nuevoEstado != "Entregada") {
        errorMsg = "Estado no válido. Debe ser: Creada, Despachada o Entregada";
        return false;
    }

    std::memset(actual.estado, 0, sizeof(actual.estado));
    std::strncpy(actual.estado, nuevoEstado.c_str(), sizeof(actual.estado) - 1);

    if (!Persistence::actualizarEnIndice<Guia>(filepath, index, actual)) {
        errorMsg = "Error al actualizar el estado de la guía en disco";
        return false;
    }

    outGuia = actual;
    return true;
}

// Realiza la baja lógica de una guía por su número
inline bool eliminarGuia(const char* filepath, int numeroGuia, std::string& errorMsg) {
    Guia actual;
    long index = -1;
    if (!Persistence::buscarGuiaPorNumero(filepath, numeroGuia, actual, index)) {
        errorMsg = "Guía número " + std::to_string(numeroGuia) + " no encontrada o ya inactiva";
        return false;
    }

    if (!Persistence::eliminarGuiaLogico(filepath, numeroGuia)) {
        errorMsg = "Error al realizar la baja lógica de la guía en disco";
        return false;
    }

    return true;
}

} // namespace Guias

#include "vehiculos.h"
#include "pilotos.h"

namespace Despachos {

const char* const DEFAULT_FILE_DESPACHOS = "data/despachos.dat";

struct ResultadoDespacho {
    int idVehiculo;
    int idPiloto;
    int totalGuias;
    double pesoTotalKg;
    double volumenTotalM3;
    std::string estadoVehiculo;
    std::vector<int> guiasDespachadas;
};

// Obtiene la lista de todos los despachos activos
inline std::vector<Despacho> listarDespachos(const char* filepath = DEFAULT_FILE_DESPACHOS) {
    return Persistence::leerTodos<Despacho>(filepath, true);
}

// Procesa el despacho de una lista de guías asignándolas a un vehículo y verificando límites de peso y volumen
inline bool procesarDespacho(const char* despachosFilepath,
                             const char* guiasFilepath,
                             const char* vehiculosFilepath,
                             const char* pilotosFilepath,
                             int idVehiculo,
                             int idPiloto,
                             const std::vector<int>& numerosGuias,
                             ResultadoDespacho& outResultado,
                             std::string& errorMsg) {
    if (idVehiculo <= 0) {
        errorMsg = "ID de vehículo inválido";
        return false;
    }

    if (numerosGuias.empty()) {
        errorMsg = "Debe especificar al menos una guía para despachar";
        return false;
    }

    // 1. Validar vehículo
    Vehiculo v;
    if (!Vehiculos::obtenerVehiculoPorId(vehiculosFilepath, idVehiculo, v)) {
        errorMsg = "El vehículo con ID " + std::to_string(idVehiculo) + " no existe o está inactivo";
        return false;
    }

    if (std::string(v.estado) == "En taller") {
        errorMsg = "El vehículo con ID " + std::to_string(idVehiculo) + " está en taller y no puede despacharse";
        return false;
    }

    // 2. Validar piloto (si se proporciona idPiloto > 0)
    if (idPiloto > 0) {
        Piloto p;
        if (!Pilotos::obtenerPilotoPorId(pilotosFilepath, idPiloto, p)) {
            errorMsg = "El piloto con ID " + std::to_string(idPiloto) + " no existe o está inactivo";
            return false;
        }
    }

    // 3. Validar guías y comprobar duplicados en la lista enviada
    std::vector<Guia> guiasAProcesar;
    std::vector<long> indicesGuias;
    double pesoAcumuladoBatch = 0.0;
    double volumenAcumuladoBatch = 0.0;

    for (size_t i = 0; i < numerosGuias.size(); ++i) {
        int numGuia = numerosGuias[i];

        // Verificar duplicados dentro de la misma solicitud
        for (size_t j = 0; j < i; ++j) {
            if (numerosGuias[j] == numGuia) {
                errorMsg = "La guía número " + std::to_string(numGuia) + " está duplicada en la solicitud de despacho";
                return false;
            }
        }

        Guia g;
        long index = -1;
        if (!Persistence::buscarGuiaPorNumero(guiasFilepath, numGuia, g, index)) {
            errorMsg = "La guía número " + std::to_string(numGuia) + " no existe o está inactiva";
            return false;
        }

        std::string estGuia(g.estado);
        if (estGuia == "Despachada") {
            errorMsg = "La guía número " + std::to_string(numGuia) + " ya se encuentra despachada";
            return false;
        }
        if (estGuia == "Entregada") {
            errorMsg = "La guía número " + std::to_string(numGuia) + " ya se encuentra entregada";
            return false;
        }

        double volM3 = (g.largoCm * g.anchoCm * g.altoCm) / 1000000.0;
        double pesoFact = g.pesoFacturableKg;

        pesoAcumuladoBatch += pesoFact;
        volumenAcumuladoBatch += volM3;

        guiasAProcesar.push_back(g);
        indicesGuias.push_back(index);
    }

    // 4. Validar capacidad del vehículo contra el peso y volumen acumulados
    if (pesoAcumuladoBatch > v.capacidadPesoKg) {
        errorMsg = "Despacho rechazado: El peso acumulado (" + std::to_string(pesoAcumuladoBatch) +
                   " kg) excede la capacidad máxima del vehículo (" + std::to_string(v.capacidadPesoKg) + " kg)";
        return false;
    }

    if (volumenAcumuladoBatch > v.capacidadVolumenM3) {
        errorMsg = "Despacho rechazado: El volumen acumulado (" + std::to_string(volumenAcumuladoBatch) +
                   " m3) excede la capacidad máxima del vehículo (" + std::to_string(v.capacidadVolumenM3) + " m3)";
        return false;
    }

    // 5. Aplicar cambios
    // 5a. Actualizar estado de las guías a "Despachada"
    for (size_t i = 0; i < guiasAProcesar.size(); ++i) {
        Guia g = guiasAProcesar[i];
        std::memset(g.estado, 0, sizeof(g.estado));
        std::strncpy(g.estado, "Despachada", sizeof(g.estado) - 1);
        if (!Persistence::actualizarEnIndice<Guia>(guiasFilepath, indicesGuias[i], g)) {
            errorMsg = "Error al actualizar estado de la guía número " + std::to_string(g.numeroGuia);
            return false;
        }
    }

    // 5b. Actualizar estado del vehículo a "En ruta"
    std::memset(v.estado, 0, sizeof(v.estado));
    std::strncpy(v.estado, "En ruta", sizeof(v.estado) - 1);
    if (!Persistence::actualizarPorId<Vehiculo>(vehiculosFilepath, idVehiculo, v)) {
        errorMsg = "Error al actualizar el estado del vehículo en disco";
        return false;
    }

    // 5c. Registrar despachos en data/despachos.dat
    for (size_t i = 0; i < guiasAProcesar.size(); ++i) {
        const auto& g = guiasAProcesar[i];
        Despacho d;
        std::memset(&d, 0, sizeof(Despacho));
        d.id = Persistence::obtenerSiguienteId<Despacho>(despachosFilepath);
        d.idVehiculo = idVehiculo;
        d.idPiloto = idPiloto;
        d.numeroGuia = g.numeroGuia;
        d.pesoKg = g.pesoFacturableKg;
        d.volumenM3 = (g.largoCm * g.anchoCm * g.altoCm) / 1000000.0;
        std::strncpy(d.fechaDespacho, "2026-10-08 21:00:00", sizeof(d.fechaDespacho) - 1);
        d.activo = true;

        Persistence::insertar<Despacho>(despachosFilepath, d);
    }

    // 6. Armar respuesta
    outResultado.idVehiculo = idVehiculo;
    outResultado.idPiloto = idPiloto;
    outResultado.totalGuias = static_cast<int>(guiasAProcesar.size());
    outResultado.pesoTotalKg = pesoAcumuladoBatch;
    outResultado.volumenTotalM3 = volumenAcumuladoBatch;
    outResultado.estadoVehiculo = "En ruta";
    outResultado.guiasDespachadas = numerosGuias;

    return true;
}

} // namespace Despachos

#endif // TRANSACCIONES_H
