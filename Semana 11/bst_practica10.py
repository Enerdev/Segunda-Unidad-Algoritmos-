"""
Práctica N.° 10 — Inducción Estructural
Verificación Formal del Índice de Matrículas SIGA-UNAP
Algoritmos y Estructuras de Datos — SIS210
Dr. Aldo Hernán Zanabria Gálvez | UNAP 2025
"""

import random
import math
from enum import Enum
from dataclasses import dataclass
from typing import Optional, List


# ─── Clases base del dominio ─────────────────────────────────────────────────

class EstadoAcademico(Enum):
    ACTIVO = "ACTIVO"
    RETIRADO = "RETIRADO"
    EGRESADO = "EGRESADO"


@dataclass
class Estudiante:
    codigo: int
    nombre: str
    apellido: str
    promedio: float
    creditos: int
    estado: EstadoAcademico
    semestre: str

    def __repr__(self):
        return f"Estudiante({self.codigo}, {self.nombre} {self.apellido})"


# ─── Nodo del BST ─────────────────────────────────────────────────────────────

class NodoBST:
    def __init__(self, dato: Estudiante):
        self.dato = dato
        self.izquierdo: Optional['NodoBST'] = None
        self.derecho: Optional['NodoBST'] = None


# ─── Árbol Académico (BST) ────────────────────────────────────────────────────

class ArbolAcademico:
    def __init__(self):
        self._raiz: Optional[NodoBST] = None

    # ── Inserción ──────────────────────────────────────────────────────────────
    def insertar(self, estudiante: Estudiante) -> None:
        self._raiz = self._insertar(self._raiz, estudiante)

    def _insertar(self, nodo: Optional[NodoBST], est: Estudiante) -> NodoBST:
        if nodo is None:
            return NodoBST(est)
        if est.codigo < nodo.dato.codigo:
            nodo.izquierdo = self._insertar(nodo.izquierdo, est)
        elif est.codigo > nodo.dato.codigo:
            nodo.derecho = self._insertar(nodo.derecho, est)
        # duplicado: ignorar
        return nodo

    # ── Eliminación ────────────────────────────────────────────────────────────
    def eliminar(self, codigo: int) -> None:
        self._raiz = self._eliminar(self._raiz, codigo)

    def _eliminar(self, nodo: Optional[NodoBST], codigo: int) -> Optional[NodoBST]:
        if nodo is None:
            return None
        if codigo < nodo.dato.codigo:
            nodo.izquierdo = self._eliminar(nodo.izquierdo, codigo)
        elif codigo > nodo.dato.codigo:
            nodo.derecho = self._eliminar(nodo.derecho, codigo)
        else:
            # Nodo encontrado
            if nodo.izquierdo is None:
                return nodo.derecho
            if nodo.derecho is None:
                return nodo.izquierdo
            # Dos hijos: usar sucesor in-order (mínimo del subárbol derecho)
            sucesor = self._minimo(nodo.derecho)
            nodo.dato = sucesor.dato
            nodo.derecho = self._eliminar(nodo.derecho, sucesor.dato.codigo)
        return nodo

    def _minimo(self, nodo: NodoBST) -> NodoBST:
        while nodo.izquierdo is not None:
            nodo = nodo.izquierdo
        return nodo

    # ── Recorrido in-order ─────────────────────────────────────────────────────
    def in_order(self) -> List[Estudiante]:
        resultado = []
        self._in_order(self._raiz, resultado)
        return resultado

    def _in_order(self, nodo: Optional[NodoBST], lista: list) -> None:
        if nodo is None:
            return
        self._in_order(nodo.izquierdo, lista)
        lista.append(nodo.dato)
        self._in_order(nodo.derecho, lista)

    # ── Altura ─────────────────────────────────────────────────────────────────
    def altura(self) -> int:
        return self._altura(self._raiz)

    def _altura(self, nodo: Optional[NodoBST]) -> int:
        if nodo is None:
            return -1
        return 1 + max(self._altura(nodo.izquierdo), self._altura(nodo.derecho))


# ─── Actividad 3: Framework de verificación ───────────────────────────────────

