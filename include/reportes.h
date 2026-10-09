#ifndef REPORTES_H
#define REPORTES_H

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include "structs.h"
#include "persistence.h"
#include "sucursales.h"
#include "clientes.h"
#include "rutas.h"
#include "vehiculos.h"
#include "pilotos.h"
#include "transacciones.h"

namespace Reportes {

struct DatosRastreoGuia {
    Guia guia;
    bool tieneRemitente;
    Cliente clienteRemitente;
    bool tieneDestinatario;
    Cliente clienteDestinatario;
    bool tieneRuta;
    Ruta ruta;
    bool tieneSucursalOrigen;
    Sucursal sucursalOrigen;
    bool tieneSucursalDestino;
    Sucursal sucursalDestino;
    bool despachada;
    Despacho despachoInfo;
    bool tieneVehiculo;
    Vehiculo vehiculo;
    bool tienePiloto;
    Piloto piloto;
};

// Obtiene todos los datos asociados a una guía para el reporte de rastreo (Fase 6A)
inline bool obtenerRastreoGuia(const char* guiasFilepath,
                              const char* clientesFilepath,
                              const char* rutasFilepath,
                              const char* sucursalesFilepath,
                              const char* despachosFilepath,
                              const char* vehiculosFilepath,
                              const char* pilotosFilepath,
                              int numeroGuia,
                              DatosRastreoGuia& outDatos,
                              std::string& errorMsg) {
    std::memset(&outDatos, 0, sizeof(DatosRastreoGuia));
    long index = -1;
    if (!Persistence::buscarGuiaPorNumero(guiasFilepath, numeroGuia, outDatos.guia, index)) {
        errorMsg = "Guía número " + std::to_string(numeroGuia) + " no encontrada o inactiva";
        return false;
    }

    outDatos.tieneRemitente = Clientes::obtenerClientePorId(clientesFilepath, outDatos.guia.idClienteRemitente, outDatos.clienteRemitente);
    outDatos.tieneDestinatario = Clientes::obtenerClientePorId(clientesFilepath, outDatos.guia.idClienteDestinatario, outDatos.clienteDestinatario);

    outDatos.tieneRuta = Rutas::obtenerRutaPorId(rutasFilepath, outDatos.guia.idRuta, outDatos.ruta);
    if (outDatos.tieneRuta) {
        outDatos.tieneSucursalOrigen = Sucursales::obtenerSucursalPorId(sucursalesFilepath, outDatos.ruta.idSucursalOrigen, outDatos.sucursalOrigen);
        outDatos.tieneSucursalDestino = Sucursales::obtenerSucursalPorId(sucursalesFilepath, outDatos.ruta.idSucursalDestino, outDatos.sucursalDestino);
    } else {
        outDatos.tieneSucursalOrigen = false;
        outDatos.tieneSucursalDestino = false;
    }

    // Buscar si existe información de despacho para esta guía
    outDatos.despachada = false;
    outDatos.tieneVehiculo = false;
    outDatos.tienePiloto = false;

    if (despachosFilepath != nullptr) {
        std::vector<Despacho> despachos = Despachos::listarDespachos(despachosFilepath);
        for (const auto& d : despachos) {
            if (d.numeroGuia == numeroGuia && d.activo) {
                outDatos.despachada = true;
                outDatos.despachoInfo = d;
                outDatos.tieneVehiculo = Vehiculos::obtenerVehiculoPorId(vehiculosFilepath, d.idVehiculo, outDatos.vehiculo);
                if (d.idPiloto > 0) {
                    outDatos.tienePiloto = Pilotos::obtenerPilotoPorId(pilotosFilepath, d.idPiloto, outDatos.piloto);
                }
                break;
            }
        }
    }

    return true;
}

} // namespace Reportes

#endif // REPORTES_H
