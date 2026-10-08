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
        res["fase"] = "Fase 4B - CRUD de Rutas y Vehículos completada";
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
        res["modulo_rutas"] = "OK";
        res["modulo_vehiculos"] = "OK";
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
        std::string placa = body["placa"].s();
        std::string marcaModelo = body["marcaModelo"].s();
        double peso = body["capacidadPesoKg"].d();
        double volumen = body["capacidadVolumenM3"].d();
        std::string estado = body["estado"].s();

        if (!Vehiculos::crearVehiculo(Vehiculos::DEFAULT_FILE_VEHICULOS, placa, marcaModelo, peso, volumen, estado, nuevo, errorMsg)) {
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
        std::string placa = body["placa"].s();
        std::string marcaModelo = body["marcaModelo"].s();
        double peso = body["capacidadPesoKg"].d();
        double volumen = body["capacidadVolumenM3"].d();
        std::string estado = body["estado"].s();

        if (!Vehiculos::actualizarVehiculo(Vehiculos::DEFAULT_FILE_VEHICULOS, id, placa, marcaModelo, peso, volumen, estado, act, errorMsg)) {
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

    std::cout << "Servidor UMG Express escuchando en puerto 18080..." << std::endl;
    app.bindaddr("127.0.0.1").port(18080).multithreaded().run();

    return 0;
}
