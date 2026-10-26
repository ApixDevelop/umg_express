#ifndef ARBOL_BST_H
#define ARBOL_BST_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

#include "structs.h"
#include "persistence.h"

namespace Arboles {

// =============================================================================
// ESTRUCTURA: Nodo del Árbol Binario de Búsqueda (BST)
// Matemática Discreta: Un árbol binario es un grafo conexo, acíclico y dirigido,
// donde cada vértice tiene a lo sumo dos hijos (izquierdo y derecho).
// En un BST se cumple la propiedad de orden:
//   Para todo nodo u en subárbol izquierdo: u.clave < raiz.clave
//   Para todo nodo v en subárbol derecho:   v.clave > raiz.clave
// =============================================================================
template <typename T>
struct NodoBST {
    int clave;
    T dato;
    NodoBST<T>* izquierdo;
    NodoBST<T>* derecho;

    NodoBST(int _clave, const T& _dato)
        : clave(_clave), dato(_dato), izquierdo(nullptr), derecho(nullptr) {}
};

// =============================================================================
// FUNCIONES RECURSIVAS PURAS
// =============================================================================

// Inserción Recursiva:
// Caso Base: Si el subárbol está vacío (nullptr), crea un nuevo nodo en esa posición.
// Paso Inductivo/Recursivo: Si la clave es menor, desciende a la izquierda;
//                           si es mayor, desciende a la derecha.
template <typename T>
bool insertarRecursivo(NodoBST<T>*& raiz, int clave, const T& dato) {
    if (raiz == nullptr) {
        raiz = new NodoBST<T>(clave, dato);
        return true;
    }

    if (clave < raiz->clave) {
        return insertarRecursivo(raiz->izquierdo, clave, dato);
    } else if (clave > raiz->clave) {
        return insertarRecursivo(raiz->derecho, clave, dato);
    }

    // Clave duplicada: no se permite en un BST con claves únicas
    return false;
}

// Búsqueda Recursiva:
// Eficiencia: O(log n) en árbol balanceado promedio vs O(n) de búsqueda lineal.
// Retorna true si lo encuentra, almacenando el dato y contando el número de
// comparaciones realizadas.
template <typename T>
bool buscarRecursivo(const NodoBST<T>* raiz, int clave, T& outDato, int& comparaciones) {
    if (raiz == nullptr) {
        return false;
    }

    comparaciones++;
    if (clave == raiz->clave) {
        outDato = raiz->dato;
        return true;
    }

    if (clave < raiz->clave) {
        return buscarRecursivo(raiz->izquierdo, clave, outDato, comparaciones);
    } else {
        return buscarRecursivo(raiz->derecho, clave, outDato, comparaciones);
    }
}

// Recorrido In-Order (Inorden: Izquierda -> Raíz -> Derecha):
// Teorema de Matemática Discreta: El recorrido In-Order de un Árbol Binario de
// Búsqueda produce los elementos estrictamente ordenados de forma ascendente.
template <typename T>
void inOrderRecursivo(const NodoBST<T>* raiz, std::vector<T>& listaDatos, std::vector<int>& listaClaves) {
    if (raiz == nullptr) return;

    inOrderRecursivo(raiz->izquierdo, listaDatos, listaClaves);
    listaDatos.push_back(raiz->dato);
    listaClaves.push_back(raiz->clave);
    inOrderRecursivo(raiz->derecho, listaDatos, listaClaves);
}

// Recorrido Pre-Order (Preorden: Raíz -> Izquierda -> Derecha):
// Permite serializar o reconstruir la estructura exacta del árbol jerárquico.
template <typename T>
void preOrderClavesRecursivo(const NodoBST<T>* raiz, std::vector<int>& listaClaves) {
    if (raiz == nullptr) return;

    listaClaves.push_back(raiz->clave);
    preOrderClavesRecursivo(raiz->izquierdo, listaClaves);
    preOrderClavesRecursivo(raiz->derecho, listaClaves);
}

// Recorrido Post-Order (Postorden: Izquierda -> Derecha -> Raíz):
// Usado formalmente para evaluar expresiones y para destrucción segura de memoria.
template <typename T>
void postOrderClavesRecursivo(const NodoBST<T>* raiz, std::vector<int>& listaClaves) {
    if (raiz == nullptr) return;

    postOrderClavesRecursivo(raiz->izquierdo, listaClaves);
    postOrderClavesRecursivo(raiz->derecho, listaClaves);
    listaClaves.push_back(raiz->clave);
}

// Cálculo Recursivo de la Altura:
// Altura = 1 + max(altura(subárbol_izq), altura(subárbol_der))
// Caso Base: Árbol vacío tiene altura 0.
template <typename T>
int calcularAlturaRecursivo(const NodoBST<T>* raiz) {
    if (raiz == nullptr) return 0;
    int alturaIzq = calcularAlturaRecursivo(raiz->izquierdo);
    int alturaDer = calcularAlturaRecursivo(raiz->derecho);
    return 1 + std::max(alturaIzq, alturaDer);
}

// Conteo Recursivo de Nodos (|V| cardinalidad de vértices):
// Caso Base: 0 si es nullptr.
// Paso Recursivo: 1 + contar(izq) + contar(der).
template <typename T>
int contarNodosRecursivo(const NodoBST<T>* raiz) {
    if (raiz == nullptr) return 0;
    return 1 + contarNodosRecursivo(raiz->izquierdo) + contarNodosRecursivo(raiz->derecho);
}

// Liberación recursiva de memoria (Post-orden):
template <typename T>
void liberarArbolRecursivo(NodoBST<T>*& raiz) {
    if (raiz == nullptr) return;
    liberarArbolRecursivo(raiz->izquierdo);
    liberarArbolRecursivo(raiz->derecho);
    delete raiz;
    raiz = nullptr;
}

// Serialización recursiva a JSON de la estructura del árbol:
// Permite que un cliente API o frontend dibuje el árbol visualmente.
template <typename T>
std::string serializarJSONRecursivo(const NodoBST<T>* raiz) {
    if (raiz == nullptr) {
        return "null";
    }

    std::ostringstream ss;
    ss << "{\"clave\":" << raiz->clave;
    ss << ",\"izquierdo\":" << serializarJSONRecursivo(raiz->izquierdo);
    ss << ",\"derecho\":" << serializarJSONRecursivo(raiz->derecho);
    ss << "}";
    return ss.str();
}

// =============================================================================
// CLASE ENVOLTORIO: ArbolBST<T>
// Gestiona el ciclo de vida del árbol, memoria dinámica y operaciones públicas.
// =============================================================================
template <typename T>
class ArbolBST {
private:
    NodoBST<T>* raiz;

public:
    ArbolBST() : raiz(nullptr) {}

