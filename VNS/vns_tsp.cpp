#include "vns_tsp.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <limits>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// --- Funções Auxiliares  ---

static double calcular_distancia(const cidade &c1, const cidade &c2)
{
    int xd = c1.x - c2.x;
    int yd = c1.y - c2.y;
    return static_cast<int>(sqrt(xd * xd + yd * yd) + 0.5);
}

static double calcular_custo_total(const vector<int> &solucao_indices, const vector<cidade> &cidades)
{
    double custo = 0.0;
    size_t n = solucao_indices.size();
    if (n < 2) return 0.0;

    for (size_t i = 0; i < n - 1; ++i) {
        custo += calcular_distancia(cidades[solucao_indices[i]], cidades[solucao_indices[i + 1]]);
    }
    custo += calcular_distancia(cidades[solucao_indices.back()], cidades[solucao_indices.front()]);
    return custo;
}

// Solução inicial gulosa (vizinho mais próximo)
static vector<int> gerar_solucao_inicial(const vector<cidade>& cidades)
{
    size_t num_cidades = cidades.size();
    if (num_cidades == 0) return {};

    vector<int> solucao_indices;
    solucao_indices.reserve(num_cidades);
    vector<bool> visitados(num_cidades, false);

    int indice_atual = 0;
    solucao_indices.push_back(indice_atual);
    visitados[indice_atual] = true;

    for (size_t i = 0; i < num_cidades - 1; ++i) {
        double menor_distancia = numeric_limits<double>::max();
        int proximo_indice = -1;
        for (size_t j = 0; j < num_cidades; ++j) {
            if (!visitados[j]) {
                double dist = calcular_distancia(cidades[indice_atual], cidades[j]);
                if (dist < menor_distancia) {
                    menor_distancia = dist;
                    proximo_indice = j;
                }
            }
        }
        if (proximo_indice != -1) {
            indice_atual = proximo_indice;
            solucao_indices.push_back(indice_atual);
            visitados[indice_atual] = true;
        } else {
             break;
        }
    }
    return solucao_indices;
}

// Busca local 
static double busca_local(vector<int>& solucao_indices, const vector<cidade>& cidades)
{
    double custo_atual = calcular_custo_total(solucao_indices, cidades);
    bool melhorou = true;
    size_t n = solucao_indices.size();

    while (melhorou) {
        melhorou = false;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                vector<int> vizinho_indices = solucao_indices;
                swap(vizinho_indices[i], vizinho_indices[j]); // Movimento 2-opt swap simples
                double custo_vizinho = calcular_custo_total(vizinho_indices, cidades);

                if (custo_vizinho < custo_atual) {
                    solucao_indices = vizinho_indices;
                    custo_atual = custo_vizinho;
                    melhorou = true;
                    goto proxima_iteracao_bl;
                }
            }
        }
    proxima_iteracao_bl:;
    }
    return custo_atual;
}

// Perturbação 
static void perturbacao_vns(vector<int>& solucao_indices, int k) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> index_dist(0, solucao_indices.size() - 1);
    size_t n = solucao_indices.size();
    if (n < 2) return;

    for (int i = 0; i < k; ++i) { 
        int pos1 = index_dist(gen);
        int pos2 = index_dist(gen);
        while (pos1 == pos2) {
            pos2 = index_dist(gen);
        }
        swap(solucao_indices[pos1], solucao_indices[pos2]);
    }
}

// ------------

vector<cidade> ler_arquivo_tsp(const string& nome_arquivo)
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

double variable_neighborhood_search_tsp(const vector<cidade>& cidades, int vns_max_iter)
{
    if (cidades.size() < 2) {
        cerr << "Erro: VNS requer pelo menos 2 cidades." << endl;
        return 0.0;
    }
    
    const int V_MAX = 15;
    vector<int> s = gerar_solucao_inicial(cidades);
    double custo_s = calcular_custo_total(s, cidades);

    int iter_sem_melhora = 0;

    cout << "VNS - Custo Inicial (Gulosa): " << custo_s << endl;
    auto inicio_total = high_resolution_clock::now();

    while (iter_sem_melhora < vns_max_iter) {
        
        int k = 1;
        bool melhorou_na_iteracao_k = false;
        while (k <= V_MAX) {

            vector<int> s_prime = s;
            perturbacao_vns(s_prime, k);
            
            vector<int> s_duas_linhas = s_prime;
            double custo_s_duas_linhas = busca_local(s_duas_linhas, cidades);
            
            if (custo_s_duas_linhas < custo_s) {
                s = s_duas_linhas;
                custo_s = custo_s_duas_linhas;
                
                cout << "  Iter " << iter_sem_melhora << ": Melhora encontrada! Novo custo: " << custo_s << " (k=" << k << ")" << endl;
                
                k = 1;
                iter_sem_melhora = 0; 
                melhorou_na_iteracao_k = true;
            } else {
                k++;
            }
        }

        if (!melhorou_na_iteracao_k) {
            iter_sem_melhora++;
        }

    }

    auto fim_total = high_resolution_clock::now();
    duration<double> duracao_total = fim_total - inicio_total;

    cout << "\n--- VNS Finalizado ---" << endl;
    cout << "Iteracoes sem melhora atingidas: " << iter_sem_melhora << " (Max: " << vns_max_iter << ")" << endl;
    cout << "Melhor Custo Encontrado: " << custo_s << endl;
    cout << "Tempo Total de Execucao: " << fixed << setprecision(4) << duracao_total.count() << " segundos" << endl;
    
    return custo_s;
}