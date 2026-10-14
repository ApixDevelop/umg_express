#include "crow.h"
#include <iostream>

struct CORSMiddleware {
    struct context {};
    void before_handle(crow::request& req, crow::response& res, context& /*ctx*/) {
        if (req.method == crow::HTTPMethod::OPTIONS) {
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
            res.code = 204;
            res.end();
        }
    }
    template <typename AllContext>
    void after_handle(crow::request& /*req*/, crow::response& res, context& /*ctx*/, AllContext& /*all_ctx*/) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
    }
};

int main() {
    crow::App<CORSMiddleware> app;
    CROW_ROUTE(app, "/")([](){ return "ok"; });
    return 0;
}
