// ============================================================
//  Práctica N.° 10 — Inducción Estructural — UNAP 2025
//  Verificación Formal del Índice de Matrículas SIGA-UNAP
//  Actividades 5 y 6 — Framework en C++17
//  Dr. Aldo Hernán Zanabria Gálvez
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <climits>
#include <random>
#include <memory>
#include <sstream>

// ─── Dominio ─────────────────────────────────────────────────────────────────

enum class EstadoAcademico { ACTIVO, RETIRADO, EGRESADO };

struct Estudiante {
    int         codigo;
    std::string nombre;
    std::string apellido;
    float       promedio;
    int         creditos;
    EstadoAcademico estado;
    std::string semestre;
};

// ─── Nodo BST ────────────────────────────────────────────────────────────────

struct NodoBST {
    Estudiante              dato;
    std::unique_ptr<NodoBST> izquierdo;
    std::unique_ptr<NodoBST> derecho;

    explicit NodoBST(Estudiante e) : dato(std::move(e)) {}
};

// ─── Árbol Académico ─────────────────────────────────────────────────────────

class ArbolAcademico {
public:
    std::unique_ptr<NodoBST> raiz;

    // ── Inserción ──────────────────────────────────────────────────────────
    void insertar(Estudiante est) {
        raiz = _insertar(std::move(raiz), std::move(est));
    }

    // ── Eliminación ────────────────────────────────────────────────────────
    void eliminar(int codigo) {
        raiz = _eliminar(std::move(raiz), codigo);
    }

    // ── In-order ───────────────────────────────────────────────────────────
    std::vector<Estudiante> inOrder() const {
        std::vector<Estudiante> v;
        _inOrder(raiz.get(), v);
        return v;
    }

    // ── Altura ─────────────────────────────────────────────────────────────
    int altura() const { return _altura(raiz.get()); }

private:
    static std::unique_ptr<NodoBST>
    _insertar(std::unique_ptr<NodoBST> nodo, Estudiante est) {
        if (!nodo) return std::make_unique<NodoBST>(std::move(est));
        if (est.codigo < nodo->dato.codigo)
            nodo->izquierdo = _insertar(std::move(nodo->izquierdo), std::move(est));
        else if (est.codigo > nodo->dato.codigo)
            nodo->derecho = _insertar(std::move(nodo->derecho), std::move(est));
        return nodo;
    }

    static std::unique_ptr<NodoBST>
    _eliminar(std::unique_ptr<NodoBST> nodo, int codigo) {
        if (!nodo) return nullptr;
        if (codigo < nodo->dato.codigo)
            nodo->izquierdo = _eliminar(std::move(nodo->izquierdo), codigo);
        else if (codigo > nodo->dato.codigo)
            nodo->derecho = _eliminar(std::move(nodo->derecho), codigo);
        else {
            if (!nodo->izquierdo) return std::move(nodo->derecho);
            if (!nodo->derecho)   return std::move(nodo->izquierdo);
            // Dos hijos — sucesor in-order
            NodoBST* sucesor = nodo->derecho.get();
            while (sucesor->izquierdo) sucesor = sucesor->izquierdo.get();
            nodo->dato = sucesor->dato;
            nodo->derecho = _eliminar(std::move(nodo->derecho), sucesor->dato.codigo);
        }
        return nodo;
    }

    static void _inOrder(const NodoBST* n, std::vector<Estudiante>& v) {
        if (!n) return;
        _inOrder(n->izquierdo.get(), v);
        v.push_back(n->dato);
        _inOrder(n->derecho.get(), v);
    }

    static int _altura(const NodoBST* n) {
        if (!n) return -1;
        return 1 + std::max(_altura(n->izquierdo.get()), _altura(n->derecho.get()));
    }
};

// ─── Actividad 5: Verificadores ──────────────────────────────────────────────

bool esBST(const NodoBST* n, int mn = INT_MIN, int mx = INT_MAX) {
    if (!n) return true;
    if (n->dato.codigo <= mn || n->dato.codigo >= mx) return false;
    return esBST(n->izquierdo.get(), mn, n->dato.codigo) &&
           esBST(n->derecho.get(),   n->dato.codigo, mx);
}