def es_bst(nodo: Optional[NodoBST],
           minimo: float = float('-inf'),
           maximo: float = float('inf')) -> bool:
    """P4/P5: Verifica la invariante BST recursivamente."""
    if nodo is None:
        return True
    if not (minimo < nodo.dato.codigo < maximo):
        return False
    return (es_bst(nodo.izquierdo, minimo, nodo.dato.codigo) and
            es_bst(nodo.derecho, nodo.dato.codigo, maximo))


def es_inorder_ordenado(arbol: ArbolAcademico) -> bool:
    """P1: La secuencia in-order debe ser estrictamente ascendente."""
    codigos = [e.codigo for e in arbol.in_order()]
    return codigos == sorted(codigos)


def contar_recursivo(nodo: Optional[NodoBST]) -> int:
    """P3: |T| = |T_L| + |T_R| + 1."""
    if nodo is None:
        return 0
    return 1 + contar_recursivo(nodo.izquierdo) + contar_recursivo(nodo.derecho)


def cota_altura_cumplida(arbol: ArbolAcademico, n: int) -> bool:
    """P2: h(T) >= ceil(log2(n+1)) - 1."""
    if n == 0:
        return True
    cota = math.ceil(math.log2(n + 1)) - 1
    return arbol.altura() >= cota


def verificar_propiedades(arbol: ArbolAcademico, n_esperado: int) -> dict:
    """Ejecuta las 4 verificaciones runtime y retorna diccionario de resultados."""
    return {
        'P1_inorder_ordenado': es_inorder_ordenado(arbol),
        'P2_cota_altura':      cota_altura_cumplida(arbol, n_esperado),
        'P3_conteo_correcto':  contar_recursivo(arbol._raiz) == n_esperado,
        'P4_es_bst':           es_bst(arbol._raiz),
    }


# ─── Actividad 4: Fuzzing — 500 casos, 50 operaciones c/u ─────────────────────

def fuzz_test(n_casos: int = 500, n_operaciones: int = 50, semilla: int = 42):
    """
    Simula n_casos secuencias de matrícula/retiro aleatorias,
    verificando P1-P4 tras CADA operación individual.
    """
    random.seed(semilla)
    fallos = []
    total_verificaciones = 0

    for caso in range(n_casos):
        arbol = ArbolAcademico()
        codigos_activos: set = set()
        codigos_pool = list(range(20_000_000, 20_000_000 + n_operaciones * 4))
        random.shuffle(codigos_pool)

        for op in range(n_operaciones):
            accion = random.choice(['insertar', 'insertar', 'eliminar'])

            if accion == 'insertar' or not codigos_activos:
                cod = codigos_pool.pop()
                est = Estudiante(cod, 'T', 'T', 15.0, 100,
                                 EstadoAcademico.ACTIVO, '2024-I')
                arbol.insertar(est)
                codigos_activos.add(cod)
                accion = 'insertar'
            else:
                cod = random.choice(list(codigos_activos))
                arbol.eliminar(cod)
                codigos_activos.discard(cod)
                accion = 'eliminar'

            resultados = verificar_propiedades(arbol, len(codigos_activos))
            total_verificaciones += 1

            if not all(resultados.values()):
                prop_fallidas = [k for k, v in resultados.items() if not v]
                fallos.append({
                    'caso': caso,
                    'operacion': op,
                    'accion': accion,
                    'codigo': cod,
                    'n_nodos': len(codigos_activos),
                    'propiedades_fallidas': prop_fallidas,
                    'resultados': resultados,
                })

    return fallos, total_verificaciones


# ─── Ejecución principal ──────────────────────────────────────────────────────

