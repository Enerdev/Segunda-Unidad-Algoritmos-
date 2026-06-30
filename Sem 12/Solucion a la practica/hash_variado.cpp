#include <iostream>
#include <list>
#include <vector>
using namespace std;

class HashTable {
    int buckets;
    vector<list<int>> table;
    int hashFunction(int key) { return key % buckets; }
public:
    int collisions = 0;
    HashTable(int size) : buckets(size), table(size) {}
    void insertItem(int key) {
        int idx = hashFunction(key);
        if (!table[idx].empty()) collisions++;   // contamos colisión si el bucket ya tenía algo
        table[idx].push_back(key);
    }
    bool searchItem(int key) {
        int idx = hashFunction(key);
        int comparisons = 0;
        for (int val : table[idx]) {
            comparisons++;
            if (val == key) { 
                cout << "   (comparaciones realizadas: " << comparisons << ")\n";
                return true; 
            }
        }
        return false;
    }
    void display() {
        for (int i = 0; i < buckets; i++) {
            cout << i << ": ";
            for (int val : table[i]) cout << val << " -> ";
            cout << "NULL\n";
        }
    }
    double loadFactor(int n) { return (double)n / buckets; }
};

int main() {
    int datos[] = {15, 11, 27, 8, 12, 23, 5, 19, 34, 41};
    int n = sizeof(datos)/sizeof(datos[0]);
    int tamanos[] = {5, 7, 13};

    for (int size : tamanos) {
        cout << "===================================\n";
        cout << "Tamaño de tabla (buckets) = " << size << "\n";
        cout << "===================================\n";
        HashTable ht(size);
        for (int v : datos) ht.insertItem(v);
        ht.display();
        cout << "Factor de carga (n/buckets) = " << ht.loadFactor(n) << "\n";
        cout << "Colisiones totales = " << ht.collisions << "\n\n";
    }
}
