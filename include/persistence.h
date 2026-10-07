#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include "structs.h"

namespace Persistence {

// Garantiza que el archivo exista (lo crea si no existe)
inline bool asegurarArchivo(const char* filepath) {
    FILE* f = fopen(filepath, "a+b");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}

// Lee todos los registros de un archivo .dat (opcionalmente filtrando por activo == true)
template <typename T>
std::vector<T> leerTodos(const char* filepath, bool soloActivos = true) {
    std::vector<T> registros;
    FILE* f = fopen(filepath, "rb");
    if (!f) return registros;

    T item;
    while (fread(&item, sizeof(T), 1, f) == 1) {
        if (!soloActivos || item.activo) {
            registros.push_back(item);
        }
    }
    fclose(f);
    return registros;
}

// Escribe un registro al final del archivo .dat
template <typename T>
bool insertar(const char* filepath, const T& record) {
    asegurarArchivo(filepath);
    FILE* f = fopen(filepath, "a+b");
    if (!f) return false;

    size_t escritos = fwrite(&record, sizeof(T), 1, f);
    fclose(f);
    return escritos == 1;
}

// Actualiza un registro en el índice directo especificado utilizando fseek
template <typename T>
bool actualizarEnIndice(const char* filepath, long index, const T& record) {
    FILE* f = fopen(filepath, "r+b");
    if (!f) return false;

    if (fseek(f, index * sizeof(T), SEEK_SET) != 0) {
        fclose(f);
        return false;
    }

    size_t escritos = fwrite(&record, sizeof(T), 1, f);
    fclose(f);
    return escritos == 1;
}

// Busca un registro por ID y retorna su índice físico en el archivo .dat
template <typename T>
bool buscarPorId(const char* filepath, int id, T& outRecord, long& outIndex) {
    FILE* f = fopen(filepath, "rb");
    if (!f) return false;

    T item;
    long index = 0;
    while (fread(&item, sizeof(T), 1, f) == 1) {
        if (item.activo && item.id == id) {
            outRecord = item;
            outIndex = index;
            fclose(f);
            return true;
        }
        index++;
    }
    fclose(f);
    return false;
}

// Comprueba si un ID existe (activo o inactivo)
template <typename T>
bool existeId(const char* filepath, int id) {
    FILE* f = fopen(filepath, "rb");
    if (!f) return false;

    T item;
    while (fread(&item, sizeof(T), 1, f) == 1) {
        if (item.id == id) {
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

// Actualiza un registro buscando por ID
template <typename T>
bool actualizarPorId(const char* filepath, int id, const T& nuevoRecord) {
    T actual;
    long index = -1;
    if (!buscarPorId<T>(filepath, id, actual, index)) {
        return false;
    }
    return actualizarEnIndice<T>(filepath, index, nuevoRecord);
}

// Realiza una baja lógica cambiando activo = false y sobrescribiendo con fseek
template <typename T>
bool eliminarLogico(const char* filepath, int id) {
    T actual;
    long index = -1;
    if (!buscarPorId<T>(filepath, id, actual, index)) {
        return false;
    }
    actual.activo = false;
    return actualizarEnIndice<T>(filepath, index, actual);
}

// Genera el siguiente ID secuencial (max_id + 1)
template <typename T>
int obtenerSiguienteId(const char* filepath) {
    FILE* f = fopen(filepath, "rb");
    if (!f) return 1;

    int maxId = 0;
    T item;
    while (fread(&item, sizeof(T), 1, f) == 1) {
        if (item.id > maxId) {
            maxId = item.id;
        }
    }
    fclose(f);
    return maxId + 1;
}

// --- Funciones especializadas para Guia (donde la llave primaria es numeroGuia) ---
inline bool buscarGuiaPorNumero(const char* filepath, int numeroGuia, Guia& outRecord, long& outIndex) {
    FILE* f = fopen(filepath, "rb");
    if (!f) return false;

    Guia item;
    long index = 0;
    while (fread(&item, sizeof(Guia), 1, f) == 1) {
        if (item.activo && item.numeroGuia == numeroGuia) {
            outRecord = item;
            outIndex = index;
            fclose(f);
            return true;
        }
        index++;
    }
    fclose(f);
    return false;
}

inline bool eliminarGuiaLogico(const char* filepath, int numeroGuia) {
    Guia actual;
    long index = -1;
    if (!buscarGuiaPorNumero(filepath, numeroGuia, actual, index)) {
        return false;
    }
    actual.activo = false;
    return actualizarEnIndice<Guia>(filepath, index, actual);
}

inline int obtenerSiguienteNumeroGuia(const char* filepath) {
    FILE* f = fopen(filepath, "rb");
    if (!f) return 1001; // Iniciar números de guía en 1001 por ejemplo

    int maxNum = 1000;
    Guia item;
    while (fread(&item, sizeof(Guia), 1, f) == 1) {
        if (item.numeroGuia > maxNum) {
            maxNum = item.numeroGuia;
        }
    }
    fclose(f);
    return maxNum + 1;
}

} // namespace Persistence

#endif // PERSISTENCE_H
