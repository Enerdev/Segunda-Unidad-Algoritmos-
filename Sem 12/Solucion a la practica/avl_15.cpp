#include <iostream>
using namespace std;

struct Node {
    int key, height;
    Node *left, *right;
    Node(int val): key(val), height(1), left(nullptr), right(nullptr) {}
};

int getHeight(Node* n) { return n ? n->height : 0; }
int getBalance(Node* n) { return n ? getHeight(n->left) - getHeight(n->right) : 0; }

Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    cout << "   -> Rotacion SIMPLE DERECHA en nodo " << y->key << "\n";
    return x;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    cout << "   -> Rotacion SIMPLE IZQUIERDA en nodo " << x->key << "\n";
    return y;
}

Node* insert(Node* node, int key) {
    if (!node) return new Node(key);
    if (key < node->key) node->left = insert(node->left, key);
    else if (key > node->key) node->right = insert(node->right, key);
    else return node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    if (balance > 1 && key < node->left->key) return rightRotate(node);
    if (balance < -1 && key > node->right->key) return leftRotate(node);
    if (balance > 1 && key > node->left->key) {
        cout << "   -> Rotacion DOBLE IZQUIERDA-DERECHA en nodo " << node->key << "\n";
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && key < node->right->key) {
        cout << "   -> Rotacion DOBLE DERECHA-IZQUIERDA en nodo " << node->key << "\n";
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

void preOrder(Node* root) {
    if (root) {
        cout << root->key << " ";
        preOrder(root->left);
        preOrder(root->right);
    }
}

void inOrder(Node* root) {
    if (root) {
        inOrder(root->left);
        cout << root->key << " ";
        inOrder(root->right);
    }
}

int main() {
    Node* root = nullptr;
    int valores[] = {10, 20, 30, 40, 50, 25, 5, 15, 1, 45, 35, 60, 70, 65, 3};

    for (int v : valores) {
        cout << "Insertando " << v << ":\n";
        root = insert(root, v);
    }

    cout << "\nPreorden del arbol AVL final: ";
    preOrder(root);
    cout << "\nInorden del arbol AVL final (debe salir ordenado): ";
    inOrder(root);
    cout << "\nAltura final del arbol: " << getHeight(root) << endl;
}
