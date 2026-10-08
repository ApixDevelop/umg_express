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

#endif // TRANSACCIONES_H
