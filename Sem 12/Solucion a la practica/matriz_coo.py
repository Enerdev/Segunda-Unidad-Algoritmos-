import random

class SparseMatrix:
    def __init__(self, rows, cols):
        self.rows = rows
        self.cols = cols
        self.data = {}  # clave = (fila, col), valor = elemento

    def add(self, r, c, val):
        if val != 0:
            self.data[(r, c)] = val

    def get(self, r, c):
        return self.data.get((r, c), 0)

    def to_coo(self):
        """Devuelve la representación COO: listas paralelas (filas, columnas, valores)"""
        filas, columnas, valores = [], [], []
        for (r, c), v in sorted(self.data.items()):
            filas.append(r)
            columnas.append(c)
            valores.append(v)
        return filas, columnas, valores

    def display_coo(self):
        filas, columnas, valores = self.to_coo()
        print(f"{'Fila':>6} {'Columna':>8} {'Valor':>8}")
        for r, c, v in zip(filas, columnas, valores):
            print(f"{r:>6} {c:>8} {v:>8}")

    def memoria_estimada(self):
        # COO guarda 3 números por elemento no nulo (fila, col, valor)
        no_nulos = len(self.data)
        return no_nulos * 3, self.rows * self.cols


# Generar matriz dispersa 1000x1000 con 20 valores no nulos aleatorios
random.seed(42)
A = SparseMatrix(1000, 1000)
posiciones = set()
while len(posiciones) < 20:
    r = random.randint(0, 999)
    c = random.randint(0, 999)
    posiciones.add((r, c))

for r, c in posiciones:
    val = random.randint(1, 99)
    A.add(r, c, val)

print("Representación COO de la matriz dispersa (1000x1000, 20 no nulos):\n")
A.display_coo()

coo_size, dense_size = A.memoria_estimada()
print(f"\nElementos no nulos almacenados (COO): {coo_size} valores (3 por entrada)")
print(f"Elementos totales en representación densa: {dense_size:,}")
print(f"Ahorro de memoria: {(1 - coo_size/dense_size)*100:.4f}%")
