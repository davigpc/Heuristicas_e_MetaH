#include "busca_tabu_mochila.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Uso correto: " << argv[0] << " <arquivo_de_instancia>" << endl;
        return 1;
    }

    string nome_arquivo = argv[1];
    int capacidade = 0;
    vector<Item> itens = ler_arquivo_mochila(nome_arquivo, capacidade);

    if (itens.empty())
    {
        return 1;
    }

    cout << "\nIniciando Busca Tabu para o Problema da Mochila..." << endl;
    busca_tabu(itens, capacidade);

    return 0;
}