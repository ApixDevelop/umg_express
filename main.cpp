#include <iostream>
#include <vector>
#include <string>
#include "crow.h"

#include "structs.h"
#include "persistence.h"
#include "auth.h"
#include "sucursales.h"
#include "clientes.h"
#include "rutas.h"
#include "vehiculos.h"
#include "pilotos.h"
#include "tarifas.h"
#include "transacciones.h"

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
        res["fase"] = "Fase 5A - Emisión de Guías completada";
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
        res["modulo_catalogos"] = "OK";
        res["modulo_guias"] = "OK";
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

    // -------------------------------------------------------------------------
    // MÓDULO: RUTAS (/v1/rutas)
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/rutas").methods(crow::HTTPMethod::GET)([]() {
        std::vector<Ruta> lista = Rutas::listarRutas(Rutas::DEFAULT_FILE_RUTAS);
        crow::json::wvalue::list jsonList;
        for (const auto& r : lista) {
            crow::json::wvalue item;
            item["id"] = r.id;
            item["idSucursalOrigen"] = r.idSucursalOrigen;
            item["idSucursalDestino"] = r.idSucursalDestino;
            item["distanciaKm"] = r.distanciaKm;
            item["tarifaBase"] = r.tarifaBase;
            jsonList.push_back(std::move(item));
        }
        crow::json::wvalue res;
        res["success"] = true;
        res["rutas"] = std::move(jsonList);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/rutas/<int>").methods(crow::HTTPMethod::GET)([](int id) {
        Ruta r;
        crow::json::wvalue res;
        if (!Rutas::obtenerRutaPorId(Rutas::DEFAULT_FILE_RUTAS, id, r)) {
            res["success"] = false;
            res["mensaje"] = "Ruta con ID " + std::to_string(id) + " no encontrada o inactiva";
            return crow::response(404, res);
        }
        crow::json::wvalue rJson;
        rJson["id"] = r.id;
        rJson["idSucursalOrigen"] = r.idSucursalOrigen;
        rJson["idSucursalDestino"] = r.idSucursalDestino;
        rJson["distanciaKm"] = r.distanciaKm;
        rJson["tarifaBase"] = r.tarifaBase;

        res["success"] = true;
        res["ruta"] = std::move(rJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/rutas").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("idSucursalOrigen") || !body.has("idSucursalDestino") || !body.has("distanciaKm") || !body.has("tarifaBase")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: idSucursalOrigen, idSucursalDestino, distanciaKm, tarifaBase";
            return crow::response(400, res);
        }

        Ruta nueva;
        std::string errorMsg;
        int idOrigen = body["idSucursalOrigen"].i();
        int idDestino = body["idSucursalDestino"].i();
        double distancia = body["distanciaKm"].d();
        double tarifa = body["tarifaBase"].d();

        if (!Rutas::crearRuta(Rutas::DEFAULT_FILE_RUTAS, Sucursales::DEFAULT_FILE_SUCURSALES, idOrigen, idDestino, distancia, tarifa, nueva, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("no existe") != std::string::npos) ? 404 :
                       (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue rJson;
        rJson["id"] = nueva.id;
        rJson["idSucursalOrigen"] = nueva.idSucursalOrigen;
        rJson["idSucursalDestino"] = nueva.idSucursalDestino;
        rJson["distanciaKm"] = nueva.distanciaKm;
        rJson["tarifaBase"] = nueva.tarifaBase;

        res["success"] = true;
        res["mensaje"] = "Ruta creada exitosamente";
        res["ruta"] = std::move(rJson);
        return crow::response(201, res);
    });

    CROW_ROUTE(app, "/v1/rutas/<int>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, int id) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("idSucursalOrigen") || !body.has("idSucursalDestino") || !body.has("distanciaKm") || !body.has("tarifaBase")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: idSucursalOrigen, idSucursalDestino, distanciaKm, tarifaBase";
            return crow::response(400, res);
        }

        Ruta act;
        std::string errorMsg;
        int idOrigen = body["idSucursalOrigen"].i();
        int idDestino = body["idSucursalDestino"].i();
        double distancia = body["distanciaKm"].d();
        double tarifa = body["tarifaBase"].d();

        if (!Rutas::actualizarRuta(Rutas::DEFAULT_FILE_RUTAS, Sucursales::DEFAULT_FILE_SUCURSALES, id, idOrigen, idDestino, distancia, tarifa, act, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("no encontrada") != std::string::npos || errorMsg.find("no existe") != std::string::npos) ? 404 :
                       (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue rJson;
        rJson["id"] = act.id;
        rJson["idSucursalOrigen"] = act.idSucursalOrigen;
        rJson["idSucursalDestino"] = act.idSucursalDestino;
        rJson["distanciaKm"] = act.distanciaKm;
        rJson["tarifaBase"] = act.tarifaBase;

        res["success"] = true;
        res["mensaje"] = "Ruta actualizada exitosamente";
        res["ruta"] = std::move(rJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/rutas/<int>").methods(crow::HTTPMethod::DELETE)([](int id) {
        crow::json::wvalue res;
        std::string errorMsg;
        if (!Rutas::eliminarRuta(Rutas::DEFAULT_FILE_RUTAS, id, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            return crow::response(404, res);
        }

        res["success"] = true;
        res["mensaje"] = "Ruta eliminada exitosamente (baja lógica)";
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // MÓDULO: VEHÍCULOS (/v1/vehiculos)
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/vehiculos").methods(crow::HTTPMethod::GET)([]() {
        std::vector<Vehiculo> lista = Vehiculos::listarVehiculos(Vehiculos::DEFAULT_FILE_VEHICULOS);
        crow::json::wvalue::list jsonList;
        for (const auto& v : lista) {
            crow::json::wvalue item;
            item["id"] = v.id;
            item["placa"] = std::string(v.placa);
            item["marcaModelo"] = std::string(v.marcaModelo);
            item["capacidadPesoKg"] = v.capacidadPesoKg;
            item["capacidadVolumenM3"] = v.capacidadVolumenM3;
            item["estado"] = std::string(v.estado);
            jsonList.push_back(std::move(item));
        }
        crow::json::wvalue res;
        res["success"] = true;
        res["vehiculos"] = std::move(jsonList);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/vehiculos/<int>").methods(crow::HTTPMethod::GET)([](int id) {
        Vehiculo v;
        crow::json::wvalue res;
        if (!Vehiculos::obtenerVehiculoPorId(Vehiculos::DEFAULT_FILE_VEHICULOS, id, v)) {
            res["success"] = false;
            res["mensaje"] = "Vehículo con ID " + std::to_string(id) + " no encontrado o inactivo";
            return crow::response(404, res);
        }
        crow::json::wvalue vJson;
        vJson["id"] = v.id;
        vJson["placa"] = std::string(v.placa);
        vJson["marcaModelo"] = std::string(v.marcaModelo);
        vJson["capacidadPesoKg"] = v.capacidadPesoKg;
        vJson["capacidadVolumenM3"] = v.capacidadVolumenM3;
        vJson["estado"] = std::string(v.estado);

        res["success"] = true;
        res["vehiculo"] = std::move(vJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/vehiculos").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("placa") || !body.has("marcaModelo") || !body.has("capacidadPesoKg") || !body.has("capacidadVolumenM3") || !body.has("estado")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: placa, marcaModelo, capacidadPesoKg, capacidadVolumenM3, estado";
            return crow::response(400, res);
        }

        Vehiculo nuevo;
        std::string errorMsg;
        if (!Vehiculos::crearVehiculo(Vehiculos::DEFAULT_FILE_VEHICULOS, body["placa"].s(), body["marcaModelo"].s(), body["capacidadPesoKg"].d(), body["capacidadVolumenM3"].d(), body["estado"].s(), nuevo, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue vJson;
        vJson["id"] = nuevo.id;
        vJson["placa"] = std::string(nuevo.placa);
        vJson["marcaModelo"] = std::string(nuevo.marcaModelo);
        vJson["capacidadPesoKg"] = nuevo.capacidadPesoKg;
        vJson["capacidadVolumenM3"] = nuevo.capacidadVolumenM3;
        vJson["estado"] = std::string(nuevo.estado);

        res["success"] = true;
        res["mensaje"] = "Vehículo creado exitosamente";
        res["vehiculo"] = std::move(vJson);
        return crow::response(201, res);
    });

    CROW_ROUTE(app, "/v1/vehiculos/<int>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, int id) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("placa") || !body.has("marcaModelo") || !body.has("capacidadPesoKg") || !body.has("capacidadVolumenM3") || !body.has("estado")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: placa, marcaModelo, capacidadPesoKg, capacidadVolumenM3, estado";
            return crow::response(400, res);
        }

        Vehiculo act;
        std::string errorMsg;
        if (!Vehiculos::actualizarVehiculo(Vehiculos::DEFAULT_FILE_VEHICULOS, id, body["placa"].s(), body["marcaModelo"].s(), body["capacidadPesoKg"].d(), body["capacidadVolumenM3"].d(), body["estado"].s(), act, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("no encontrado") != std::string::npos) ? 404 :
                       (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue vJson;
        vJson["id"] = act.id;
        vJson["placa"] = std::string(act.placa);
        vJson["marcaModelo"] = std::string(act.marcaModelo);
        vJson["capacidadPesoKg"] = act.capacidadPesoKg;
        vJson["capacidadVolumenM3"] = act.capacidadVolumenM3;
        vJson["estado"] = std::string(act.estado);

        res["success"] = true;
        res["mensaje"] = "Vehículo actualizado exitosamente";
        res["vehiculo"] = std::move(vJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/vehiculos/<int>").methods(crow::HTTPMethod::DELETE)([](int id) {
        crow::json::wvalue res;
        std::string errorMsg;
        if (!Vehiculos::eliminarVehiculo(Vehiculos::DEFAULT_FILE_VEHICULOS, id, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            return crow::response(404, res);
        }

        res["success"] = true;
        res["mensaje"] = "Vehículo eliminado exitosamente (baja lógica)";
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // MÓDULO: PILOTOS (/v1/pilotos)
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/pilotos").methods(crow::HTTPMethod::GET)([]() {
        std::vector<Piloto> lista = Pilotos::listarPilotos(Pilotos::DEFAULT_FILE_PILOTOS);
        crow::json::wvalue::list jsonList;
        for (const auto& p : lista) {
            crow::json::wvalue item;
            item["id"] = p.id;
            item["nombre"] = std::string(p.nombre);
            item["licencia"] = std::string(p.licencia);
            item["telefono"] = std::string(p.telefono);
            jsonList.push_back(std::move(item));
        }
        crow::json::wvalue res;
        res["success"] = true;
        res["pilotos"] = std::move(jsonList);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/pilotos/<int>").methods(crow::HTTPMethod::GET)([](int id) {
        Piloto p;
        crow::json::wvalue res;
        if (!Pilotos::obtenerPilotoPorId(Pilotos::DEFAULT_FILE_PILOTOS, id, p)) {
            res["success"] = false;
            res["mensaje"] = "Piloto con ID " + std::to_string(id) + " no encontrado o inactivo";
            return crow::response(404, res);
        }
        crow::json::wvalue pJson;
        pJson["id"] = p.id;
        pJson["nombre"] = std::string(p.nombre);
        pJson["licencia"] = std::string(p.licencia);
        pJson["telefono"] = std::string(p.telefono);

        res["success"] = true;
        res["piloto"] = std::move(pJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/pilotos").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("nombre") || !body.has("licencia") || !body.has("telefono")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: nombre, licencia, telefono";
            return crow::response(400, res);
        }

        Piloto nuevo;
        std::string errorMsg;
        if (!Pilotos::crearPiloto(Pilotos::DEFAULT_FILE_PILOTOS, body["nombre"].s(), body["licencia"].s(), body["telefono"].s(), nuevo, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue pJson;
        pJson["id"] = nuevo.id;
        pJson["nombre"] = std::string(nuevo.nombre);
        pJson["licencia"] = std::string(nuevo.licencia);
        pJson["telefono"] = std::string(nuevo.telefono);

        res["success"] = true;
        res["mensaje"] = "Piloto creado exitosamente";
        res["piloto"] = std::move(pJson);
        return crow::response(201, res);
    });

    CROW_ROUTE(app, "/v1/pilotos/<int>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, int id) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("nombre") || !body.has("licencia") || !body.has("telefono")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: nombre, licencia, telefono";
            return crow::response(400, res);
        }

        Piloto act;
        std::string errorMsg;
        if (!Pilotos::actualizarPiloto(Pilotos::DEFAULT_FILE_PILOTOS, id, body["nombre"].s(), body["licencia"].s(), body["telefono"].s(), act, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("no encontrado") != std::string::npos) ? 404 :
                       (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue pJson;
        pJson["id"] = act.id;
        pJson["nombre"] = std::string(act.nombre);
        pJson["licencia"] = std::string(act.licencia);
        pJson["telefono"] = std::string(act.telefono);

        res["success"] = true;
        res["mensaje"] = "Piloto actualizado exitosamente";
        res["piloto"] = std::move(pJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/pilotos/<int>").methods(crow::HTTPMethod::DELETE)([](int id) {
        crow::json::wvalue res;
        std::string errorMsg;
        if (!Pilotos::eliminarPiloto(Pilotos::DEFAULT_FILE_PILOTOS, id, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            return crow::response(404, res);
        }

        res["success"] = true;
        res["mensaje"] = "Piloto eliminado exitosamente (baja lógica)";
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // MÓDULO: TARIFAS (/v1/tarifas)
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/tarifas").methods(crow::HTTPMethod::GET)([]() {
        std::vector<Tarifa> lista = Tarifas::listarTarifas(Tarifas::DEFAULT_FILE_TARIFAS);
        crow::json::wvalue::list jsonList;
        for (const auto& t : lista) {
            crow::json::wvalue item;
            item["id"] = t.id;
            item["tipoCarga"] = std::string(t.tipoCarga);
            item["precioPorKg"] = t.precioPorKg;
            item["precioPorM3"] = t.precioPorM3;
            item["recargoEspecial"] = t.recargoEspecial;
            jsonList.push_back(std::move(item));
        }
        crow::json::wvalue res;
        res["success"] = true;
        res["tarifas"] = std::move(jsonList);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/tarifas/<int>").methods(crow::HTTPMethod::GET)([](int id) {
        Tarifa t;
        crow::json::wvalue res;
        if (!Tarifas::obtenerTarifaPorId(Tarifas::DEFAULT_FILE_TARIFAS, id, t)) {
            res["success"] = false;
            res["mensaje"] = "Tarifa con ID " + std::to_string(id) + " no encontrada o inactiva";
            return crow::response(404, res);
        }
        crow::json::wvalue tJson;
        tJson["id"] = t.id;
        tJson["tipoCarga"] = std::string(t.tipoCarga);
        tJson["precioPorKg"] = t.precioPorKg;
        tJson["precioPorM3"] = t.precioPorM3;
        tJson["recargoEspecial"] = t.recargoEspecial;

        res["success"] = true;
        res["tarifa"] = std::move(tJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/tarifas").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("tipoCarga") || !body.has("precioPorKg") || !body.has("precioPorM3") || !body.has("recargoEspecial")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: tipoCarga, precioPorKg, precioPorM3, recargoEspecial";
            return crow::response(400, res);
        }

        Tarifa nueva;
        std::string errorMsg;
        if (!Tarifas::crearTarifa(Tarifas::DEFAULT_FILE_TARIFAS, body["tipoCarga"].s(), body["precioPorKg"].d(), body["precioPorM3"].d(), body["recargoEspecial"].d(), nueva, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue tJson;
        tJson["id"] = nueva.id;
        tJson["tipoCarga"] = std::string(nueva.tipoCarga);
        tJson["precioPorKg"] = nueva.precioPorKg;
        tJson["precioPorM3"] = nueva.precioPorM3;
        tJson["recargoEspecial"] = nueva.recargoEspecial;

        res["success"] = true;
        res["mensaje"] = "Tarifa creada exitosamente";
        res["tarifa"] = std::move(tJson);
        return crow::response(201, res);
    });

    CROW_ROUTE(app, "/v1/tarifas/<int>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, int id) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("tipoCarga") || !body.has("precioPorKg") || !body.has("precioPorM3") || !body.has("recargoEspecial")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: tipoCarga, precioPorKg, precioPorM3, recargoEspecial";
            return crow::response(400, res);
        }

        Tarifa act;
        std::string errorMsg;
        if (!Tarifas::actualizarTarifa(Tarifas::DEFAULT_FILE_TARIFAS, id, body["tipoCarga"].s(), body["precioPorKg"].d(), body["precioPorM3"].d(), body["recargoEspecial"].d(), act, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("no encontrada") != std::string::npos) ? 404 :
                       (errorMsg.find("Ya existe") != std::string::npos) ? 409 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue tJson;
        tJson["id"] = act.id;
        tJson["tipoCarga"] = std::string(act.tipoCarga);
        tJson["precioPorKg"] = act.precioPorKg;
        tJson["precioPorM3"] = act.precioPorM3;
        tJson["recargoEspecial"] = act.recargoEspecial;

        res["success"] = true;
        res["mensaje"] = "Tarifa actualizada exitosamente";
        res["tarifa"] = std::move(tJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/tarifas/<int>").methods(crow::HTTPMethod::DELETE)([](int id) {
        crow::json::wvalue res;
        std::string errorMsg;
        if (!Tarifas::eliminarTarifa(Tarifas::DEFAULT_FILE_TARIFAS, id, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            return crow::response(404, res);
        }

        res["success"] = true;
        res["mensaje"] = "Tarifa eliminada exitosamente (baja lógica)";
        return crow::response(200, res);
    });

    // -------------------------------------------------------------------------
    // MÓDULO: TRANSACCIONES / GUÍAS (/v1/transacciones/guias)
    // -------------------------------------------------------------------------
    CROW_ROUTE(app, "/v1/transacciones/guias").methods(crow::HTTPMethod::GET)([]() {
        std::vector<Guia> lista = Guias::listarGuias(Guias::DEFAULT_FILE_GUIAS);
        crow::json::wvalue::list jsonList;
        for (const auto& g : lista) {
            crow::json::wvalue item;
            item["numeroGuia"] = g.numeroGuia;
            item["idClienteRemitente"] = g.idClienteRemitente;
            item["idClienteDestinatario"] = g.idClienteDestinatario;
            item["idRuta"] = g.idRuta;
            item["pesoRealKg"] = g.pesoRealKg;
            item["largoCm"] = g.largoCm;
            item["anchoCm"] = g.anchoCm;
            item["altoCm"] = g.altoCm;
            item["estado"] = std::string(g.estado);
            jsonList.push_back(std::move(item));
        }
        crow::json::wvalue res;
        res["success"] = true;
        res["guias"] = std::move(jsonList);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/transacciones/guias/<int>").methods(crow::HTTPMethod::GET)([](int numeroGuia) {
        Guia g;
        crow::json::wvalue res;
        if (!Guias::obtenerGuiaPorNumero(Guias::DEFAULT_FILE_GUIAS, numeroGuia, g)) {
            res["success"] = false;
            res["mensaje"] = "Guía número " + std::to_string(numeroGuia) + " no encontrada o inactiva";
            return crow::response(404, res);
        }
        crow::json::wvalue gJson;
        gJson["numeroGuia"] = g.numeroGuia;
        gJson["idClienteRemitente"] = g.idClienteRemitente;
        gJson["idClienteDestinatario"] = g.idClienteDestinatario;
        gJson["idRuta"] = g.idRuta;
        gJson["pesoRealKg"] = g.pesoRealKg;
        gJson["largoCm"] = g.largoCm;
        gJson["anchoCm"] = g.anchoCm;
        gJson["altoCm"] = g.altoCm;
        gJson["estado"] = std::string(g.estado);

        res["success"] = true;
        res["guia"] = std::move(gJson);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/v1/transacciones/guias").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        crow::json::wvalue res;
        auto body = crow::json::load(req.body);
        if (!body || !body.has("idClienteRemitente") || !body.has("idClienteDestinatario") || !body.has("idRuta") || !body.has("pesoRealKg") || !body.has("largoCm") || !body.has("anchoCm") || !body.has("altoCm")) {
            res["success"] = false;
            res["mensaje"] = "Se requieren los campos: idClienteRemitente, idClienteDestinatario, idRuta, pesoRealKg, largoCm, anchoCm, altoCm";
            return crow::response(400, res);
        }

        Guia nueva;
        std::string errorMsg;
        int rem = body["idClienteRemitente"].i();
        int dest = body["idClienteDestinatario"].i();
        int rId = body["idRuta"].i();
        double peso = body["pesoRealKg"].d();
        double largo = body["largoCm"].d();
        double ancho = body["anchoCm"].d();
        double alto = body["altoCm"].d();

        if (!Guias::crearGuia(Guias::DEFAULT_FILE_GUIAS, Clientes::DEFAULT_FILE_CLIENTES, Rutas::DEFAULT_FILE_RUTAS, rem, dest, rId, peso, largo, ancho, alto, nueva, errorMsg)) {
            res["success"] = false;
            res["mensaje"] = errorMsg;
            int code = (errorMsg.find("no existe") != std::string::npos) ? 404 : 400;
            return crow::response(code, res);
        }

        crow::json::wvalue gJson;
        gJson["numeroGuia"] = nueva.numeroGuia;
        gJson["idClienteRemitente"] = nueva.idClienteRemitente;
        gJson["idClienteDestinatario"] = nueva.idClienteDestinatario;
        gJson["idRuta"] = nueva.idRuta;
        gJson["pesoRealKg"] = nueva.pesoRealKg;
        gJson["largoCm"] = nueva.largoCm;
        gJson["anchoCm"] = nueva.anchoCm;
        gJson["altoCm"] = nueva.altoCm;
        gJson["estado"] = std::string(nueva.estado);

        res["success"] = true;
        res["mensaje"] = "Guía creada exitosamente";
        res["guia"] = std::move(gJson);
        return crow::response(201, res);
    });

    std::cout << "Servidor UMG Express escuchando en puerto 18080..." << std::endl;
    app.bindaddr("127.0.0.1").port(18080).multithreaded().run();

    return 0;
}
