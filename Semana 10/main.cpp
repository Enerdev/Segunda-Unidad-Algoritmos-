// ============================================================
// PRÁCTICA N.° 10 — main.cpp (Actividades 8–12)
// Estudiante: Flores Flores, Emerson Aldair | Cód. 240439
// SIS210 — Algoritmos y Estructuras de Datos — UNAP 2025
// ============================================================
#include "ArbolAcademico.hpp"
#include <iostream>
#include <iomanip>
using namespace una_puno;

int main() {
    std::cout << "=== BST Sistema Academico UNA-PUNO (C++17) ===\n\n";
    ArbolAcademico arbol;

    // Mismos 7 registros que Python (Actividades 2–4)
    arbol.insertar({20210500,"Mamani Quispe, Juan",   "Ing. Sistemas",15.8f,120,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol.insertar({20210300,"Huanca Apaza, Maria",   "Ing. Civil",   14.2f,110,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol.insertar({20210700,"Condori Flores, Pedro", "Medicina",     17.1f,130,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol.insertar({20210100,"Ticona Lupaca, Rosa",   "Contabilidad", 12.0f, 90,EstadoAcademico::SUSPENDIDO,"2021-I"});
    arbol.insertar({20210400,"Larico Ccama, Carlos",  "Ing. Sistemas",16.5f,115,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol.insertar({20210600,"Cutipa Vargas, Elena",  "Agronomia",    13.7f,100,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol.insertar({20210900,"Pari Choque, Luis",     "Ing. Sistemas",18.3f,140,EstadoAcademico::EGRESADO,  "2021-I"});

    // Actividad 9: Estructura ASCII + In-Order + BFS
    arbol.imprimirArbol();

    std::cout << "\n-- In-Order (ordenado por codigo) --\n";
    std::cout << std::left
              << std::setw(12) << "CODIGO"
              << std::setw(35) << "NOMBRE"
              << std::setw(20) << "ESCUELA"
              << "PPA\n"
              << std::string(72, '-') << '\n';
    for (const auto& e : arbol.inOrder()) e.print();

    std::cout << "\n-- Pre-Order: ";
    for (const auto& e : arbol.preOrder()) std::cout << e.codigo << " ";

    std::cout << "\n-- Post-Order: ";
    for (const auto& e : arbol.postOrder()) std::cout << e.codigo << " ";

    std::cout << "\n-- BFS (nivel por nivel): ";
    for (const auto& e : arbol.bfs()) std::cout << e.codigo << " ";
    std::cout << "\n\nAltura: " << arbol.altura() << '\n';

    // Actividad 10: Búsqueda y eliminación
    std::cout << "\n-- Busqueda --\n";
    auto res = arbol.buscar(20210700);
    if (res) { std::cout << "Encontrado: "; res->print(); }
    auto noExiste = arbol.buscar(99999999);
    std::cout << "Buscar 99999999: "
              << (noExiste ? "encontrado" : "no encontrado") << '\n';

    // Actividad 11: Consulta PPA + Estadísticas
    std::cout << "\n-- Estudiantes con PPA >= 15.0 --\n";
    for (const auto& e : arbol.porRangoPPA(15.0f)) e.print();

    std::cout << "\n-- Estadisticas --\n";
    arbol.estadisticas();

    // Eliminación
    std::cout << "\n-- Eliminando codigo 20210300 --\n";
    arbol.eliminar(20210300);
    std::cout << "Nodos restantes: " << arbol.inOrder().size() << '\n';
    std::cout << "In-Order: ";
    for (const auto& e : arbol.inOrder()) std::cout << e.codigo << " ";
    std::cout << '\n';

    // Actividad 12: Verificación tabla
    std::cout << "\n============================================\n";
    std::cout << "TABLA DE VERIFICACION Python vs C++\n";
    std::cout << "============================================\n";
    auto io  = arbol.inOrder();
    auto bfs = arbol.bfs();
    std::cout << "In-order primer codigo : " << io.front().codigo  << "   esperado: 20210100\n";
    std::cout << "In-order ultimo codigo : " << io.back().codigo   << "   esperado: 20210900\n";
    std::cout << "Altura del arbol       : " << arbol.altura()     << "         esperado: 2\n";
    std::cout << "Total nodos (tras elim): " << io.size()          << "             esperado: 6\n";
    auto b700 = arbol.buscar(20210700);
    std::cout << "Buscar 20210700 PPA    : " << (b700 ? b700->ppa : -1.0f) << "      esperado: 17.1\n";
    std::cout << "Buscar 99999999        : " << (!arbol.buscar(99999999) ? "no encontrado" : "encontrado")
              << "  esperado: no encontrado\n";

    // Máximo + rango de código
    std::cout << "\n-- Maximo --\n";
    auto mx = arbol.maximo();
    if (mx) std::cout << "Codigo maximo: " << mx->codigo << '\n';

    std::cout << "\n-- Rango de codigo [20210400, 20210700] --\n";
    // Re-insertar para tener todos los nodos
    ArbolAcademico arbol2;
    arbol2.insertar({20210500,"Mamani Quispe, Juan",   "Ing. Sistemas",15.8f,120,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol2.insertar({20210300,"Huanca Apaza, Maria",   "Ing. Civil",   14.2f,110,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol2.insertar({20210700,"Condori Flores, Pedro", "Medicina",     17.1f,130,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol2.insertar({20210100,"Ticona Lupaca, Rosa",   "Contabilidad", 12.0f, 90,EstadoAcademico::SUSPENDIDO,"2021-I"});
    arbol2.insertar({20210400,"Larico Ccama, Carlos",  "Ing. Sistemas",16.5f,115,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol2.insertar({20210600,"Cutipa Vargas, Elena",  "Agronomia",    13.7f,100,EstadoAcademico::ACTIVO,    "2021-I"});
    arbol2.insertar({20210900,"Pari Choque, Luis",     "Ing. Sistemas",18.3f,140,EstadoAcademico::EGRESADO,  "2021-I"});
    for (const auto& e : arbol2.buscarRangoCodigo(20210400, 20210700))
        std::cout << e.codigo << " ";
    std::cout << '\n';

    return 0;
}
