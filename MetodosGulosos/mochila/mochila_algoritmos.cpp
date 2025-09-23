#include "mochila_algoritmos.h"
#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>

using namespace std;

// --- MÓDULO DE LEITURA E AVALIAÇÃO ---

vector<Item> ler_arquivo_mochila(const string &nome_arquivo, int &capacidade_saida)
{
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open())
    {
        cerr << "Erro: Nao foi possivel abrir o arquivo '" << nome_arquivo << "'" << endl;
        return {};
    }
    int num_itens;
    arquivo >> num_itens >> capacidade_saida;
    vector<Item> itens;
    int v, p;
    for (int i = 0; i < num_itens; ++i)
    {
        arquivo >> v >> p;
        itens.push_back({i + 1, v, p});
    }
    arquivo.close();
    cout << "Arquivo '" << nome_arquivo << "' lido com sucesso." << endl;
    cout << "Itens: " << num_itens << ", Capacidade da Mochila: " << capacidade_saida << endl;
    return itens;
}

pair<int, int> avaliar_solucao(const vector<int> &solucao, const vector<Item> &itens, int capacidade)
{
    int valor_total = 0;
    int peso_total = 0;
    for (size_t i = 0; i < solucao.size(); ++i)
    {
        if (solucao[i] == 1)
        {
            valor_total += itens[i].valor;
            peso_total += itens[i].peso;
        }
    }
    if (peso_total > capacidade)
    {
        return {0, peso_total};
    }
    return {valor_total, peso_total};
}

// --- ALGORITMOS ---

vector<int> gerar_solucao_gulosa(const vector<Item> &itens, int capacidade)
{
    int num_itens = itens.size();
    if (num_itens == 0)
    {
        return {};
    }
    vector<int> indices(num_itens);
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&](int a, int b) {
        double razao_a = (itens[a].peso > 0) ? static_cast<double>(itens[a].valor) / itens[a].peso : 0;
        double razao_b = (itens[b].peso > 0) ? static_cast<double>(itens[b].valor) / itens[b].peso : 0;
        return razao_a > razao_b;
    });
    vector<int> solucao(num_itens, 0);
    int peso_atual = 0;
    for (int indice_item : indices)
    {
        if (peso_atual + itens[indice_item].peso <= capacidade)
        {
            solucao[indice_item] = 1;
            peso_atual += itens[indice_item].peso;
        }
    }
    return solucao;
}

void analisar_otimo_local(const vector<int> &solucao, const vector<Item> &itens, int capacidade)
{
    int valor_solucao = avaliar_solucao(solucao, itens, capacidade).first;
    cout << "\n--- Analise do Otimo Local Encontrado ---" << endl;
    cout << "A solucao e um \"otimo local\", significando que nenhum vizinho direto e melhor:" << endl;
    cout << "Avaliando vizinhos da solucao ";
    for (int bit : solucao) cout << bit;
    cout << " (Lucro: " << valor_solucao << "):" << endl;
    for (size_t i = 0; i < itens.size(); ++i)
    {
        vector<int> vizinho = solucao;
        vizinho[i] = 1 - vizinho[i];
        pair<int, int> resultado_vizinho = avaliar_solucao(vizinho, itens, capacidade);
        int valor_vizinho = resultado_vizinho.first;
        int peso_vizinho = resultado_vizinho.second;
        cout << "  - Vizinho ";
        for (int bit : vizinho) cout << bit;
        cout << " (flip item " << i + 1 << "): Lucro: " << valor_vizinho << ", Peso: " << peso_vizinho;
        if (peso_vizinho > capacidade)
        {
            cout << ". Status: Invalido (excede capacidade)." << endl;
        }
        else if (valor_vizinho <= valor_solucao)
        {
            cout << ". Status: Pior ou igual." << endl;
        }
    }
    cout << "\nConclusao: Nenhum vizinho e melhor. O algoritmo parou corretamente." << endl;
}

vector<int> gerar_solucao_gulosa_peso_leve(const vector<Item> &itens, int capacidade)
{
    int num_itens = itens.size();
    if (num_itens == 0)
    {
        return {};
    }

    vector<int> indices(num_itens);
    iota(indices.begin(), indices.end(), 0);

    sort(indices.begin(), indices.end(), [&](int a, int b) {
        if (itens[a].peso == itens[b].peso) {
            return itens[a].valor > itens[b].valor; 
        }
        return itens[a].peso < itens[b].peso; // Critério principal: menor peso primeiro
    });

    vector<int> solucao(num_itens, 0);
    int peso_atual = 0;
    for (int indice_item : indices)
    {
        if (peso_atual + itens[indice_item].peso <= capacidade)
        {
            solucao[indice_item] = 1;
            peso_atual += itens[indice_item].peso;
        }
    }

    return solucao;
}

void busca_local_mochila(const vector<Item> &itens, int capacidade, int max_iteracoes)
{
    int num_itens = itens.size();
    vector<int> solucao_atual = gerar_solucao_gulosa(itens, capacidade);
    int valor_atual = avaliar_solucao(solucao_atual, itens, capacidade).first;
    cout << "\nIniciando busca local..." << endl;
    cout << "Solucao Inicial (gulosa) -> Valor: " << valor_atual << endl;
    for (int i = 0; i < max_iteracoes; ++i)
    {
        vector<int> melhor_vizinho;
        int melhor_valor_vizinho = -1;
        for (int j = 0; j < num_itens; ++j)
        {
            vector<int> vizinho_temp = solucao_atual;
            vizinho_temp[j] = 1 - vizinho_temp[j];
            int valor_vizinho = avaliar_solucao(vizinho_temp, itens, capacidade).first;
            if (valor_vizinho > melhor_valor_vizinho)
            {
                melhor_valor_vizinho = valor_vizinho;
                melhor_vizinho = vizinho_temp;
            }
        }
        if (melhor_valor_vizinho > valor_atual)
        {
            solucao_atual = melhor_vizinho;
            valor_atual = melhor_valor_vizinho;
            cout << "Iteracao " << i + 1 << ": Melhora encontrada. Novo valor: " << valor_atual << endl;
        }
        else
        {
            cout << "Iteracao " << i + 1 << ": Nenhum vizinho melhor. Otimo local atingido." << endl;
            break;
        }
    }
    cout << "\n--- Busca Finalizada ---" << endl;
    pair<int, int> resultado_final = avaliar_solucao(solucao_atual, itens, capacidade);
    cout << "Melhor solucao encontrada:" << endl;
    cout << "  - Representacao binaria: ";
    for (int bit : solucao_atual) cout << bit;
    cout << endl;
    cout << "  - Itens na mochila (ID): ";
    for (size_t i = 0; i < solucao_atual.size(); ++i)
    {
        if (solucao_atual[i] == 1)
        {
            cout << itens[i].id << " ";
        }
    }
    cout << endl;
    cout << "  - Lucro Total: " << resultado_final.first << endl;
    cout << "  - Peso Total: " << resultado_final.second << " / " << capacidade << endl;
    if (num_itens <= 15)
        analisar_otimo_local(solucao_atual, itens, capacidade);
}