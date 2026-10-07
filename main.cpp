#include <iostream>
#include "crow.h"
#include "structs.h"
#include "persistence.h"

void inicializarPersistencia() {
    Persistence::asegurarArchivo("data/usuarios.dat");
    Persistence::asegurarArchivo("data/sucursales.dat");
    Persistence::asegurarArchivo("data/clientes.dat");
    Persistence::asegurarArchivo("data/rutas.dat");
    Persistence::asegurarArchivo("data/vehiculos.dat");
    Persistence::asegurarArchivo("data/pilotos.dat");
    Persistence::asegurarArchivo("data/tarifas.dat");
    Persistence::asegurarArchivo("data/guias.dat");
    std::cout << "[PERSISTENCIA] Archivos .dat verificados e inicializados correctamente." << std::endl;
}

int main() {
    // Inicializar capas de almacenamiento binario .dat
    inicializarPersistencia();

    crow::SimpleApp app;

    // Ruta de bienvenida / prueba inicial
    CROW_ROUTE(app, "/")([]() {
        crow::json::wvalue res;
        res["estado"] = "ok";
        res["mensaje"] = "Servidor UMG Express API en funcionamiento";
        res["version"] = "1.0.0";
        res["fase"] = "Fase 2 - Estructuras y Persistencia completada";
        return crow::response(200, res);
    });

    // Endpoint de verificación de salud (healthcheck)
    CROW_ROUTE(app, "/v1/health")([]() {
        crow::json::wvalue res;
        res["status"] = "UP";
        res["servicio"] = "UMG Express API";
        res["persistencia"] = "OK (.dat archivos binarios inicializados)";
        return crow::response(200, res);
    });

    std::cout << "Servidor UMG Express escuchando en puerto 18080..." << std::endl;
    app.bindaddr("127.0.0.1").port(18080).multithreaded().run();

    return 0;
}
