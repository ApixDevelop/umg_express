#include <iostream>
#include "crow.h"

int main() {
    crow::SimpleApp app;

    // Ruta de bienvenida / prueba inicial
    CROW_ROUTE(app, "/")([]() {
        crow::json::wvalue res;
        res["estado"] = "ok";
        res["mensaje"] = "Servidor UMG Express API en funcionamiento";
        res["version"] = "1.0.0";
        return crow::response(200, res);
    });

    // Endpoint de verificación de salud (healthcheck)
    CROW_ROUTE(app, "/v1/health")([]() {
        crow::json::wvalue res;
        res["status"] = "UP";
        res["servicio"] = "UMG Express API";
        return crow::response(200, res);
    });

    std::cout << "Servidor UMG Express escuchando en puerto 18080..." << std::endl;
    app.bindaddr("127.0.0.1").port(18080).multithreaded().run();

    return 0;
}
