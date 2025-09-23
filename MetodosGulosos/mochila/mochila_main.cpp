#include "mochila_algoritmos.h"
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

    cout << "\n--- COMPARACAO DAS HEURISTICAS GULOSAS ---" << endl;

    // --- Estratégia 1: Melhor Razão (Valor/Peso) ---
    vector<int> solucao_razao = gerar_solucao_gulosa(itens, capacidade);
    pair<int, int> resultado_razao = avaliar_solucao(solucao_razao, itens, capacidade);
    cout << "\n1) Estrategia por Razao (Valor/Peso):" << endl;
    cout << "   - Lucro Total: " << resultado_razao.first << endl;
    cout << "   - Peso Total: " << resultado_razao.second << " / " << capacidade << endl;

    // --- Estratégia 2: Itens Mais Leves ---
    vector<int> solucao_leve = gerar_solucao_gulosa_peso_leve(itens, capacidade);
    pair<int, int> resultado_leve = avaliar_solucao(solucao_leve, itens, capacidade);
    cout << "\n2) Estrategia por Itens Mais Leves:" << endl;
    cout << "   - Lucro Total: " << resultado_leve.first << endl;
    cout << "   - Peso Total: " << resultado_leve.second << " / " << capacidade << endl;

    cout << "\n------------------------------------------------" << endl;
    cout << "\nIniciando Busca Local a partir da solucao por RAZAO (geralmente melhor)..." << endl;
    
    // A busca local pode começar a partir da melhor solução gulosa encontrada
    busca_local_mochila(itens, capacidade, 1000);

    return 0;
}