#include <iostream>
#include <cassert>
#include <cmath>
#include "algebra.h"
#include "transacciones.h"

bool casiIgual(double a, double b, double epsilon = 0.001) {
    return std::abs(a - b) < epsilon;
}

void probarVectoresYEspaciosVectoriales() {
    std::cout << "  [1] Probando Vectores y Operaciones de Espacio Vectorial (R2 y R3)..." << std::endl;

    // Espacio Vectorial R2: u + v, k * v
    Algebra::Vector2D u2(3.0, 4.0);
    Algebra::Vector2D v2(1.0, 2.0);
    Algebra::Vector2D suma2 = u2 + v2;
    assert(casiIgual(suma2.x, 4.0) && casiIgual(suma2.y, 6.0));

    Algebra::Vector2D esc2 = u2 * 2.0;
    assert(casiIgual(esc2.x, 6.0) && casiIgual(esc2.y, 8.0));
    assert(casiIgual(u2.norma(), 5.0)); // sqrt(3^2 + 4^2) = 5.0

    // Espacio Vectorial R3: u + v, k * v
    Algebra::Vector3D u3(1.0, 2.0, 3.0);
    Algebra::Vector3D v3(4.0, 5.0, 6.0);
    Algebra::Vector3D suma3 = u3 + v3;
    assert(casiIgual(suma3.x, 5.0) && casiIgual(suma3.y, 7.0) && casiIgual(suma3.z, 9.0));

    std::cout << "      -> OK: Cerradura bajo suma, multiplicación por escalar y normas verificadas." << std::endl;
}

void probarProductoPunto() {
    std::cout << "  [2] Probando Producto Punto y Costo Vectorial..." << std::endl;

    Algebra::Vector2D a(2.0, 3.0);
    Algebra::Vector2D b(4.0, -1.0);
    // a . b = (2 * 4) + (3 * -1) = 8 - 3 = 5.0
    assert(casiIgual(Algebra::productoPunto(a, b), 5.0));

    // Aplicación logística: vector carga (12.0 kg, 0.06 m3) . vector tarifas (5.0 Q/kg, 20.0 Q/m3)
    double costoVar = Algebra::calcularCostoVariableVectorial(12.0, 0.06, 5.0, 20.0);
    // 12.0 * 5.0 + 0.06 * 20.0 = 60.0 + 1.2 = 61.20
    assert(casiIgual(costoVar, 61.20));

    std::cout << "      -> OK: Producto punto y aplicación al costo logístico verificados." << std::endl;
}

void probarProductoCruzYAreas() {
    std::cout << "  [3] Probando Producto Cruz y Cálculo de Áreas..." << std::endl;

    // Vectores ortogonales unitarios en X e Y: i x j = k = (0, 0, 1)
    Algebra::Vector3D i(1.0, 0.0, 0.0);
    Algebra::Vector3D j(0.0, 1.0, 0.0);
    Algebra::Vector3D k = Algebra::productoCruz(i, j);
    assert(casiIgual(k.x, 0.0) && casiIgual(k.y, 0.0) && casiIgual(k.z, 1.0));

    // Área del paralelogramo: ||u x v||
    Algebra::Vector3D u(2.0, 0.0, 0.0);
    Algebra::Vector3D v(0.0, 3.0, 0.0);
    double area = Algebra::calcularAreaParalelogramo(u, v);
    assert(casiIgual(area, 6.0));

    // Área superficial de una caja de 50cm x 40cm x 30cm (0.5m x 0.4m x 0.3m)
    // A = 2 * (0.5*0.4 + 0.5*0.3 + 0.4*0.3) = 2 * (0.20 + 0.15 + 0.12) = 2 * 0.47 = 0.94 m2
    double areaSuperficial = Algebra::calcularAreaSuperficialPaqueteM2(50.0, 40.0, 30.0);
    assert(casiIgual(areaSuperficial, 0.94));

    std::cout << "      -> OK: Producto cruz y cálculo de áreas de embalaje verificados." << std::endl;
}

void probarDeterminantesYVolumenes() {
    std::cout << "  [4] Probando Determinantes (3x3) y Cálculo de Volúmenes..." << std::endl;

    // Volumen de caja 50cm x 40cm x 30cm = 0.5m * 0.4m * 0.3m = 0.06 m3
    // Calculado mediante determinante / triple producto escalar: u . (v x w)
    double volM3 = Algebra::calcularVolumenPaqueteM3(50.0, 40.0, 30.0);
    assert(casiIgual(volM3, 0.06));

    // Verificación de integración con Guias::calcularVolumenM3
    double volGuias = Guias::calcularVolumenM3(50.0, 40.0, 30.0);
    assert(casiIgual(volGuias, 0.06));

    std::cout << "      -> OK: Determinante 3x3 y volumen de paralelepípedo verificados." << std::endl;
}

void probarEcuacionesParametricaYSimetricaDeRecta() {
    std::cout << "  [5] Probando Ecuación Paramétrica y Simétrica de la Recta..." << std::endl;

    // Sucursal Central (0, 0, 0) y Sucursal Norte (100, 200, 50)
    Algebra::Vector3D origen(0.0, 0.0, 0.0);
    Algebra::Vector3D destino(100.0, 200.0, 50.0);
    Algebra::RectaParametrica3D ruta(origen, destino);

    // Ecuación Paramétrica: r(t) = P0 + t * v
    // A medio camino (t = 0.5):
    Algebra::Vector3D puntoMedio = ruta.evaluar(0.5);
    assert(casiIgual(puntoMedio.x, 50.0));
    assert(casiIgual(puntoMedio.y, 100.0));
    assert(casiIgual(puntoMedio.z, 25.0));

    // Al llegar al destino (t = 1.0):
    Algebra::Vector3D puntoDestino = ruta.evaluar(1.0);
    assert(casiIgual(puntoDestino.x, 100.0));
    assert(casiIgual(puntoDestino.y, 200.0));
    assert(casiIgual(puntoDestino.z, 50.0));

    // Ecuación Simétrica: (x - x0)/vx = (y - y0)/vy = (z - z0)/vz
    // Verificar que puntoMedio pertenece a la recta
    assert(Algebra::puntoPerteneceARecta(ruta, puntoMedio));

    // Punto desalineado (fuera de la ruta)
    Algebra::Vector3D puntoFuera(50.0, 999.0, 25.0);
    assert(!Algebra::puntoPerteneceARecta(ruta, puntoFuera));

    std::cout << "      -> OK: Trayectoria paramétrica r(t) y ecuación simétrica verificadas." << std::endl;
}

int main() {
    std::cout << "=== Test de Álgebra Lineal y Geometría Analítica (UMG Express) ===" << std::endl;
    probarVectoresYEspaciosVectoriales();
    probarProductoPunto();
    probarProductoCruzYAreas();
    probarDeterminantesYVolumenes();
    probarEcuacionesParametricaYSimetricaDeRecta();
    std::cout << "=== Todos los conceptos matemáticos implementados y probados con éxito! ===" << std::endl;
    return 0;
}