if __name__ == '__main__':
    print("=" * 70)
    print("  PRÁCTICA N.° 10 — Inducción Estructural — UNAP 2025")
    print("  Verificación Formal del Índice de Matrículas SIGA-UNAP")
    print("=" * 70)

    # ── Demo rápida de las propiedades con árbol pequeño ──────────────────────
    print("\n[ACTIVIDAD 3] Verificación con árbol de demostración:")
    arbol_demo = ArbolAcademico()
    datos_demo = [
        Estudiante(20240005, 'Ana',    'Quispe',  17.5, 120, EstadoAcademico.ACTIVO, '2024-I'),
        Estudiante(20240003, 'Luis',   'Mamani',  15.0, 100, EstadoAcademico.ACTIVO, '2024-I'),
        Estudiante(20240008, 'María',  'Condori', 18.0, 130, EstadoAcademico.ACTIVO, '2024-I'),
        Estudiante(20240001, 'Pedro',  'Huanca',  14.5,  90, EstadoAcademico.ACTIVO, '2024-I'),
        Estudiante(20240004, 'Rosa',   'Apaza',   16.0, 110, EstadoAcademico.ACTIVO, '2024-I'),
        Estudiante(20240007, 'Carlos', 'Lupaca',  13.0,  80, EstadoAcademico.ACTIVO, '2024-I'),
        Estudiante(20240009, 'Julia',  'Ticona',  19.0, 140, EstadoAcademico.ACTIVO, '2024-I'),
    ]
    for e in datos_demo:
        arbol_demo.insertar(e)

    n_demo = len(datos_demo)
    res_demo = verificar_propiedades(arbol_demo, n_demo)
    print(f"  Nodos insertados : {n_demo}")
    print(f"  Altura del árbol : {arbol_demo.altura()}")
    print(f"  In-order (códigos): {[e.codigo for e in arbol_demo.in_order()]}")
    for prop, ok in res_demo.items():
        estado = "PASS ✓" if ok else "FAIL ✗"
        print(f"  {prop:<28}: {estado}")

    # ── Eliminación y reverificación ──────────────────────────────────────────
    print("\n  Eliminando nodo 20240005 (tiene dos hijos — Caso 3 de P5):")
    arbol_demo.eliminar(20240005)
    res_post = verificar_propiedades(arbol_demo, n_demo - 1)
    print(f"  In-order post-eliminar: {[e.codigo for e in arbol_demo.in_order()]}")
    for prop, ok in res_post.items():
        estado = "PASS ✓" if ok else "FAIL ✗"
        print(f"  {prop:<28}: {estado}")

    # ── Fuzzing masivo ─────────────────────────────────────────────────────────
    print("\n[ACTIVIDAD 4] Fuzzing: 500 secuencias × 50 operaciones...")
    fallos, total = fuzz_test(n_casos=500, n_operaciones=50, semilla=42)

    print(f"\n  Verificaciones ejecutadas : {total:,}")
    print(f"  Fallos detectados         : {len(fallos)}")

    if fallos:
        print(f"\n  PRIMER FALLO DETECTADO:")
        f = fallos[0]
        print(f"    Caso #{f['caso']}, Operación #{f['operacion']}")
        print(f"    Acción: {f['accion']} | Código: {f['codigo']}")
        print(f"    Nodos tras operación: {f['n_nodos']}")
        print(f"    Propiedades fallidas: {f['propiedades_fallidas']}")
    else:
        print("\n  ✓ TODAS LAS PROPIEDADES VERIFICADAS")
        print("  El índice de matrículas certifica P1-P4 en los 25,000 casos.")

    # ── Tabla de certificación ────────────────────────────────────────────────
    print("\n" + "=" * 70)
    print("  TABLA DE CERTIFICACIÓN — Python")
    print("=" * 70)
    props = ['P1_inorder_ordenado', 'P2_cota_altura',
             'P3_conteo_correcto', 'P4_es_bst']
    labels = ['P1 — in-order ordenado', 'P2 — cota de altura',
              'P3 — conteo correcto',   'P4 — invariante BST']
    print(f"  {'Propiedad':<28} {'Casos':>8} {'Fallos Python':>14} {'Certificada':>12}")
    print("  " + "-" * 66)
    for prop, label in zip(props, labels):
        fallos_prop = sum(1 for f in fallos if prop in f['propiedades_fallidas'])
        cert = "Sí ✓" if fallos_prop == 0 else "No ✗"
        print(f"  {label:<28} {total:>8,} {fallos_prop:>14} {cert:>12}")
    print("=" * 70)
