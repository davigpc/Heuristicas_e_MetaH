#include "grasp.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <limits>

using namespace std;

// --- Funções Utilitárias ---

static double calcular_distancia(const cidade &c1, const cidade &c2)
{
    int xd = c1.x - c2.x;
    int yd = c1.y - c2.y;
    return static_cast<int>(sqrt(xd * xd + yd * yd) + 0.5);
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

vector<cidade> ler_arquivo_instancia(const string& nome_arquivo)
{
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo " << nome_arquivo << endl;
        return {};
    }
    string linha;
    while (getline(arquivo, linha) && linha.find("NODE_COORD_SECTION") == string::npos) {}
    vector<cidade> cidades;
    int id;
    double x, y;
    while (arquivo >> id >> x >> y) {
        cidades.push_back({id, x, y});
    }
    arquivo.close();
    if (cidades.empty()) {
        cerr << "Erro: Nenhuma cidade foi lida do arquivo." << endl;
    }
    return cidades;
}

// --- Busca Local (Hill Climbing com 2-opt swap) ---

double busca_local(vector<int>& solucao, const vector<cidade>& cidades)
{
    double custo_atual = calcular_custo_total(solucao, cidades);
    bool melhorou = true;
    while (melhorou) {
        melhorou = false;
        for (size_t i = 0; i < cidades.size(); ++i) {
            for (size_t j = i + 1; j < cidades.size(); ++j) {
                vector<int> vizinho = solucao;
                swap(vizinho[i], vizinho[j]);
                double custo_vizinho = calcular_custo_total(vizinho, cidades);
                if (custo_vizinho < custo_atual) {
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

// --- Implementação do GRASP ---

vector<int> construcao_gulosa_aleatoria(const vector<cidade>& cidades, double alpha) {
    int num_cidades = cidades.size();
    if (num_cidades == 0) return {};

    vector<int> solucao;
    solucao.reserve(num_cidades);
    vector<bool> visitados(num_cidades, false);
    
    random_device rd;
    mt19937 gen(rd());
    
    // Começa de uma cidade aleatória
    uniform_int_distribution<> dist_inicio(0, num_cidades - 1);
    int cidade_atual = dist_inicio(gen);
    
    solucao.push_back(cidade_atual);
    visitados[cidade_atual] = true;

    while (solucao.size() < num_cidades) {
        double c_min = numeric_limits<double>::max();
        double c_max = 0.0;
        vector<pair<int, double>> candidatos; // {índice_cidade, custo}

        // 1. Monta a lista de candidatos (LC) e calcula os custos
        for (int i = 0; i < num_cidades; ++i) {
            if (!visitados[i]) {
                double custo = calcular_distancia(cidades[cidade_atual], cidades[i]);
                candidatos.push_back({i, custo});
                if (custo < c_min) c_min = custo;
                if (custo > c_max) c_max = custo;
            }
        }

        // 2. Define o limiar e constrói a Lista de Candidatos Restrita (LRC)
        double limite = c_min + alpha * (c_max - c_min);
        vector<int> lrc;
        for (const auto& candidato : candidatos) {
            if (candidato.second <= limite) {
                lrc.push_back(candidato.first);
            }
        }

        // 3. Seleciona um candidato aleatório da LRC
        uniform_int_distribution<> dist_lrc(0, lrc.size() - 1);
        int proxima_cidade = lrc[dist_lrc(gen)];
        
        solucao.push_back(proxima_cidade);
        visitados[proxima_cidade] = true;
        cidade_atual = proxima_cidade;
    }

    return solucao;
}


void grasp_tsp(const vector<cidade>& cidades) {
    // (a) Critério de Parada e (b) Alpha Estático
    const int GRASP_MAX = 50; // Número de iterações do GRASP
    const double ALPHA = 0.3; // Fator de aleatoriedade (0=guloso, 1=aleatório)

    double melhor_custo_global = numeric_limits<double>::max();
    vector<int> melhor_solucao_global;

    cout << "Iniciando GRASP com " << GRASP_MAX << " iteracoes e alpha = " << ALPHA << "..." << endl;

    for (int i = 0; i < GRASP_MAX; ++i) {
        // Fase 1: Construção
        vector<int> solucao = construcao_gulosa_aleatoria(cidades, ALPHA);
        
        // Fase 2: Busca Local
        double custo_local = busca_local(solucao, cidades);

        cout << "Iteracao " << i + 1 << "/" << GRASP_MAX << " -> Custo: " << custo_local << endl;

        // Atualiza a melhor solução global
        if (custo_local < melhor_custo_global) {
            melhor_custo_global = custo_local;
            melhor_solucao_global = solucao;
            cout << "  *** Nova melhor solucao encontrada! Custo: " << melhor_custo_global << " ***" << endl;
        }
    }

    cout << "\n--- GRASP Finalizado ---" << endl;
    cout << "Melhor Custo Encontrado: " << melhor_custo_global << endl;
    cout << "Melhor Rota: ";
    for (int id_idx : melhor_solucao_global) {
        cout << cidades[id_idx].id << " -> ";
    }
    cout << cidades[melhor_solucao_global.front()].id << endl;
}