#include "common_utils.h"
#include "tsp_module.h"
#include <chrono>               
#include <iostream>
#include <fstream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <limits>
#include <map>

using namespace std;
using namespace std::chrono;

// --- Funções Utilitárias Específicas do TSP ---

static double calcular_distancia(const cidade &c1, const cidade &c2)
{
    int xd = c1.x - c2.x;
    int yd = c1.y - c2.y;
    return static_cast<int>(sqrt(xd * xd + yd * yd) + 0.5);
}

static double calcular_custo_total(const vector<int> &solucao, const vector<cidade> &cidades)
{
    double custo = 0.0;
    for (size_t i = 0; i < solucao.size() - 1; ++i) {
        custo += calcular_distancia(cidades[solucao[i]], cidades[solucao[i + 1]]);
    }
    custo += calcular_distancia(cidades[solucao.back()], cidades[solucao.front()]);
    return custo;
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

static vector<int> gerar_solucao_aleatoria(size_t num_cidades) 
{
    vector<int> solucao(num_cidades);
    iota(solucao.begin(), solucao.end(), 0);
    random_device rd;
    mt19937 g(rd());
    shuffle(solucao.begin(), solucao.end(), g);
    return solucao;
}

static vector<int> gerar_solucao_gulosa(const vector<cidade>& cidades)
{
    size_t num_cidades = cidades.size(); 
    vector<int> solucao;
    solucao.reserve(num_cidades);
    vector<bool> visitados(num_cidades, false);
    int cidade_atual = 0;
    solucao.push_back(cidade_atual);
    visitados[cidade_atual] = true;
    for (size_t i = 0; i < num_cidades - 1; ++i) { 
        double menor_distancia = numeric_limits<double>::max();
        int prox_cidade = -1;
        for (size_t j = 0; j < num_cidades; ++j) { 
            if (!visitados[j]) {
                double dist = calcular_distancia(cidades[cidade_atual], cidades[j]);
                if (dist < menor_distancia) {
                    menor_distancia = dist;
                    prox_cidade = j;
                }
            }
        }
        if (prox_cidade != -1) {
            cidade_atual = prox_cidade;
            solucao.push_back(cidade_atual);
            visitados[cidade_atual] = true;
        }
    }
    return solucao;
}

static double busca_local_tsp(vector<int>& solucao, const vector<cidade>& cidades)
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


// --- 1. Simulated Annealing (TSP) ---

double simulated_annealing_tsp(const vector<cidade>& cidades, double temp_inicial, double taxa_resfriamento)
{
    double temperatura = temp_inicial;
    double temperatura_final = 0.001;
    int iter_por_temp = 500;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_int_distribution<> index_dist(0, cidades.size() - 1);

    vector<int> solucao_atual = gerar_solucao_aleatoria(cidades.size());
    double custo_atual = calcular_custo_total(solucao_atual, cidades);
    vector<int> melhor_solucao = solucao_atual;
    double melhor_custo = custo_atual;

    while (temperatura > temperatura_final) {
        for (int i = 0; i < iter_por_temp; ++i) {
            vector<int> vizinho = solucao_atual;
            int pos1 = index_dist(gen);
            int pos2 = index_dist(gen);
            while (pos1 == pos2) pos2 = index_dist(gen);
            swap(vizinho[pos1], vizinho[pos2]);

            double custo_vizinho = calcular_custo_total(vizinho, cidades);
            double delta_custo = custo_vizinho - custo_atual;

            if (delta_custo < 0 || dis(gen) < exp(-delta_custo / temperatura)) {
                solucao_atual = vizinho;
                custo_atual = custo_vizinho;
            }
            if (custo_atual < melhor_custo) {
                melhor_solucao = solucao_atual;
                melhor_custo = custo_atual;
            }
        }
        temperatura *= taxa_resfriamento;
    }
    return melhor_custo;
}

// --- 2. Busca Tabu (TSP) ---

double busca_tabu_tsp(const vector<cidade>& cidades, int tamanho_lista_tabu, int max_iter_sem_melhora)
{
    size_t num_cidades = cidades.size(); 
    vector<int> solucao_atual = gerar_solucao_gulosa(cidades);
    double custo_atual = calcular_custo_total(solucao_atual, cidades);
    vector<int> melhor_solucao_global = solucao_atual;
    double melhor_custo_global = custo_atual;

    map<int, int> lista_tabu;
    int iter_sem_melhora = 0;
    int iter_total = 0;

    while (iter_sem_melhora < max_iter_sem_melhora) {
        iter_total++;
        iter_sem_melhora++;

        vector<int> melhor_vizinho_iteracao;
        double melhor_custo_vizinho_iteracao = numeric_limits<double>::max();
        pair<int, int> movimento_escolhido = {-1, -1};

        for (size_t i = 0; i < num_cidades; ++i) { 
            for (size_t j = i + 1; j < num_cidades; ++j) { 
                vector<int> vizinho_temp = solucao_atual;
                swap(vizinho_temp[i], vizinho_temp[j]);
                double custo_vizinho = calcular_custo_total(vizinho_temp, cidades);

                bool is_tabu = (lista_tabu.count(i) && lista_tabu[i] > iter_total) || 
                               (lista_tabu.count(j) && lista_tabu[j] > iter_total);
                bool aspiracao = (custo_vizinho < melhor_custo_global);

                if ((!is_tabu || aspiracao) && (custo_vizinho < melhor_custo_vizinho_iteracao)) {
                    melhor_custo_vizinho_iteracao = custo_vizinho;
                    melhor_vizinho_iteracao = vizinho_temp;
                    movimento_escolhido = {i, j};
                }
            }
        }
        
        if (movimento_escolhido.first == -1) break;
        solucao_atual = melhor_vizinho_iteracao;
        custo_atual = melhor_custo_vizinho_iteracao;

        int iter_expira = iter_total + tamanho_lista_tabu;
        lista_tabu[movimento_escolhido.first] = iter_expira;
        lista_tabu[movimento_escolhido.second] = iter_expira;

        if (custo_atual < melhor_custo_global) {
            melhor_solucao_global = solucao_atual;
            melhor_custo_global = custo_atual;
            iter_sem_melhora = 0;
        }
    }
    return melhor_custo_global;
}

// --- 3. GRASP (TSP) ---

static vector<int> construcao_gulosa_aleatoria_tsp(const vector<cidade>& cidades, double alpha) {
    size_t num_cidades = cidades.size(); 
    if (num_cidades == 0) return {};

    vector<int> solucao;
    solucao.reserve(num_cidades);
    vector<bool> visitados(num_cidades, false);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist_inicio(0, num_cidades - 1);
    int cidade_atual = dist_inicio(gen);
    
    solucao.push_back(cidade_atual);
    visitados[cidade_atual] = true;

    while (solucao.size() < num_cidades) {
        double c_min = numeric_limits<double>::max();
        double c_max = 0.0;
        vector<pair<int, double>> candidatos;

        for (size_t i = 0; i < num_cidades; ++i) { 
            if (!visitados[i]) {
                double custo = calcular_distancia(cidades[cidade_atual], cidades[i]);
                candidatos.push_back({i, custo});
                if (custo < c_min) c_min = custo;
                if (custo > c_max) c_max = custo;
            }
        }

        double limite = c_min + alpha * (c_max - c_min);
        vector<int> lrc;
        for (const auto& candidato : candidatos) {
            if (candidato.second <= limite) {
                lrc.push_back(candidato.first);
            }
        }
        
        if (lrc.empty()) { // Fallback se todos os candidatos forem inválidos
             for (const auto& candidato : candidatos) lrc.push_back(candidato.first);
             if(lrc.empty()) break;
        }

        uniform_int_distribution<> dist_lrc(0, lrc.size() - 1);
        int proxima_cidade = lrc[dist_lrc(gen)];
        
        solucao.push_back(proxima_cidade);
        visitados[proxima_cidade] = true;
        cidade_atual = proxima_cidade;
    }
    return solucao;
}

double grasp_tsp(const vector<cidade>& cidades, double alpha, int grasp_max_iter) {
    double melhor_custo_global = numeric_limits<double>::max();
    for (int i = 0; i < grasp_max_iter; ++i) {
        vector<int> solucao = construcao_gulosa_aleatoria_tsp(cidades, alpha);
        double custo_local = busca_local_tsp(solucao, cidades);
        if (custo_local < melhor_custo_global) {
            melhor_custo_global = custo_local;
        }
    }
    return melhor_custo_global;
}

// --- Função Principal de Testes (TSP) ---

void executar_testes_tsp(const string& nome_arquivo) {
    vector<cidade> cidades = ler_arquivo_tsp(nome_arquivo);
    if (cidades.empty()) return;

    cout << "\n=============================================" << endl;
    cout << "EXECUTANDO TESTES PARA O TSP (Instancia: " << nome_arquivo << ")" << endl;
    cout << "=============================================\n" << endl;

    const int NUM_EXECUCOES = 10;

    // --- 1. Testes do Simulated Annealing ---
    cout << "--- 1. Simulated Annealing (TSP) ---" << endl;
    vector<pair<string, pair<double, double>>> configs_sa = {
        {"Temp: 10000.0, Taxa: 0.995", {10000.0, 0.995}},
        {"Temp: 20000.0, Taxa: 0.99", {20000.0, 0.99}}
    };

    for (auto const& config : configs_sa) {
        vector<double> resultados;
        vector<double> tempos;
        for (int i = 0; i < NUM_EXECUCOES; ++i) {
            auto inicio = high_resolution_clock::now();
            double custo = simulated_annealing_tsp(cidades, config.second.first, config.second.second);
            auto fim = high_resolution_clock::now();
            duration<double> duracao = fim - inicio;
            resultados.push_back(custo);
            tempos.push_back(duracao.count());
        }
        double melhor = *min_element(resultados.begin(), resultados.end());
        double pior = *max_element(resultados.begin(), resultados.end());
        double media = accumulate(resultados.begin(), resultados.end(), 0.0) / NUM_EXECUCOES;
        double tempo_medio = accumulate(tempos.begin(), tempos.end(), 0.0) / NUM_EXECUCOES;
        imprimir_tabela_estocastica(config.first, {melhor, pior, media, tempo_medio});
    }

    // --- 2. Testes da Busca Tabu ---
    cout << "\n--- 2. Busca Tabu (TSP) ---" << endl;
    vector<pair<string, pair<int, int>>> configs_ts = {
        {"Duracao Tabu: 7, Iter s/ melhora: 100", {7, 100}},
        {"Duracao Tabu: 15, Iter s/ melhora: 200", {15, 200}}
    };

    for (auto const& config : configs_ts) {
        auto inicio = high_resolution_clock::now();
        double custo = busca_tabu_tsp(cidades, config.second.first, config.second.second);
        auto fim = high_resolution_clock::now();
        duration<double> duracao = fim - inicio;
        imprimir_tabela_deterministica(config.first, {custo, duracao.count()});
    }

    // --- 3. Testes do GRASP ---
    cout << "\n--- 3. GRASP (TSP) ---" << endl;
    vector<pair<string, pair<double, int>>> configs_grasp = {
        {"Alpha: 0.3, Iter GRASP: 50", {0.3, 50}},
        {"Alpha: 0.5, Iter GRASP: 100", {0.5, 100}}
    };

    for (auto const& config : configs_grasp) {
        vector<double> resultados;
        vector<double> tempos;
        for (int i = 0; i < NUM_EXECUCOES; ++i) {
            auto inicio = high_resolution_clock::now();
            double custo = grasp_tsp(cidades, config.second.first, config.second.second);
            auto fim = high_resolution_clock::now();
            duration<double> duracao = fim - inicio;
            resultados.push_back(custo);
            tempos.push_back(duracao.count());
        }
        double melhor = *min_element(resultados.begin(), resultados.end());
        double pior = *max_element(resultados.begin(), resultados.end());
        double media = accumulate(resultados.begin(), resultados.end(), 0.0) / NUM_EXECUCOES;
        double tempo_medio = accumulate(tempos.begin(), tempos.end(), 0.0) / NUM_EXECUCOES;
        imprimir_tabela_estocastica(config.first, {melhor, pior, media, tempo_medio});
    }
}