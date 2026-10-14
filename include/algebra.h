#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <cmath>
#include <iostream>
#include <string>

namespace Algebra {

// =============================================================================
// 1. ESPACIOS VECTORIALES Y VECTORES (R2 y R3)
// =============================================================================

struct Vector2D {
    double x;
    double y;

    Vector2D(double _x = 0.0, double _y = 0.0) : x(_x), y(_y) {}

    // Suma de vectores: u + v
    Vector2D operator+(const Vector2D& o) const {
        return Vector2D(x + o.x, y + o.y);
    }

    // Resta de vectores: u - v
    Vector2D operator-(const Vector2D& o) const {
        return Vector2D(x - o.x, y - o.y);
    }

    // Multiplicación por un escalar: k * v
    Vector2D operator*(double k) const {
        return Vector2D(x * k, y * k);
    }

    // Magnitud o norma euclidiana: ||v|| = sqrt(x^2 + y^2)
    double norma() const {
        return std::sqrt(x * x + y * y);
    }
};

struct Vector3D {
    double x;
    double y;
    double z;

    Vector3D(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {}

    // Suma de vectores en R3
    Vector3D operator+(const Vector3D& o) const {
        return Vector3D(x + o.x, y + o.y, z + o.z);
    }

    // Resta de vectores en R3
    Vector3D operator-(const Vector3D& o) const {
        return Vector3D(x - o.x, y - o.y, z - o.z);
    }

    // Multiplicación por un escalar en R3
    Vector3D operator*(double k) const {
        return Vector3D(x * k, y * k, z * k);
    }

    // Magnitud o norma euclidiana: ||v|| = sqrt(x^2 + y^2 + z^2)
    double norma() const {
        return std::sqrt(x * x + y * y + z * z);
    }
};

// =============================================================================
// 2. PRODUCTO PUNTO (PRODUCTO ESCALAR)
// =============================================================================

// u . v = (u_x * v_x) + (u_y * v_y)
inline double productoPunto(const Vector2D& u, const Vector2D& v) {
    return (u.x * v.x) + (u.y * v.y);
}

// u . v = (u_x * v_x) + (u_y * v_y) + (u_z * v_z)
inline double productoPunto(const Vector3D& u, const Vector3D& v) {
    return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

// Aplicación logística del Producto Punto:
// Costo variable = vector_carga (peso, volumen) . vector_tarifas (precioKg, precioM3)
inline double calcularCostoVariableVectorial(double pesoFacturableKg, double volumenM3,
                                            double precioPorKg, double precioPorM3) {
    Vector2D vectorCarga(pesoFacturableKg, volumenM3);
    Vector2D vectorTarifas(precioPorKg, precioPorM3);
    return productoPunto(vectorCarga, vectorTarifas);
}

// =============================================================================
// 3. PRODUCTO CRUZ (PRODUCTO VECTORIAL EN R3)
// =============================================================================

// u x v = (u_y*v_z - u_z*v_y, u_z*v_x - u_x*v_z, u_x*v_y - u_y*v_x)
inline Vector3D productoCruz(const Vector3D& u, const Vector3D& v) {
    return Vector3D(
        (u.y * v.z) - (u.z * v.y),
        (u.z * v.x) - (u.x * v.z),
        (u.x * v.y) - (u.y * v.x)
    );
}

// =============================================================================
// 4. APLICACIONES PARA EL CÁLCULO DE ÁREAS
// =============================================================================

// Área del paralelogramo formado por u y v: Area = ||u x v||
inline double calcularAreaParalelogramo(const Vector3D& u, const Vector3D& v) {
    return productoCruz(u, v).norma();
}

// Área del triángulo formado por u y v: Area = (1/2) * ||u x v||
inline double calcularAreaTriangulo(const Vector3D& u, const Vector3D& v) {
    return 0.5 * calcularAreaParalelogramo(u, v);
}

// Aplicación al empaque: Área superficial total de embalaje de un paquete (en m2)
// A partir de los vectores arista ortogonales u = (l,0,0), v = (0,w,0), w = (0,0,h)
// Area = 2 * ( ||u x v|| + ||u x w|| + ||v x w|| )
inline double calcularAreaSuperficialPaqueteM2(double largoCm, double anchoCm, double altoCm) {
    if (largoCm <= 0.0 || anchoCm <= 0.0 || altoCm <= 0.0) return 0.0;
    double l = largoCm / 100.0;
    double w = anchoCm / 100.0;
    double h = altoCm / 100.0;

    Vector3D vecL(l, 0.0, 0.0);
    Vector3D vecW(0.0, w, 0.0);
    Vector3D vecH(0.0, 0.0, h);

    double caraBase = calcularAreaParalelogramo(vecL, vecW);   // l * w
    double caraFrente = calcularAreaParalelogramo(vecL, vecH); // l * h
    double caraLateral = calcularAreaParalelogramo(vecW, vecH);// w * h

    return 2.0 * (caraBase + caraFrente + caraLateral);
}

// =============================================================================
// 5. DETERMINANTES Y CÁLCULO DE VOLÚMENES
// =============================================================================

// Determinante de una matriz 3x3 formada por 3 vectores fila/columna:
// Corresponde al Triple Producto Escalar: det(u, v, w) = u . (v x w)
inline double determinante3x3(const Vector3D& u, const Vector3D& v, const Vector3D& w) {
    return productoPunto(u, productoCruz(v, w));
}

// Volumen del paralelepípedo generado por tres vectores en R3:
// Volumen = | det(u, v, w) | = | u . (v x w) |
inline double calcularVolumenParalelepipedo(const Vector3D& u, const Vector3D& v, const Vector3D& w) {
    return std::abs(determinante3x3(u, v, w));
}

// Aplicación al paquete de carga: volumen en m3 calculado mediante determinante
inline double calcularVolumenPaqueteM3(double largoCm, double anchoCm, double altoCm) {
    if (largoCm <= 0.0 || anchoCm <= 0.0 || altoCm <= 0.0) return 0.0;
    double l = largoCm / 100.0;
    double w = anchoCm / 100.0;
    double h = altoCm / 100.0;

    Vector3D u(l, 0.0, 0.0);
    Vector3D v(0.0, w, 0.0);
    Vector3D wVec(0.0, 0.0, h);

    return calcularVolumenParalelepipedo(u, v, wVec);
}

// =============================================================================
// 6. ECUACIÓN PARAMÉTRICA DE UNA RECTA
// =============================================================================

// Ecuación vectorial y paramétrica de la recta en R3:
// r(t) = P0 + t * v
// donde:
//   P0: Punto de origen (por ejemplo, coordenadas de sucursal de origen)
//   v:  Vector director v = (P1 - P0) hacia la sucursal de destino
//   t:  Parámetro escalar (por ejemplo, tiempo o factor de avance t in [0.0, 1.0])
struct RectaParametrica3D {
    Vector3D p0; // Punto inicial (x0, y0, z0)
    Vector3D v;  // Vector director (vx, vy, vz)

    RectaParametrica3D(const Vector3D& origen, const Vector3D& destino)
        : p0(origen), v(destino - origen) {}

    // Evalúa la posición r(t) para un parámetro t
    // x(t) = x0 + t * vx
    // y(t) = y0 + t * vy
    // z(t) = z0 + t * vz
    Vector3D evaluar(double t) const {
        return p0 + (v * t);
    }
};

// =============================================================================
// 7. ECUACIÓN SIMÉTRICA DE UNA RECTA
// =============================================================================

// Forma simétrica de la recta:
// (x - x0) / vx = (y - y0) / vy = (z - z0) / vz = t
// Permite verificar si un punto P pertenece a la trayectoria de la recta
inline bool puntoPerteneceARecta(const RectaParametrica3D& recta, const Vector3D& punto, double epsilon = 0.001) {
    double tX = (std::abs(recta.v.x) > 1e-9) ? (punto.x - recta.p0.x) / recta.v.x : 0.0;
    double tY = (std::abs(recta.v.y) > 1e-9) ? (punto.y - recta.p0.y) / recta.v.y : 0.0;
    double tZ = (std::abs(recta.v.z) > 1e-9) ? (punto.z - recta.p0.z) / recta.v.z : 0.0;

    bool coincideXY = (std::abs(recta.v.x) <= 1e-9 && std::abs(punto.x - recta.p0.x) < epsilon) ||
                      (std::abs(recta.v.y) <= 1e-9 && std::abs(punto.y - recta.p0.y) < epsilon) ||
                      (std::abs(tX - tY) < epsilon);

    bool coincideYZ = (std::abs(recta.v.y) <= 1e-9 && std::abs(punto.y - recta.p0.y) < epsilon) ||
                      (std::abs(recta.v.z) <= 1e-9 && std::abs(punto.z - recta.p0.z) < epsilon) ||
                      (std::abs(tY - tZ) < epsilon);

    return coincideXY && coincideYZ;
}

} // namespace Algebra

#endif // ALGEBRA_H
