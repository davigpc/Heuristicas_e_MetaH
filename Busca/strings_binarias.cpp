#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

void gerar_strings_binarias_recursivo(vector<char>& arr, int n, int i) {
    if (i == n) {
        for (int k = 0; k < n; ++k) {
            cout << arr[k];
        }
        cout << endl;
        return;
    }
    arr[i] = '0';
    gerar_strings_binarias_recursivo(arr, n, i + 1);
    arr[i] = '1';
    gerar_strings_binarias_recursivo(arr, n, i + 1);
}

void gerar_strings_binarias(int n) {
    cout << "--- Gerando todas as strings binarias de tamanho " << n << " ---" << endl;
    vector<char> arr(n);
    gerar_strings_binarias_recursivo(arr, n, 0);
}

int main() {
    int n; 

    cout << "Digite o tamanho de N";
    cin >> n;

    clock_t inicio = clock();
    gerar_strings_binarias(n);
    clock_t fim = clock();

    double duracao = double(fim - inicio) / CLOCKS_PER_SEC;
    cout << "\nTempo de processamento para n = " << n << ": " << duracao << " segundos" << endl;

    return 0;
}