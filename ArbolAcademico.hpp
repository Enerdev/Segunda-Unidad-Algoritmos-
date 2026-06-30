// ============================================================
// PRÁCTICA N.° 09 — BST Sistema Académico UNA-PUNO (C++17)
// Estudiante: Flores Flores, Emerson Aldair | Cód. 240439
// SIS210 — Algoritmos y Estructuras de Datos — UNAP 2025
// ============================================================
#pragma once
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>

namespace una_puno {

// ── Estado académico ─────────────────────────────────────────
enum class EstadoAcademico { ACTIVO, EGRESADO, RETIRADO, SUSPENDIDO };

inline std::string estadoStr(EstadoAcademico e) {
    switch(e) {
        case EstadoAcademico::ACTIVO:     return "ACTIVO";
        case EstadoAcademico::EGRESADO:   return "EGRESADO";
        case EstadoAcademico::RETIRADO:   return "RETIRADO";
        case EstadoAcademico::SUSPENDIDO: return "SUSPENDIDO";
        default:                          return "DESCONOCIDO";
    }
}

// ── Datos del expediente ──────────────────────────────────────
struct Estudiante {
    int            codigo;
    std::string    nombre;
    std::string    escuela;
    float          ppa;
    int            creditos;
    EstadoAcademico estado;
    std::string    semestre_ingreso;

    Estudiante(int cod, std::string nom, std::string esc,
               float pp, int cred, EstadoAcademico est, std::string sem)
        : codigo(cod), nombre(std::move(nom)), escuela(std::move(esc)),
          ppa(pp), creditos(cred), estado(est),
          semestre_ingreso(std::move(sem))
    {
        if (cod < 10000000 || cod > 29999999)
            throw std::invalid_argument("Codigo invalido: " + std::to_string(cod));
        if (pp < 0.0f || pp > 20.0f)
            throw std::invalid_argument("PPA fuera de rango [0,20]");
    }

    void print() const {
        std::cout << std::left
                  << std::setw(12) << codigo
                  << std::setw(35) << nombre
                  << std::setw(20) << escuela
                  << "PPA:" << std::fixed << std::setprecision(1) << ppa
                  << "  " << estadoStr(estado) << '\n';
    }
};

// ── Nodo del BST ──────────────────────────────────────────────
struct NodoBST {
    Estudiante               dato;
    std::unique_ptr<NodoBST> izquierdo;
    std::unique_ptr<NodoBST> derecho;

    explicit NodoBST(Estudiante e)
        : dato(std::move(e)), izquierdo(nullptr), derecho(nullptr) {}
};

// ── Clase ArbolAcademico ──────────────────────────────────────
class ArbolAcademico {
public:
    ArbolAcademico() = default;

    // ── INSERTAR ────────────────────────────────────────────────
    void insertar(Estudiante e) {
        insertar_(raiz_, std::move(e));
    }

    // ── IN-ORDER ─────────────────────────────────────────────────
    std::vector<Estudiante> inOrder() const {
        std::vector<Estudiante> r;
        inOrder_(raiz_.get(), r);
        return r;
    }

    // ── PRE-ORDER ────────────────────────────────────────────────
    std::vector<Estudiante> preOrder() const {
        std::vector<Estudiante> r;
        preOrder_(raiz_.get(), r);
        return r;
    }

    // ── POST-ORDER ───────────────────────────────────────────────
    std::vector<Estudiante> postOrder() const {
        std::vector<Estudiante> r;
        postOrder_(raiz_.get(), r);
        return r;
    }

    // ── BFS ──────────────────────────────────────────────────────
    std::vector<Estudiante> bfs() const {
        std::vector<Estudiante> resultado;
        if (!raiz_) return resultado;
        std::queue<const NodoBST*> cola;
        cola.push(raiz_.get());
        while (!cola.empty()) {
            const NodoBST* curr = cola.front(); cola.pop();
            resultado.push_back(curr->dato);
            if (curr->izquierdo) cola.push(curr->izquierdo.get());
            if (curr->derecho)   cola.push(curr->derecho.get());
        }
        return resultado;
    }

    // ── ALTURA ───────────────────────────────────────────────────
    int  altura()    const { return altura_(raiz_.get()); }
    bool estaVacio() const { return raiz_ == nullptr; }

    // ── BUSCAR ───────────────────────────────────────────────────
    std::optional<Estudiante> buscar(int codigo) const {
        const NodoBST* n = buscar_(raiz_.get(), codigo);
        if (n) return n->dato;
        return std::nullopt;
    }

    // ── ELIMINAR ─────────────────────────────────────────────────
    void eliminar(int codigo) {
        if (!buscar(codigo))
            throw std::runtime_error("Codigo no encontrado: " + std::to_string(codigo));
        raiz_ = eliminar_(std::move(raiz_), codigo);
    }

    // ── MÁXIMO ───────────────────────────────────────────────────
    std::optional<Estudiante> maximo() const {
        if (!raiz_) return std::nullopt;
        const NodoBST* n = raiz_.get();
        while (n->derecho) n = n->derecho.get();
        return n->dato;
    }

    // ── RANGO DE CÓDIGO ──────────────────────────────────────────
    std::vector<Estudiante> buscarRangoCodigo(int cod_min, int cod_max) const {
        std::vector<Estudiante> resultado;
        rango_(raiz_.get(), cod_min, cod_max, resultado);
        return resultado;
    }

