#include <iostream>
#include <vector>
#include <string>
#include "crow.h"

#include "structs.h"
#include "persistence.h"
#include "auth.h"
#include "sucursales.h"
#include "clientes.h"

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
    
    std::cout << "[PERSISTENCIA] Archivos .dat e infraestructura inicializada." << std::endl;
}

int main() {
    inicializarPersistencia();

    crow::SimpleApp app;

    // -------------------------------------------------------------------------
    // Ruta de bienvenida / estado general
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/")([]() {
        crow::json::wvalue res;
        res["estado"] = "ok";
        res["mensaje"] = "Servidor UMG Express API en funcionamiento";
        res["version"] = "1.0.0";
        res["fase"] = "Fase 4A - CRUD de Sucursales y Clientes completada";
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
        res["modulo_sucursales"] = "OK";
        res["modulo_clientes"] = "OK";
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // MÓDULO: AUTENTICACIÓN
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/auth/login").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password")) {
            res["success"] = false;
            res["mensaje"] = "Campos 'username' y 'password' son requeridos";
            return crow::response(400, res);
        }

        Usuario u;
        std::string errorMsg;
        if (!Auth::autenticarUsuario(Auth::DEFAULT_FILE_USUARIOS, body["username"].s(), body["password"].s(), u, errorMsg)) {
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

    CROW_ROUTE(app, "/v1/auth/usuarios").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("username") || !body.has("password") || !body.has("nombre") || !body.has("rol")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: username, password, nombre, rol";
            return crow::response(400, res);
        }

        Usuario nuevoU;
        std::string errorMsg;
        if (!Auth::registrarUsuario(Auth::DEFAULT_FILE_USUARIOS, body["username"].s(), body["password"].s(), body["nombre"].s(), body["rol"].s(), nuevoU, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
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

    // -------------------------------------------------------------------------
    // MÓDULO: SUCURSALES (/v1/sucursales)
    // -------------------------------------------------------------------------
    
    // GET /v1/sucursales - Listar todas las sucursales activas
    CROW_ROUTE(app, "/v1/sucursales").methods(crow::HTTPMethod::GET)([]() {
        std::vector<Sucursal> lista = Sucursales::listarSucursales(Sucursales::DEFAULT_FILE_SUCURSALES);
        crow::json::wvalue::list jsonList;
        for (const auto& s : lista) {
            crow::json::wvalue item;
            item["id"] = s.id;
            item["nombre"] = std::string(s.nombre);
            item["direccion"] = std::string(s.direccion);
            item["telefono"] = std::string(s.telefono);
            jsonList.push_back(std::move(item));
        }
        crow::json::wvalue res;
        res["success"] = true;
        res["sucursales"] = std::move(jsonList);
        return crow::response(200, res);
    });

    // GET /v1/sucursales/<int> - Obtener sucursal por ID
    CROW_ROUTE(app, "/v1/sucursales/<int>").methods(crow::HTTPMethod::GET)([](int id) {
        Sucursal s;
        crow::json::wvalue res;
        if (!Sucursales::obtenerSucursalPorId(Sucursales::DEFAULT_FILE_SUCURSALES, id, s)) {
            res["success"] = false;
            res["mensaje"] = "Sucursal con ID " + std::to_string(id) + " no encontrada o inactiva";
            return crow::response(404, res);
        }
        crow::json::wvalue sJson;
        sJson["id"] = s.id;
        sJson["nombre"] = std::string(s.nombre);
        sJson["direccion"] = std::string(s.direccion);
        sJson["telefono"] = std::string(s.telefono);

        res["success"] = true;
        res["sucursal"] = std::move(sJson);
        return crow::response(200, res);
    });

    // POST /v1/sucursales - Crear nueva sucursal
    CROW_ROUTE(app, "/v1/sucursales").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("nombre") || !body.has("direccion") || !body.has("telefono")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: nombre, direccion, telefono";
            return crow::response(400, res);
        }

        Sucursal nueva;
        std::string errorMsg;
        if (!Sucursales::crearSucursal(Sucursales::DEFAULT_FILE_SUCURSALES, body["nombre"].s(), body["direccion"].s(), body["telefono"].s(), nueva, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue sJson;
        sJson["id"] = nueva.id;
        sJson["nombre"] = std::string(nueva.nombre);
        sJson["direccion"] = std::string(nueva.direccion);
        sJson["telefono"] = std::string(nueva.telefono);

        res["success"] = true;
        res["mensaje"] = "Sucursal creada exitosamente";
        res["sucursal"] = std::move(sJson);
        return crow::response(201, res);
    });

    // PUT /v1/sucursales/<int> - Actualizar sucursal por ID
    CROW_ROUTE(app, "/v1/sucursales/<int>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, int id) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("nombre") || !body.has("direccion") || !body.has("telefono")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: nombre, direccion, telefono";
            return crow::response(400, res);
        }

        Sucursal act;
        std::string errorMsg;
        if (!Sucursales::actualizarSucursal(Sucursales::DEFAULT_FILE_SUCURSALES, id, body["nombre"].s(), body["direccion"].s(), body["telefono"].s(), act, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("no encontrada") != std::string::npos) ? 404 :
                       (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue sJson;
        sJson["id"] = act.id;
        sJson["nombre"] = std::string(act.nombre);
        sJson["direccion"] = std::string(act.direccion);
        sJson["telefono"] = std::string(act.telefono);

        res["success"] = true;
        res["mensaje"] = "Sucursal actualizada exitosamente";
        res["sucursal"] = std::move(sJson);
        return crow::response(200, res);
    });

    // DELETE /v1/sucursales/<int> - Baja lógica de sucursal
    CROW_ROUTE(app, "/v1/sucursales/<int>").methods(crow::HTTPMethod::DELETE)([](int id) {
        crow::json::wvalue res;
        std::string errorMsg;
        if (!Sucursales::eliminarSucursal(Sucursales::DEFAULT_FILE_SUCURSALES, id, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            return crow::response(404, res);
        }

        res["success"] = true;
        res["mensaje"] = "Sucursal eliminada exitosamente (baja lógica)";
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // MÓDULO: CLIENTES (/v1/clientes)
    // -------------------------------------------------------------------------
    
    // GET /v1/clientes - Listar todos los clientes activos
    CROW_ROUTE(app, "/v1/clientes").methods(crow::HTTPMethod::GET)([]() {
        std::vector<Cliente> lista = Clientes::listarClientes(Clientes::DEFAULT_FILE_CLIENTES);
        crow::json::wvalue::list jsonList;
        for (const auto& c : lista) {
            crow::json::wvalue item;
            item["id"] = c.id;
            item["nit"] = std::string(c.nit);
            item["nombre"] = std::string(c.nombre);
            item["direccion"] = std::string(c.direccion);
            item["telefono"] = std::string(c.telefono);
            jsonList.push_back(std::move(item));
        }
        crow::json::wvalue res;
        res["success"] = true;
        res["clientes"] = std::move(jsonList);
        return crow::response(200, res);
    });

    // GET /v1/clientes/<int> - Obtener cliente por ID
    CROW_ROUTE(app, "/v1/clientes/<int>").methods(crow::HTTPMethod::GET)([](int id) {
        Cliente c;
        crow::json::wvalue res;
        if (!Clientes::obtenerClientePorId(Clientes::DEFAULT_FILE_CLIENTES, id, c)) {
            res["success"] = false;
            res["mensaje"] = "Cliente con ID " + std::to_string(id) + " no encontrado o inactivo";
            return crow::response(404, res);
        }
        crow::json::wvalue cJson;
        cJson["id"] = c.id;
        cJson["nit"] = std::string(c.nit);
        cJson["nombre"] = std::string(c.nombre);
        cJson["direccion"] = std::string(c.direccion);
        cJson["telefono"] = std::string(c.telefono);

        res["success"] = true;
        res["cliente"] = std::move(cJson);
        return crow::response(200, res);
    });

    // POST /v1/clientes - Crear nuevo cliente
    CROW_ROUTE(app, "/v1/clientes").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("nit") || !body.has("nombre") || !body.has("direccion") || !body.has("telefono")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: nit, nombre, direccion, telefono";
            return crow::response(400, res);
        }

        Cliente nuevo;
        std::string errorMsg;
        if (!Clientes::crearCliente(Clientes::DEFAULT_FILE_CLIENTES, body["nit"].s(), body["nombre"].s(), body["direccion"].s(), body["telefono"].s(), nuevo, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue cJson;
        cJson["id"] = nuevo.id;
        cJson["nit"] = std::string(nuevo.nit);
        cJson["nombre"] = std::string(nuevo.nombre);
        cJson["direccion"] = std::string(nuevo.direccion);
        cJson["telefono"] = std::string(nuevo.telefono);

        res["success"] = true;
        res["mensaje"] = "Cliente creado exitosamente";
        res["cliente"] = std::move(cJson);
        return crow::response(201, res);
    });

    // PUT /v1/clientes/<int> - Actualizar cliente por ID
    CROW_ROUTE(app, "/v1/clientes/<int>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, int id) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("nit") || !body.has("nombre") || !body.has("direccion") || !body.has("telefono")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: nit, nombre, direccion, telefono";
            return crow::response(400, res);
        }

        Cliente act;
        std::string errorMsg;
        if (!Clientes::actualizarCliente(Clientes::DEFAULT_FILE_CLIENTES, id, body["nit"].s(), body["nombre"].s(), body["direccion"].s(), body["telefono"].s(), act, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("no encontrado") != std::string::npos) ? 404 :
                       (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue cJson;
        cJson["id"] = act.id;
        cJson["nit"] = std::string(act.nit);
        cJson["nombre"] = std::string(act.nombre);
        cJson["direccion"] = std::string(act.direccion);
        cJson["telefono"] = std::string(act.telefono);

        res["success"] = true;
        res["mensaje"] = "Cliente actualizado exitosamente";
        res["cliente"] = std::move(cJson);
        return crow::response(200, res);
    });

    // DELETE /v1/clientes/<int> - Baja lógica de cliente
    CROW_ROUTE(app, "/v1/clientes/<int>").methods(crow::HTTPMethod::DELETE)([](int id) {
        crow::json::wvalue res;
        std::string errorMsg;
        if (!Clientes::eliminarCliente(Clientes::DEFAULT_FILE_CLIENTES, id, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            return crow::response(404, res);
        }

        res["success"] = true;
        res["mensaje"] = "Cliente eliminado exitosamente (baja lógica)";
        return crow::response(200, res);
    });

    std::cout << "Servidor UMG Express escuchando en puerto 18080..." << std::endl;
    app.bindaddr("127.0.0.1").port(18080).multithreaded().run();

    return 0;
}
