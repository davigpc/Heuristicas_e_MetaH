#include "tsp_algoritmos.h"
#include <cmath>
#include <limits>
#include <fstream>
#include <iostream>

using namespace std;

// --- Implementação das Funções Utilitárias ---

double calcular_distancia(const cidade &c1, const cidade &c2)
{
    int xd = c1.x - c2.x;
    int yd = c1.y - c2.y;
    return (int)(sqrt(xd * xd + yd * yd) + 0.5);
}

double calcular_custo_total(const vector<int> &solucao, const vector<cidade> &cidades)
{
    double custo = 0.0;
    for (size_t i = 0; i < solucao.size() - 1; ++i)
    {
        custo += calcular_distancia(cidades[solucao[i]], cidades[solucao[i + 1]]);
    }
    custo += calcular_distancia(cidades[solucao.back()], cidades[solucao.front()]);
    return custo;
}

vector<cidade> ler_arquivo_instancia(const string &nome_arquivo)
{
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open())
    {
        cerr << "Erro: Nao foi possivel abrir o arquivo " << nome_arquivo << endl;
        return {};
    }

    string linha;
    while (getline(arquivo, linha) && linha.find("NODE_COORD_SECTION") == string::npos)
    {
        // Pula o cabeçalho
    }

    vector<cidade> cidades;
    int id;
    double x, y;
    while (arquivo >> id >> x >> y)
    {
        cidades.push_back({id, x, y});
    }
    arquivo.close();

    if (cidades.empty())
    {
        cerr << "Erro: Nenhuma cidade foi lida do arquivo." << endl;
    }
    
    return cidades;
}

// --- Implementação dos Algoritmos de Construção ---

vector<int> gerar_solucao_vizinho_proximo(const vector<cidade> &cidades)
{
    size_t num_cidades = cidades.size();
    if (num_cidades == 0) return {};

    vector<int> solucao;
    solucao.reserve(num_cidades);
    vector<bool> visitados(num_cidades, false);

    int cidade_atual = 0;
    solucao.push_back(cidade_atual);
    visitados[cidade_atual] = true;

    for (size_t i = 0; i < num_cidades - 1; ++i)
    {
        double menor_distancia = numeric_limits<double>::max();
        int prox_cidade = -1;
        for (size_t j = 0; j < num_cidades; ++j)
        {
            if (!visitados[j])
            {
                double dist = calcular_distancia(cidades[cidade_atual], cidades[j]);
                if (dist < menor_distancia)
                {
                    menor_distancia = dist;
                    prox_cidade = j;
                }
            }
        }
        if (prox_cidade != -1)
        {
            cidade_atual = prox_cidade;
            solucao.push_back(cidade_atual);
            visitados[cidade_atual] = true;
        }
    }
    return solucao;
}

vector<int> gerar_solucao_vizinho_distante(const vector<cidade> &cidades)
{
    size_t num_cidades = cidades.size();
    if (num_cidades == 0) return {};

    vector<int> solucao;
    solucao.reserve(num_cidades);
    vector<bool> visitados(num_cidades, false);

    int cidade_atual = 0;
    solucao.push_back(cidade_atual);
    visitados[cidade_atual] = true;

    for (size_t i = 0; i < num_cidades - 1; ++i)
    {
        double maior_distancia = 0.0;
        int prox_cidade = -1;
        for (size_t j = 0; j < num_cidades; ++j)
        {
            if (!visitados[j])
            {
                double dist = calcular_distancia(cidades[cidade_atual], cidades[j]);
                if (dist > maior_distancia)
                {
                    maior_distancia = dist;
                    prox_cidade = j;
                }
            }
        }
        if (prox_cidade != -1)
        {
            cidade_atual = prox_cidade;
            solucao.push_back(cidade_atual);
            visitados[cidade_atual] = true;
        }
    }
    return solucao;
}

// --- Implementação dos Algoritmos de Melhoria ---

static void troca(vector<int> &vizinho, int pos1, int pos2)
{
    swap(vizinho[pos1], vizinho[pos2]);
}

double busca_local(vector<int> &solucao, const vector<cidade> &cidades)
{
    double custo_atual = calcular_custo_total(solucao, cidades);
    bool melhorou = true;

    while (melhorou)
    {
        melhorou = false;
        for (size_t i = 0; i < cidades.size(); ++i)
        {
            for (size_t j = i + 1; j < cidades.size(); ++j)
            {
                vector<int> vizinho = solucao;
                troca(vizinho, i, j);
                double custo_vizinho = calcular_custo_total(vizinho, cidades);

                if (custo_vizinho < custo_atual)
                {
                    solucao = vizinho;
                    custo_atual = custo_vizinho;
                    melhorou = true;
                    goto proxima_iteracao;
                }
            }
        }
    proxima_iteracao:;
    }
    return custo_atual;
}