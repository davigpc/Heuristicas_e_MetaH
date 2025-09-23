#include <iostream>
#include <vector>
#include <string>
#include "tsp_algoritmos.h"

using namespace std;

void imprimir_rota(const vector<int> &solucao, const vector<cidade> &cidades)
{
    for (int id_idx : solucao)
    {
        cout << cidades[id_idx].id << " -> ";
    }
    cout << cidades[solucao.front()].id << endl;
}

int main()
{
    string nome_arquivo;
    cout << "Digite o nome do arquivo de instancia do TSP: ";
    cin >> nome_arquivo;

    vector<cidade> cidades = ler_arquivo_instancia(nome_arquivo);

    if (cidades.empty())
    {
        return 1;
    }

    cout << "\n--- Opcao 1: Guloso com Vizinho Mais Proximo ---" << endl;
    vector<int> solucao_proximo = gerar_solucao_vizinho_proximo(cidades);
    double custo_proximo = calcular_custo_total(solucao_proximo, cidades);
    cout << "Custo inicial (Vizinho Proximo): " << custo_proximo << endl;
    
    cout << "\nAplicando Busca Local para refinar..." << endl;
    double custo_final = busca_local(solucao_proximo, cidades);
    cout << "Custo final (apos Busca Local): " << custo_final << endl;
    cout << "Rota Final: ";
    imprimir_rota(solucao_proximo, cidades);
    
    cout << "\n----------------------------------------------------" << endl;

    cout << "\n--- Opcao 2: Guloso com Vizinho Mais Distante ---" << endl;
    vector<int> solucao_distante = gerar_solucao_vizinho_distante(cidades);
    double custo_distante = calcular_custo_total(solucao_distante, cidades);
    cout << "Custo (Vizinho Distante): " << custo_distante << endl;
    cout << "Rota: ";
    imprimir_rota(solucao_distante, cidades);

    return 0;
}