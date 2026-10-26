#include <cassert>
#include <iostream>
#include <vector>
#include <string>

#include "arbol_bst.h"
#include "structs.h"

void testInsercionYBusqueda() {
    std::cout << "[TEST] 1. Inserción y Búsqueda Binaria Recursiva..." << std::endl;
    Arboles::ArbolBST<int> arbol;

    assert(arbol.estaVacio());
    assert(arbol.totalNodos() == 0);
    assert(arbol.altura() == 0);

    // Insertar claves desordenadas
    // Árbol resultante:
    //         50
    //       /    \
    //     30      70
    //    /  \    /  \
    //   20  40  60  80
    assert(arbol.insertar(50, 500));
    assert(arbol.insertar(30, 300));
    assert(arbol.insertar(70, 700));
    assert(arbol.insertar(20, 200));
    assert(arbol.insertar(40, 400));
    assert(arbol.insertar(60, 600));
    assert(arbol.insertar(80, 800));

    // Validar rechazo de clave duplicada
    assert(!arbol.insertar(50, 999));
    assert(!arbol.insertar(30, 999));

    assert(!arbol.estaVacio());
    assert(arbol.totalNodos() == 7);
    assert(arbol.altura() == 3);

    // Búsqueda de la raíz (debe tomar 1 comparación)
    int dato = 0;
    int comps = 0;
    assert(arbol.buscar(50, dato, comps));
    assert(dato == 500);
    assert(comps == 1);

    // Búsqueda de un nodo hoja (debe tomar 3 comparaciones: 50 -> 30 -> 20)
    assert(arbol.buscar(20, dato, comps));
    assert(dato == 200);
    assert(comps == 3);

    // Búsqueda de un nodo hoja derecho (50 -> 70 -> 80)
    assert(arbol.buscar(80, dato, comps));
    assert(dato == 800);
    assert(comps == 3);

    // Búsqueda de clave inexistente
    assert(!arbol.buscar(99, dato, comps));
    assert(!arbol.buscar(10, dato, comps));

    std::cout << "   -> Inserción y búsqueda validadas con éxito." << std::endl;
}

void testRecorridos() {
    std::cout << "[TEST] 2. Recorridos In-Order, Pre-Order y Post-Order..." << std::endl;
    Arboles::ArbolBST<std::string> arbol;

    // Insertar claves: 50, 25, 75, 10, 30
    arbol.insertar(50, "Cincuenta");
    arbol.insertar(25, "Veinticinco");
    arbol.insertar(75, "Setenta y cinco");
    arbol.insertar(10, "Diez");
    arbol.insertar(30, "Treinta");

    // 1. Recorrido In-Order (Izquierda -> Raíz -> Derecha): DEBE ESTAR ORDENADO
    std::vector<int> inOrder = arbol.obtenerInOrderClaves();
    assert(inOrder.size() == 5);
    std::vector<int> esperadoInOrder = {10, 25, 30, 50, 75};
    assert(inOrder == esperadoInOrder);

    // 2. Recorrido Pre-Order (Raíz -> Izquierda -> Derecha)
    std::vector<int> preOrder = arbol.obtenerPreOrderClaves();
    std::vector<int> esperadoPreOrder = {50, 25, 10, 30, 75};
    assert(preOrder == esperadoPreOrder);

    // 3. Recorrido Post-Order (Izquierda -> Derecha -> Raíz)
    std::vector<int> postOrder = arbol.obtenerPostOrderClaves();
    std::vector<int> esperadoPostOrder = {10, 30, 25, 75, 50};
    assert(postOrder == esperadoPostOrder);

    std::cout << "   -> Propiedad de ordenamiento In-Order comprobada matemáticamente." << std::endl;
}

void testEstructuraJSON() {
    std::cout << "[TEST] 3. Serialización de Estructura de Árbol para Frontend..." << std::endl;
    Arboles::ArbolBST<int> arbol;
    arbol.insertar(2, 20);
    arbol.insertar(1, 10);
    arbol.insertar(3, 30);

    std::string json = arbol.exportarEstructuraJSON();
    assert(json.find("\"clave\":2") != std::string::npos);
    assert(json.find("\"clave\":1") != std::string::npos);
    assert(json.find("\"clave\":3") != std::string::npos);

    std::cout << "   -> JSON exportado: " << json << std::endl;
}

void testIntegracionGuias() {
    std::cout << "[TEST] 4. Indexación en Árbol Binario con Guías reales..." << std::endl;
    Arboles::ArbolBST<Guia> arbolGuias;

    Guia g1;
    g1.numeroGuia = 1005;
    g1.costoTotal = 150.0;
    std::strncpy(g1.estado, "Creada", sizeof(g1.estado));

    Guia g2;
    g2.numeroGuia = 1001;
    g2.costoTotal = 80.0;
    std::strncpy(g2.estado, "Despachada", sizeof(g2.estado));

    Guia g3;
    g3.numeroGuia = 1010;
    g3.costoTotal = 220.0;
    std::strncpy(g3.estado, "Entregada", sizeof(g3.estado));

    arbolGuias.insertar(g1.numeroGuia, g1);
    arbolGuias.insertar(g2.numeroGuia, g2);
    arbolGuias.insertar(g3.numeroGuia, g3);

    assert(arbolGuias.totalNodos() == 3);
    assert(arbolGuias.altura() == 2);

    Guia encontrada;
    int comps = 0;
    assert(arbolGuias.buscar(1001, encontrada, comps));
    assert(encontrada.costoTotal == 80.0);
    assert(std::string(encontrada.estado) == "Despachada");

    // Verificar recorrido ordenado por número de guía
    std::vector<Guia> listaOrdenada = arbolGuias.obtenerInOrderDatos();
    assert(listaOrdenada.size() == 3);
    assert(listaOrdenada[0].numeroGuia == 1001);
    assert(listaOrdenada[1].numeroGuia == 1005);
    assert(listaOrdenada[2].numeroGuia == 1010);

    std::cout << "   -> Integración con Guía validada con éxito." << std::endl;
}

int main() {
    std::cout << "=================================================" << std::endl;
    std::cout << "   EJECUTANDO PRUEBAS DE ÁRBOL BINARIO (BST)    " << std::endl;
    std::cout << "=================================================" << std::endl;

    testInsercionYBusqueda();
    testRecorridos();
    testEstructuraJSON();
    testIntegracionGuias();

    std::cout << "=================================================" << std::endl;
    std::cout << "   TODAS LAS PRUEBAS DE ÁRBOL BST PASARON EXITOSAMENTE! " << std::endl;
    std::cout << "=================================================" << std::endl;
    return 0;
}