    // ── CONSULTA POR RANGO DE PPA ────────────────────────────────
    std::vector<Estudiante> porRangoPPA(float ppa_min, float ppa_max = 20.0f) const {
        auto todos = inOrder();
        std::vector<Estudiante> resultado;
        std::copy_if(todos.begin(), todos.end(), std::back_inserter(resultado),
                     [ppa_min, ppa_max](const Estudiante& e){
                         return e.ppa >= ppa_min && e.ppa <= ppa_max;
                     });
        return resultado;
    }

    // ── ESTADÍSTICAS ─────────────────────────────────────────────
    void estadisticas() const {
        auto todos = inOrder();
        if (todos.empty()) { std::cout << "Arbol vacio\n"; return; }
        float suma = 0.0f, mn = 20.0f, mx = 0.0f;
        int activos = 0;
        for (const auto& e : todos) {
            suma += e.ppa;
            if (e.ppa < mn) mn = e.ppa;
            if (e.ppa > mx) mx = e.ppa;
            if (e.estado == EstadoAcademico::ACTIVO) activos++;
        }
        std::cout << std::fixed << std::setprecision(2)
                  << "  total_nodos  : " << todos.size()          << '\n'
                  << "  altura       : " << altura()               << '\n'
                  << "  ppa_promedio : " << suma / todos.size()    << '\n'
                  << "  ppa_minimo   : " << mn                     << '\n'
                  << "  ppa_maximo   : " << mx                     << '\n'
                  << "  activos      : " << activos                << '\n';
    }

    // ── IMPRESIÓN ASCII ──────────────────────────────────────────
    void imprimirArbol() const {
        std::cout << "\n-- Estructura del BST --\n";
        imprimir_(raiz_.get(), "", false);
    }

private:
    std::unique_ptr<NodoBST> raiz_;

    void insertar_(std::unique_ptr<NodoBST>& nodo, Estudiante e) {
        if (!nodo) { nodo = std::make_unique<NodoBST>(std::move(e)); return; }
        if (e.codigo < nodo->dato.codigo)
            insertar_(nodo->izquierdo, std::move(e));
        else if (e.codigo > nodo->dato.codigo)
            insertar_(nodo->derecho, std::move(e));
        else
            throw std::runtime_error("Codigo duplicado: " + std::to_string(e.codigo));
    }

    void inOrder_(const NodoBST* n, std::vector<Estudiante>& r) const {
        if (!n) return;
        inOrder_(n->izquierdo.get(), r);
        r.push_back(n->dato);
        inOrder_(n->derecho.get(), r);
    }

    void preOrder_(const NodoBST* n, std::vector<Estudiante>& r) const {
        if (!n) return;
        r.push_back(n->dato);
        preOrder_(n->izquierdo.get(), r);
        preOrder_(n->derecho.get(), r);
    }

    void postOrder_(const NodoBST* n, std::vector<Estudiante>& r) const {
        if (!n) return;
        postOrder_(n->izquierdo.get(), r);
        postOrder_(n->derecho.get(), r);
        r.push_back(n->dato);
    }

    int altura_(const NodoBST* n) const {
        if (!n) return -1;
        return 1 + std::max(altura_(n->izquierdo.get()), altura_(n->derecho.get()));
    }

    const NodoBST* buscar_(const NodoBST* n, int cod) const {
        if (!n || n->dato.codigo == cod) return n;
        if (cod < n->dato.codigo) return buscar_(n->izquierdo.get(), cod);
        return buscar_(n->derecho.get(), cod);
    }

    std::unique_ptr<NodoBST> eliminar_(std::unique_ptr<NodoBST> nodo, int cod) {
        if (!nodo) return nullptr;
        if (cod < nodo->dato.codigo)
            nodo->izquierdo = eliminar_(std::move(nodo->izquierdo), cod);
        else if (cod > nodo->dato.codigo)
            nodo->derecho = eliminar_(std::move(nodo->derecho), cod);
        else {
            if (!nodo->izquierdo) return std::move(nodo->derecho);
            if (!nodo->derecho)   return std::move(nodo->izquierdo);
            NodoBST* suc = minimo_(nodo->derecho.get());
            nodo->dato   = suc->dato;
            nodo->derecho = eliminar_(std::move(nodo->derecho), suc->dato.codigo);
        }
        return nodo;
    }

    NodoBST* minimo_(NodoBST* n) const {
        while (n->izquierdo) n = n->izquierdo.get();
        return n;
    }

    void rango_(const NodoBST* n, int lo, int hi,
                std::vector<Estudiante>& r) const {
        if (!n) return;
        if (n->dato.codigo > lo) rango_(n->izquierdo.get(), lo, hi, r);
        if (n->dato.codigo >= lo && n->dato.codigo <= hi) r.push_back(n->dato);
        if (n->dato.codigo < hi) rango_(n->derecho.get(), lo, hi, r);
    }

    void imprimir_(const NodoBST* n, std::string prefix, bool isLeft) const {
        if (!n) return;
        std::string conector = isLeft ? "+-- " : "`-- ";
        std::cout << prefix << conector
                  << n->dato.codigo << " [PPA:"
                  << std::fixed << std::setprecision(1) << n->dato.ppa << "]\n";
        std::string ext = isLeft ? "|   " : "    ";
        imprimir_(n->izquierdo.get(), prefix + ext, true);
        imprimir_(n->derecho.get(),   prefix + ext, false);
    }
};

} // namespace una_puno
