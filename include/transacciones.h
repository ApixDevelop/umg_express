#ifndef TRANSACCIONES_H
#define TRANSACCIONES_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "structs.h"
#include "persistence.h"
#include "clientes.h"
#include "rutas.h"

namespace Guias {

const char* const DEFAULT_FILE_GUIAS = "data/guias.dat";

// Obtiene la lista de todas las guías activas
inline std::vector<Guia> listarGuias(const char* filepath = DEFAULT_FILE_GUIAS) {
    return Persistence::leerTodos<Guia>(filepath, true);
}

// Obtiene una guía activa por su número de guía
inline bool obtenerGuiaPorNumero(const char* filepath, int numeroGuia, Guia& outGuia) {
    long index = -1;
    return Persistence::buscarGuiaPorNumero(filepath, numeroGuia, outGuia, index);
}

// Crea una nueva guía validando clientes y ruta de envío
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

    if (largoCm < 0.0 || anchoCm < 0.0 || altoCm < 0.0) {
        errorMsg = "Las dimensiones del paquete (largo, ancho, alto) no pueden ser negativas";
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
    nueva.pesoVolumetricoKg = 0.0;
    nueva.pesoFacturableKg = pesoRealKg;
    nueva.costoTotal = 0.0;
    std::strncpy(nueva.estado, "Creada", sizeof(nueva.estado) - 1);
    nueva.activo = true;

    if (!Persistence::insertar<Guia>(filepath, nueva)) {
        errorMsg = "Error de escritura en el archivo binario de guías";
        return false;
    }

    outGuia = nueva;
    return true;
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