    ~ArbolBST() {
        limpiar();
    }

    // Evitar copias superficiales accidentales
    ArbolBST(const ArbolBST&) = delete;
    ArbolBST& operator=(const ArbolBST&) = delete;

    // Movimiento permitido
    ArbolBST(ArbolBST&& otro) noexcept : raiz(otro.raiz) {
        otro.raiz = nullptr;
    }

    ArbolBST& operator=(ArbolBST&& otro) noexcept {
        if (this != &otro) {
            limpiar();
            raiz = otro.raiz;
            otro.raiz = nullptr;
        }
        return *this;
    }

    void limpiar() {
        liberarArbolRecursivo(raiz);
    }

    bool insertar(int clave, const T& dato) {
        return insertarRecursivo(raiz, clave, dato);
    }

    bool buscar(int clave, T& outDato, int& comparaciones) const {
        comparaciones = 0;
        return buscarRecursivo(raiz, clave, outDato, comparaciones);
    }

    bool contiene(int clave) const {
        T dummy;
        int comp = 0;
        return buscar(clave, dummy, comp);
    }

    std::vector<T> obtenerInOrderDatos() const {
        std::vector<T> datos;
        std::vector<int> claves;
        inOrderRecursivo(raiz, datos, claves);
        return datos;
    }

    std::vector<int> obtenerInOrderClaves() const {
        std::vector<T> datos;
        std::vector<int> claves;
        inOrderRecursivo(raiz, datos, claves);
        return claves;
    }

    std::vector<int> obtenerPreOrderClaves() const {
        std::vector<int> claves;
        preOrderClavesRecursivo(raiz, claves);
        return claves;
    }

    std::vector<int> obtenerPostOrderClaves() const {
        std::vector<int> claves;
        postOrderClavesRecursivo(raiz, claves);
        return claves;
    }

    int altura() const {
        return calcularAlturaRecursivo(raiz);
    }

    int totalNodos() const {
        return contarNodosRecursivo(raiz);
    }

    bool estaVacio() const {
        return raiz == nullptr;
    }

    std::string exportarEstructuraJSON() const {
        return serializarJSONRecursivo(raiz);
    }

    const NodoBST<T>* obtenerRaiz() const {
        return raiz;
    }
};

// =============================================================================
// FUNCIONES DE INTEGRACIÓN CON LA PERSISTENCIA (.DAT) DE UMG EXPRESS
// =============================================================================

// Construye un Árbol Binario de Búsqueda indexando todas las guías activas
// almacenadas en el archivo binario data/guias.dat.
inline ArbolBST<Guia> construirArbolGuias(const char* filepath = "data/guias.dat") {
    ArbolBST<Guia> arbol;
    std::vector<Guia> guias = Persistence::leerTodos<Guia>(filepath, true);
    for (const auto& g : guias) {
        arbol.insertar(g.numeroGuia, g);
    }
    return arbol;
}

// Construye un Árbol Binario de Búsqueda indexando todos los clientes activos
// almacenados en el archivo binario data/clientes.dat.
inline ArbolBST<Cliente> construirArbolClientes(const char* filepath = "data/clientes.dat") {
    ArbolBST<Cliente> arbol;
    std::vector<Cliente> clientes = Persistence::leerTodos<Cliente>(filepath, true);
    for (const auto& c : clientes) {
        arbol.insertar(c.id, c);
    }
    return arbol;
}

} // namespace Arboles

#endif // ARBOL_BST_H
