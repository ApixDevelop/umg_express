#ifndef REPORTES_H
#define REPORTES_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cstring>

#include "structs.h"
#include "persistence.h"
#include "sucursales.h"
#include "clientes.h"
#include "rutas.h"
#include "vehiculos.h"
#include "pilotos.h"
#include "transacciones.h"

#include <set>
#include <algorithm>

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

struct GuiaManifiesto {
    Guia guia;
    double volumenM3;
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
};

struct DatosManifiestoVehiculo {
    Vehiculo vehiculo;
    bool tienePiloto;
    Piloto piloto;
    bool tieneRutaPrincipal;
    Ruta rutaPrincipal;
    bool tieneSucursalOrigen;
    Sucursal sucursalOrigen;
    bool tieneSucursalDestino;
    Sucursal sucursalDestino;
    std::vector<GuiaManifiesto> guias;
    int totalGuias;
    double pesoTotalKg;
    double volumenTotalM3;
    char fechaDespacho[30];
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

// Obtiene el manifiesto de carga y guías para un vehículo específico (Fase 6B)
inline bool obtenerManifiestoVehiculo(const char* vehiculosFilepath,
                                     const char* despachosFilepath,
                                     const char* guiasFilepath,
                                     const char* pilotosFilepath,
                                     const char* rutasFilepath,
                                     const char* sucursalesFilepath,
                                     const char* clientesFilepath,
                                     int idVehiculo,
                                     DatosManifiestoVehiculo& outDatos,
                                     std::string& errorMsg) {
    outDatos.tienePiloto = false;
    outDatos.tieneRutaPrincipal = false;
    outDatos.tieneSucursalOrigen = false;
    outDatos.tieneSucursalDestino = false;
    outDatos.guias.clear();
    outDatos.totalGuias = 0;
    outDatos.pesoTotalKg = 0.0;
    outDatos.volumenTotalM3 = 0.0;
    std::memset(outDatos.fechaDespacho, 0, sizeof(outDatos.fechaDespacho));

    // 1. Obtener vehículo
    if (!Vehiculos::obtenerVehiculoPorId(vehiculosFilepath, idVehiculo, outDatos.vehiculo)) {
        errorMsg = "Vehículo ID " + std::to_string(idVehiculo) + " no encontrado o inactivo";
        return false;
    }

    // 2. Obtener despachos asociados a este vehículo
    if (despachosFilepath == nullptr) {
        return true; // Vehículo existe pero sin despachos
    }

    std::vector<Despacho> despachos = Despachos::listarDespachos(despachosFilepath);
    std::set<int> guiasProcesadas;

    for (const auto& d : despachos) {
        if (d.idVehiculo == idVehiculo && d.activo) {
            // Guardar fecha de despacho si está disponible
            if (outDatos.fechaDespacho[0] == '\0' && d.fechaDespacho[0] != '\0') {
                std::strncpy(outDatos.fechaDespacho, d.fechaDespacho, sizeof(outDatos.fechaDespacho) - 1);
            }

            // Obtener piloto si aún no se tiene uno
            if (!outDatos.tienePiloto && d.idPiloto > 0) {
                outDatos.tienePiloto = Pilotos::obtenerPilotoPorId(pilotosFilepath, d.idPiloto, outDatos.piloto);
            }

            // Evitar duplicados de guía en el manifiesto
            if (guiasProcesadas.find(d.numeroGuia) != guiasProcesadas.end()) {
                continue;
            }
            guiasProcesadas.insert(d.numeroGuia);

            // Obtener guía de la persistencia
            Guia g;
            long index = -1;
            if (Persistence::buscarGuiaPorNumero(guiasFilepath, d.numeroGuia, g, index)) {
                GuiaManifiesto gm;
                std::memset(&gm, 0, sizeof(GuiaManifiesto));
                gm.guia = g;
                gm.volumenM3 = (g.largoCm * g.anchoCm * g.altoCm) / 1000000.0;

                gm.tieneRemitente = Clientes::obtenerClientePorId(clientesFilepath, g.idClienteRemitente, gm.clienteRemitente);
                gm.tieneDestinatario = Clientes::obtenerClientePorId(clientesFilepath, g.idClienteDestinatario, gm.clienteDestinatario);

                gm.tieneRuta = Rutas::obtenerRutaPorId(rutasFilepath, g.idRuta, gm.ruta);
                if (gm.tieneRuta) {
                    gm.tieneSucursalOrigen = Sucursales::obtenerSucursalPorId(sucursalesFilepath, gm.ruta.idSucursalOrigen, gm.sucursalOrigen);
                    gm.tieneSucursalDestino = Sucursales::obtenerSucursalPorId(sucursalesFilepath, gm.ruta.idSucursalDestino, gm.sucursalDestino);

                    if (!outDatos.tieneRutaPrincipal) {
                        outDatos.tieneRutaPrincipal = true;
                        outDatos.rutaPrincipal = gm.ruta;
                        outDatos.tieneSucursalOrigen = gm.tieneSucursalOrigen;
                        outDatos.sucursalOrigen = gm.sucursalOrigen;
                        outDatos.tieneSucursalDestino = gm.tieneSucursalDestino;
                        outDatos.sucursalDestino = gm.sucursalDestino;
                    }
                }

                outDatos.guias.push_back(gm);
                outDatos.totalGuias++;
                outDatos.pesoTotalKg += g.pesoFacturableKg;
                outDatos.volumenTotalM3 += gm.volumenM3;
            }
        }
    }

    return true;
}

#include <map>

struct ResumenEstadoLiquidacion {
    std::string estado;
    int cantidadGuias;
    double importeTotal;
};

struct ResumenRutaLiquidacion {
    int idRuta;
    int cantidadGuias;
    double importeTotal;
};

struct ResumenSucursalLiquidacion {
    int idSucursal;
    std::string nombre;
    int cantidadGuias;
    double importeTotal;
};

struct GuiaLiquidacion {
    Guia guia;
    bool tieneRuta;
    Ruta ruta;
    bool tieneSucursalOrigen;
    Sucursal sucursalOrigen;
};

struct DatosLiquidacion {
    int totalGuiasConsideradas;
    int totalGuiasInactivas;
    double importeTotalFacturado;
    double cobradoConfirmado;
    std::vector<ResumenEstadoLiquidacion> resumenPorEstado;
    std::vector<ResumenRutaLiquidacion> resumenPorRuta;
    std::vector<ResumenSucursalLiquidacion> resumenPorSucursalOrigen;
    std::vector<GuiaLiquidacion> guias;
};

// Obtiene el reporte consolidado de liquidación financiera y operativa (Fase 6C)
inline bool obtenerReporteLiquidacion(const char* guiasFilepath,
                                      const char* rutasFilepath,
                                      const char* sucursalesFilepath,
                                      DatosLiquidacion& outDatos,
                                      std::string& errorMsg) {
    outDatos.totalGuiasConsideradas = 0;
    outDatos.totalGuiasInactivas = 0;
    outDatos.importeTotalFacturado = 0.0;
    outDatos.cobradoConfirmado = 0.0; // UMG Express no tiene tabla de pagos; cobradoConfirmado es 0.0
    outDatos.resumenPorEstado.clear();
    outDatos.resumenPorRuta.clear();
    outDatos.resumenPorSucursalOrigen.clear();
    outDatos.guias.clear();

    if (guiasFilepath == nullptr) {
        return true;
    }

    std::vector<Guia> todasGuias = Persistence::leerTodos<Guia>(guiasFilepath, false);

    std::map<std::string, ResumenEstadoLiquidacion> mapEstados;
    std::map<int, ResumenRutaLiquidacion> mapRutas;
    std::map<int, ResumenSucursalLiquidacion> mapSucursales;

    for (const auto& g : todasGuias) {
        if (!g.activo) {
            outDatos.totalGuiasInactivas++;
            continue;
        }

        outDatos.totalGuiasConsideradas++;
        outDatos.importeTotalFacturado += g.costoTotal;

        // 1. Agrupar por Estado
        std::string est = std::string(g.estado);
        if (est.empty()) est = "Desconocido";
        mapEstados[est].estado = est;
        mapEstados[est].cantidadGuias++;
        mapEstados[est].importeTotal += g.costoTotal;

        // 2. Agrupar por Ruta
        mapRutas[g.idRuta].idRuta = g.idRuta;
        mapRutas[g.idRuta].cantidadGuias++;
        mapRutas[g.idRuta].importeTotal += g.costoTotal;

        // 3. Obtener Ruta y Sucursal de Origen
        GuiaLiquidacion gl;
        gl.guia = g;
        gl.tieneRuta = (rutasFilepath != nullptr) && Rutas::obtenerRutaPorId(rutasFilepath, g.idRuta, gl.ruta);

        if (gl.tieneRuta && sucursalesFilepath != nullptr) {
            gl.tieneSucursalOrigen = Sucursales::obtenerSucursalPorId(sucursalesFilepath, gl.ruta.idSucursalOrigen, gl.sucursalOrigen);
            if (gl.tieneSucursalOrigen) {
                int idSuc = gl.sucursalOrigen.id;
                mapSucursales[idSuc].idSucursal = idSuc;
                mapSucursales[idSuc].nombre = std::string(gl.sucursalOrigen.nombre);
                mapSucursales[idSuc].cantidadGuias++;
                mapSucursales[idSuc].importeTotal += g.costoTotal;
            }
        } else {
            gl.tieneSucursalOrigen = false;
        }

        outDatos.guias.push_back(gl);
    }

    for (const auto& pair : mapEstados) {
        outDatos.resumenPorEstado.push_back(pair.second);
    }

    for (const auto& pair : mapRutas) {
        outDatos.resumenPorRuta.push_back(pair.second);
    }

    for (const auto& pair : mapSucursales) {
        outDatos.resumenPorSucursalOrigen.push_back(pair.second);
    }

    return true;
}

} // namespace Reportes

#endif // REPORTES_H
