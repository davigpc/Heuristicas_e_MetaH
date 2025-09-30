#include "simulated_annealing.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Uso correto: " << argv[0] << " <nome_do_arquivo>" << endl;
        return 1;
    }

    string nome_arquivo = argv[1];

    cout << "Lendo instancia do arquivo: " << nome_arquivo << endl;
    vector<cidade> cidades = ler_arquivo_instancia(nome_arquivo);

    if (cidades.empty())
    {
        return 1;
    }

    cout << "\nIniciando Simulated Annealing..." << endl;
    simulated_annealing(cidades);

    return 0;
}