bool esInOrderOrdenado(const ArbolAcademico& a) {
    auto v = a.inOrder();
    return std::is_sorted(v.begin(), v.end(),
        [](const Estudiante& x, const Estudiante& y){ return x.codigo < y.codigo; });
}

int contarRecursivo(const NodoBST* n) {
    if (!n) return 0;
    return 1 + contarRecursivo(n->izquierdo.get())
             + contarRecursivo(n->derecho.get());
}

bool cotaAlturaOK(int altura, int n) {
    if (n == 0) return true;
    int cota = static_cast<int>(std::ceil(std::log2(n + 1))) - 1;
    return altura >= cota;
}

// ─── Actividad 6: Fuzz testing ────────────────────────────────────────────────

struct ResultadoFuzz {
    int         caso, op;
    std::string accion;
    int         codigo;
    int         nNodos;
    bool        p1, p2, p3, p4;
};

std::vector<ResultadoFuzz>
fuzzTest(int nCasos = 500, int nOps = 50, unsigned semilla = 42) {
    std::mt19937 rng(semilla);
    std::uniform_int_distribution<int> distOp(0, 2);

    std::vector<ResultadoFuzz> fallos;

    for (int caso = 0; caso < nCasos; caso++) {
        ArbolAcademico arbol;
        std::vector<int> activos;

        for (int op = 0; op < nOps; op++) {
            bool insertar = (distOp(rng) != 2) || activos.empty();
            int codigo;

            if (insertar) {
                codigo = 20000000 + caso * 1000 + op;
                arbol.insertar({codigo, "T", "T", 15.0f, 100,
                                EstadoAcademico::ACTIVO, "2024-I"});
                activos.push_back(codigo);
            } else {
                std::uniform_int_distribution<int> distIdx(0, (int)activos.size()-1);
                int idx = distIdx(rng);
                codigo = activos[idx];
                arbol.eliminar(codigo);
                activos.erase(activos.begin() + idx);
            }

            int n    = (int)activos.size();
            bool p1  = esInOrderOrdenado(arbol);
            bool p2  = cotaAlturaOK(arbol.altura(), n);
            bool p3  = (contarRecursivo(arbol.raiz.get()) == n);
            bool p4  = esBST(arbol.raiz.get());

            if (!(p1 && p2 && p3 && p4)) {
                fallos.push_back({caso, op,
                    insertar ? "insertar" : "eliminar",
                    codigo, n, p1, p2, p3, p4});
            }
        }
    }
    return fallos;
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << std::string(70, '=') << "\n";
    std::cout << "  PRACTICA N.° 10 — Induccion Estructural — UNAP 2025\n";
    std::cout << "  Verificacion Formal del Indice de Matriculas SIGA-UNAP (C++17)\n";
    std::cout << std::string(70, '=') << "\n\n";

    // ── Demo con árbol pequeño ────────────────────────────────────────────
    std::cout << "[ACTIVIDAD 5] Verificacion con arbol de demostracion:\n";
    ArbolAcademico demo;
    std::vector<int> codigos_demo = {20240005,20240003,20240008,
                                     20240001,20240004,20240007,20240009};
    for (int c : codigos_demo)
        demo.insertar({c,"T","T",15.0f,100,EstadoAcademico::ACTIVO,"2024-I"});

    int n_demo = (int)codigos_demo.size();
    auto seq = demo.inOrder();
    std::cout << "  Nodos insertados : " << n_demo << "\n";
    std::cout << "  Altura del arbol : " << demo.altura() << "\n";
    std::cout << "  In-order (codigos): ";
    for (auto& e : seq) std::cout << e.codigo << " ";
    std::cout << "\n";

    bool p1d = esInOrderOrdenado(demo);
    bool p2d = cotaAlturaOK(demo.altura(), n_demo);
    bool p3d = (contarRecursivo(demo.raiz.get()) == n_demo);
    bool p4d = esBST(demo.raiz.get());

    auto yesno = [](bool b){ return b ? "PASS ✓" : "FAIL ✗"; };
    std::cout << "  P1_inorder_ordenado         : " << yesno(p1d) << "\n";
    std::cout << "  P2_cota_altura              : " << yesno(p2d) << "\n";
    std::cout << "  P3_conteo_correcto          : " << yesno(p3d) << "\n";
    std::cout << "  P4_es_bst                   : " << yesno(p4d) << "\n";

    // assert para certificar en tiempo de compilación/runtime
    assert(p1d && "P1 fallo en demo");
    assert(p2d && "P2 fallo en demo");
    assert(p3d && "P3 fallo en demo");
    assert(p4d && "P4 fallo en demo");

    // Eliminación — Caso 3 (dos hijos)
    std::cout << "\n  Eliminando 20240005 (dos hijos — Caso 3 de P5):\n";
    demo.eliminar(20240005);
    auto seq2 = demo.inOrder();
    std::cout << "  In-order post-eliminar: ";
    for (auto& e : seq2) std::cout << e.codigo << " ";
    std::cout << "\n";
    bool p1p = esInOrderOrdenado(demo);
    bool p4p = esBST(demo.raiz.get());
    std::cout << "  P1_inorder_ordenado : " << yesno(p1p) << "\n";
    std::cout << "  P4_es_bst           : " << yesno(p4p) << "\n";
    assert(p1p && p4p);

    // ── Fuzzing masivo ────────────────────────────────────────────────────
    std::cout << "\n[ACTIVIDAD 6] Fuzzing: 500 secuencias x 50 operaciones...\n";
    auto fallos = fuzzTest(500, 50, 42);
    int total = 500 * 50;

    std::cout << "\n  Verificaciones ejecutadas : " << total << "\n";
    std::cout << "  Fallos detectados         : " << fallos.size() << "\n";

    if (fallos.empty()) {
        std::cout << "\n  CERTIFICACION C++ EXITOSA\n";
        std::cout << "  P1, P2, P3, P4 verificadas en todos los casos. ✓\n";
    } else {
        auto& f = fallos[0];
        std::cout << "\n  PRIMER FALLO:\n";
        std::cout << "    Caso #" << f.caso << ", Op #" << f.op
                  << " | " << f.accion << " | cod=" << f.codigo
                  << " | nodos=" << f.nNodos << "\n";
        std::cout << "    P1=" << yesno(f.p1) << "  P2=" << yesno(f.p2)
                  << "  P3=" << yesno(f.p3) << "  P4=" << yesno(f.p4) << "\n";
    }

    // Tabla de certificación
    std::cout << "\n" << std::string(70,'=') << "\n";
    std::cout << "  TABLA DE CERTIFICACION — C++\n";
    std::cout << std::string(70,'=') << "\n";
    std::cout << "  Propiedad                      Casos  Fallos C++  Certificada\n";
    std::cout << "  " << std::string(66,'-') << "\n";

    struct PropInfo { std::string label; bool (*check)(const ResultadoFuzz&); };
    auto fallosP1 = std::count_if(fallos.begin(),fallos.end(),[](auto& f){return !f.p1;});
    auto fallosP2 = std::count_if(fallos.begin(),fallos.end(),[](auto& f){return !f.p2;});
    auto fallosP3 = std::count_if(fallos.begin(),fallos.end(),[](auto& f){return !f.p3;});
    auto fallosP4 = std::count_if(fallos.begin(),fallos.end(),[](auto& f){return !f.p4;});

    auto row = [&](const std::string& lbl, long fp){
        std::cout << "  " << lbl;
        for(int i=(int)lbl.size();i<30;i++) std::cout<<' ';
        std::cout << " " << total;
        std::cout << "  " << fp;
        std::cout << "  " << (fp==0?"Si ✓":"No ✗") << "\n";
    };
    row("P1 — in-order ordenado",  fallosP1);
    row("P2 — cota de altura",     fallosP2);
    row("P3 — conteo correcto",    fallosP3);
    row("P4 — invariante BST",     fallosP4);
    std::cout << std::string(70,'=') << "\n";

    return 0;
}
