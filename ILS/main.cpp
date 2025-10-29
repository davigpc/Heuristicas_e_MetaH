#include "ils_tsp.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

int main(int argc, char *argv[])
{

    if (argc != 3) {
        cerr << "Uso correto: " << argv[0] << " <arquivo_instancia.tsp> <ils_max_iter>" << endl;
        cerr << "Exemplo:   " << argv[0] << " berlin52.tsp 100" << endl;
        return 1;
    }

    string nome_arquivo = argv[1];
    int ils_max_iter = 100;

    try {
        ils_max_iter = stoi(argv[2]);
        if (ils_max_iter <= 0) {
             cerr << "Erro: <ils_max_iter> deve ser um inteiro positivo." << endl;
             return 1;
        }
    } catch (const invalid_argument& e) {
        cerr << "Erro: <ils_max_iter> deve ser um numero inteiro. " << e.what() << endl;
        return 1;
    } catch (const out_of_range& e) {
        cerr << "Erro: <ils_max_iter> fora do intervalo permitido. " << e.what() << endl;
        return 1;
    }

    cout << "Lendo instancia do arquivo: " << nome_arquivo << endl;
    vector<cidade> cidades = ler_arquivo_tsp(nome_arquivo);

    if (cidades.empty()) {
        return 1;
    }

    cout << "\nIniciando Iterated Local Search (ILS) com max_iter = " << ils_max_iter << "..." << endl;
    iterated_local_search_tsp(cidades, ils_max_iter);

    return 0;
}