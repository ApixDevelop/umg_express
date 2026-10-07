#include <iostream>
#include "crow.h"
#include "structs.h"
#include "persistence.h"
#include "auth.h"

void inicializarPersistencia() {
    Persistence::asegurarArchivo("data/usuarios.dat");
    Persistence::asegurarArchivo("data/sucursales.dat");
    Persistence::asegurarArchivo("data/clientes.dat");
    Persistence::asegurarArchivo("data/rutas.dat");
    Persistence::asegurarArchivo("data/vehiculos.dat");
    Persistence::asegurarArchivo("data/pilotos.dat");
    Persistence::asegurarArchivo("data/tarifas.dat");
    Persistence::asegurarArchivo("data/guias.dat");
    
    // Crear usuario admin por defecto si el archivo está vacío
    Auth::inicializarUsuarioAdminPorDefecto(Auth::DEFAULT_FILE_USUARIOS);
    
    std::cout << "[PERSISTENCIA] Archivos .dat e infraestructura de autenticación inicializados." << std::endl;
}

int main() {
    // Inicializar capas de almacenamiento binario .dat y usuario por defecto
    inicializarPersistencia();

    crow::SimpleApp app;

    // -------------------------------------------------------------------------
    // Ruta de bienvenida / prueba inicial
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/")([]() {
        crow::json::wvalue res;
        res["estado"] = "ok";
        res["mensaje"] = "Servidor UMG Express API en funcionamiento";
        res["version"] = "1.0.0";
        res["fase"] = "Fase 3 - Autenticación y Usuarios completada";
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // Endpoint de salud del servidor
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/health")([]() {
        crow::json::wvalue res;
        res["status"] = "UP";
        res["servicio"] = "UMG Express API";
        res["modulo_auth"] = "OK";
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // POST /v1/auth/login
    // Autenticación de credenciales de usuario
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/auth/login").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body) {
            res["success"] = false;
            res["mensaje"] = "Cuerpo de solicitud JSON inválido o malformado";
            return crow::response(400, res);
        }

        if (!body.has("username") || !body.has("password")) {
            res["success"] = false;
            res["mensaje"] = "Campos 'username' y 'password' son requeridos";
            return crow::response(400, res);
        }

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        Usuario u;
        std::string errorMsg;
        if (!Auth::autenticarUsuario(Auth::DEFAULT_FILE_USUARIOS, username, password, u, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            return crow::response(401, res);
        }

        crow::json::wvalue uJson;
        uJson["id"] = u.id;
        uJson["username"] = std::string(u.username);
        uJson["nombre"] = std::string(u.nombre);
        uJson["rol"] = std::string(u.rol);

        res["success"] = true;
        res["mensaje"] = "Autenticación exitosa";
        res["usuario"] = std::move(uJson);
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // POST /v1/auth/usuarios
    // Registro de un nuevo usuario
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/auth/usuarios").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body) {
            res["success"] = false;
            res["mensaje"] = "Cuerpo de solicitud JSON inválido o malformado";
            return crow::response(400, res);
        }

        if (!body.has("username") || !body.has("password") || !body.has("nombre") || !body.has("rol")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: username, password, nombre, rol";
            return crow::response(400, res);
        }

        std::string username = body["username"].s();
        std::string password = body["password"].s();
        std::string nombre = body["nombre"].s();
        std::string rol = body["rol"].s();

        Usuario nuevoU;
        std::string errorMsg;
        if (!Auth::registrarUsuario(Auth::DEFAULT_FILE_USUARIOS, username, password, nombre, rol, nuevoU, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            // Si fue por duplicado devolver 409, de lo contrario 400
            int code = (errorMsg.find("ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue uJson;
        uJson["id"] = nuevoU.id;
        uJson["username"] = std::string(nuevoU.username);
        uJson["nombre"] = std::string(nuevoU.nombre);
        uJson["rol"] = std::string(nuevoU.rol);

        res["success"] = true;
        res["mensaje"] = "Usuario creado exitosamente";
        res["usuario"] = std::move(uJson);
        return crow::response(201, res);
    });

    std::cout << "Servidor UMG Express escuchando en puerto 18080..." << std::endl;
    app.bindaddr("127.0.0.1").port(18080).multithreaded().run();

    return 0;
}
