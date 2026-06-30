import tkinter as tk
from tkinter import messagebox, simpledialog

class NodoDecision:
    def __init__(self, pregunta=None, si=None, no=None, resultado=None):
        self.pregunta = pregunta
        self.si = si
        self.no = no
        self.resultado = resultado

# Árbol base
hoja_gripe = NodoDecision(resultado="Gripe")
hoja_resfriado = NodoDecision(resultado="Resfriado")
hoja_sano = NodoDecision(resultado="Sano")
nodo_cabeza = NodoDecision(pregunta="¿Tiene dolor de cabeza?", si=hoja_gripe, no=hoja_resfriado)
arbol = NodoDecision(pregunta="¿Tiene fiebre?", si=nodo_cabeza, no=hoja_sano)

# Aplicación
class DiagnosticoApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Diagnóstico Médico con Árbol de Decisión")
        self.nodo_actual = arbol

        self.label_pregunta = tk.Label(root, text=self.nodo_actual.pregunta, font=("Arial", 14), wraplength=400)
        self.label_pregunta.pack(pady=10)

        self.boton_si = tk.Button(root, text="Sí", command=self.responder_si, width=15, bg="lightgreen")
        self.boton_si.pack(pady=5)

        self.boton_no = tk.Button(root, text="No", command=self.responder_no, width=15, bg="lightcoral")
        self.boton_no.pack(pady=5)

        self.boton_reiniciar = tk.Button(root, text="Reiniciar", command=self.reiniciar, state="disabled")
        self.boton_reiniciar.pack(pady=10)

        self.boton_agregar = tk.Button(root, text="Agregar enfermedad", command=self.agregar_enfermedad)
        self.boton_agregar.pack(pady=10)

    def mostrar_resultado(self, resultado):
        respuesta = messagebox.askyesno("Resultado", f"Diagnóstico: {resultado}\n¿Deseas agregar una nueva enfermedad desde aquí?")
        self.label_pregunta.config(text=f"Diagnóstico: {resultado}")
        self.boton_si.config(state="disabled")
        self.boton_no.config(state="disabled")
        self.boton_reiniciar.config(state="normal")
        if respuesta:
            self.agregar_enfermedad()

    def responder_si(self):
        if self.nodo_actual.si is None:
            return
        if self.nodo_actual.si.resultado:
            self.mostrar_resultado(self.nodo_actual.si.resultado)
        else:
            self.nodo_actual = self.nodo_actual.si
            self.label_pregunta.config(text=self.nodo_actual.pregunta)

    def responder_no(self):
        if self.nodo_actual.no is None:
            return
        if self.nodo_actual.no.resultado:
            self.mostrar_resultado(self.nodo_actual.no.resultado)
        else:
            self.nodo_actual = self.nodo_actual.no
            self.label_pregunta.config(text=self.nodo_actual.pregunta)

    def reiniciar(self):
        self.nodo_actual = arbol
        self.label_pregunta.config(text=self.nodo_actual.pregunta)
        self.boton_si.config(state="normal")
        self.boton_no.config(state="normal")
        self.boton_reiniciar.config(state="disabled")

    def agregar_enfermedad(self):
        # Pide pregunta de diagnóstico
        nueva_pregunta = simpledialog.askstring("Agregar", "¿Cuál es la nueva pregunta para diferenciar la enfermedad?")
        if not nueva_pregunta:
            return

        nueva_enfermedad = simpledialog.askstring("Agregar", "¿Cuál es el nombre de la nueva enfermedad?")
        if not nueva_enfermedad:
            return

        rama = messagebox.askquestion("Rama", "¿La respuesta 'Sí' debe conducir a la nueva enfermedad?")

        nuevo_nodo = NodoDecision(resultado=nueva_enfermedad)
        viejo_resultado = self.nodo_actual.resultado

        # Convierte hoja en nodo con pregunta nueva
        self.nodo_actual.pregunta = nueva_pregunta
        if rama == 'yes':
            self.nodo_actual.si = nuevo_nodo
            self.nodo_actual.no = NodoDecision(resultado=viejo_resultado)
        else:
            self.nodo_actual.no = nuevo_nodo
            self.nodo_actual.si = NodoDecision(resultado=viejo_resultado)

        self.nodo_actual.resultado = None
        self.label_pregunta.config(text=self.nodo_actual.pregunta)
        self.boton_si.config(state="normal")
        self.boton_no.config(state="normal")
        self.boton_reiniciar.config(state="disabled")
        messagebox.showinfo("Éxito", f"Enfermedad '{nueva_enfermedad}' añadida correctamente.")

# Ejecutar app
if __name__ == "__main__":
    root = tk.Tk()
    app = DiagnosticoApp(root)
    root.mainloop()
