#include "vns_tsp.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept> // Para stoi

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3) {
        cerr << "Uso correto: " << argv[0] << " <arquivo_instancia.tsp> <vns_max_iter>" << endl;
        cerr << "Exemplo:   " << argv[0] << " tsp_51 50" << endl;
        return 1;
    }

    string nome_arquivo = argv[1];
    int vns_max_iter = 50;

    try {
        vns_max_iter = stoi(argv[2]);
        if (vns_max_iter <= 0) {
             cerr << "Erro: <vns_max_iter> deve ser um inteiro positivo." << endl;
             return 1;
        }
    } catch (const invalid_argument& e) {
        cerr << "Erro: <vns_max_iter> deve ser um numero inteiro. " << e.what() << endl;
        return 1;
    } catch (const out_of_range& e) {
        cerr << "Erro: <vns_max_iter> fora do intervalo permitido. " << e.what() << endl;
        return 1;
    }

    cout << "Lendo instancia do arquivo: " << nome_arquivo << endl;
    vector<cidade> cidades = ler_arquivo_tsp(nome_arquivo);

    if (cidades.empty()) {
        return 1;
    }

    cout << "\nIniciando Variable Neighborhood Search (VNS) com max_iter = " << vns_max_iter << "..." << endl;
    variable_neighborhood_search_tsp(cidades, vns_max_iter);

    return 0;
}