#include "ils_tsp.h"
#include <iomanip>
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

using namespace std;
using namespace std::chrono;

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

static vector<int> gerar_solucao_gulosa(const vector<cidade>& cidades)
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


static double busca_local_tsp(vector<int>& solucao_indices, const vector<cidade>& cidades)
{
    double custo_atual = calcular_custo_total(solucao_indices, cidades);
    bool melhorou = true;
    size_t n = solucao_indices.size();

    while (melhorou) {
        melhorou = false;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                vector<int> vizinho_indices = solucao_indices;
                swap(vizinho_indices[i], vizinho_indices[j]);
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


static void perturbacao(vector<int>& solucao_indices, int d) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> index_dist(0, solucao_indices.size() - 1);
    size_t n = solucao_indices.size();
    if (n < 2) return; 

    for (int k = 0; k < d; ++k) {
        int pos1 = index_dist(gen);
        int pos2 = index_dist(gen);
        while (pos1 == pos2) { 
            pos2 = index_dist(gen);
        }
        swap(solucao_indices[pos1], solucao_indices[pos2]);
    }
}

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

double iterated_local_search_tsp(const vector<cidade>& cidades, int ils_max_iter) {
    if (cidades.size() < 2) {
        cerr << "Erro: ILS requer pelo menos 2 cidades." << endl;
        return 0.0;
    }


    vector<int> s0_indices = gerar_solucao_gulosa(cidades);
    vector<int> s_indices = s0_indices;
    double custo_s = busca_local_tsp(s_indices, cidades); 

    vector<int> s_best_indices = s_indices; 
    double custo_s_best = custo_s;

    int iter = 0;
    int d = 1;

    cout << "ILS - Custo Inicial (Apos BL): " << custo_s << endl;

    auto inicio_total = high_resolution_clock::now(); 


    while (iter < ils_max_iter) {

        iter++;


        vector<int> s_linha_indices = s_indices; 
        perturbacao(s_linha_indices, d);


        vector<int> s_duas_linhas_indices = s_linha_indices;
        double custo_s_duas_linhas = busca_local_tsp(s_duas_linhas_indices, cidades);


        if (custo_s_duas_linhas < custo_s) {

            s_indices = s_duas_linhas_indices;
            custo_s = custo_s_duas_linhas;
            iter = 0;
            d = 1;
             cout << "  Iter " << iter << " : Melhora encontrada. Novo custo base: " << custo_s << endl;

            if (custo_s < custo_s_best) {
                 s_best_indices = s_indices;
                 custo_s_best = custo_s;
                 cout << "    *** Nova melhor solucao global: " << custo_s_best << " ***" << endl;
            }
        } else {
            d++;
        }
    }

    auto fim_total = high_resolution_clock::now();
    duration<double> duracao_total = fim_total - inicio_total;

    cout << "\n--- ILS Finalizado ---" << endl;
    cout << "Iteracoes sem melhora atingidas: " << iter << " (Max: " << ils_max_iter << ")" << endl;
    cout << "Melhor Custo Encontrado: " << custo_s_best << endl;
    cout << "Tempo Total de Execucao: " << fixed << setprecision(4) << duracao_total.count() << " segundos" << endl;
    cout << "Melhor Rota (Indices): ";
    for (int idx : s_best_indices) { cout << idx << " "; } cout << endl;
    cout << "Melhor Rota (IDs): ";
     for (int idx : s_best_indices) { cout << cidades[idx].id << " -> "; }
     cout << cidades[s_best_indices.front()].id << endl;


    return custo_s_best;
}