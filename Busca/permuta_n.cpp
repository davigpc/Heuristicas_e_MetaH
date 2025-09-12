#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

void imprimir_vetor(const vector<int>& arr) {
    for (int val : arr) {
        cout << val << " ";
    }
    cout << endl;
}

void trocar(int& a, int& b) {
    int temp = a; 
    a = b;        
    b = temp;     
}

void gerar_permutacoes(vector<int>& arr, int l, int r) {
    if (l == r) {
        imprimir_vetor(arr);
    } else {
        for (int i = l; i <= r; i++) {

            trocar(arr[l], arr[i]);

            gerar_permutacoes(arr, l + 1, r);
            
            // Backtrack: desfaz a troca usando a mesma função
            trocar(arr[l], arr[i]);
        }
    }
}

int main() {
    int n_cidades;

    cout << "Digite o numero para gerar as permutacoes: ";
    cin >> n_cidades;

    vector<int> cidades;
    for (int i = 1; i <= n_cidades; ++i) { // popula o vetor com valores de 1 a n_cidades
        cidades.push_back(i);
    }

    cout << "--- Gerando todas as permutacoes de 1 a " << n_cidades << " ---" << endl;

    clock_t inicio = clock();
    gerar_permutacoes(cidades, 0, cidades.size() - 1);
    clock_t fim = clock();

    double duracao = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "\nTempo de processamento para n = " << n_cidades << ": " << duracao << " segundos" << endl;

    return 0;
}