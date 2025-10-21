#include "common_utils.h" 
#include "mochila_module.h"
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

// --- Funções Utilitárias Específicas da Mochila ---

vector<Item> ler_arquivo_mochila(const string& nome_arquivo, int& capacidade_saida)
{
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo " << nome_arquivo << endl;
        return {};
    }
    int num_itens;
    arquivo >> num_itens >> capacidade_saida;
    vector<Item> itens;
    int v, p;
    for (int i = 0; i < num_itens; ++i) {
        arquivo >> v >> p;
        itens.push_back({i + 1, v, p});
    }
    arquivo.close();
    if (itens.empty()) {
        cerr << "Erro: Nenhuma cidade foi lida do arquivo." << endl;
    }
    return itens;
}

static pair<int, int> avaliar_solucao(const vector<int>& solucao, const vector<Item>& itens, int capacidade)
{
    int valor_total = 0;
    int peso_total = 0;
    for (size_t i = 0; i < solucao.size(); ++i) {
        if (solucao[i] == 1) {
            valor_total += itens[i].valor;
            peso_total += itens[i].peso;
        }
    }
    if (peso_total > capacidade) {
        return {0, peso_total}; // Penalidade: solução inválida tem valor 0
    }
    return {valor_total, peso_total};
}

static vector<int> gerar_solucao_aleatoria_mochila(size_t num_itens) 
{
    vector<int> solucao(num_itens);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);
    for(size_t i = 0; i < num_itens; ++i) { 
        solucao[i] = dis(gen);
    }
    return solucao;
}

static vector<int> gerar_solucao_gulosa_mochila(const vector<Item>& itens, int capacidade)
{
    size_t num_itens = itens.size(); 
    vector<int> indices(num_itens);
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&](int a, int b) {
        double razao_a = (itens[a].peso > 0) ? static_cast<double>(itens[a].valor) / itens[a].peso : 0;
        double razao_b = (itens[b].peso > 0) ? static_cast<double>(itens[b].valor) / itens[b].peso : 0;
        return razao_a > razao_b;
    });
    vector<int> solucao(num_itens, 0);
    int peso_atual = 0;
    for (int indice_item : indices) {
        if (peso_atual + itens[indice_item].peso <= capacidade) {
            solucao[indice_item] = 1;
            peso_atual += itens[indice_item].peso;
        }
    }
    return solucao;
}

static void busca_local_mochila(vector<int>& solucao, const vector<Item>& itens, int capacidade)
{
    int valor_atual = avaliar_solucao(solucao, itens, capacidade).first;
    bool melhorou = true;
    while(melhorou) {
        melhorou = false;
        for(size_t i = 0; i < solucao.size(); ++i) {
            vector<int> vizinho = solucao;
            vizinho[i] = 1 - vizinho[i]; // Flip
            int valor_vizinho = avaliar_solucao(vizinho, itens, capacidade).first;
            if (valor_vizinho > valor_atual) {
                solucao = vizinho;
                valor_atual = valor_vizinho;
                melhorou = true;
                goto proxima_iteracao; // First-improvement
            }
        }
    proxima_iteracao:;
    }
}

// --- 1. Simulated Annealing (Mochila) ---

int simulated_annealing_mochila(const vector<Item>& itens, int capacidade, double temp_inicial, double taxa_resfriamento)
{
    double temperatura = temp_inicial;
    double temperatura_final = 0.001;
    size_t num_itens = itens.size(); 

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_int_distribution<> index_dist(0, num_itens - 1);

    vector<int> solucao_atual = gerar_solucao_aleatoria_mochila(num_itens);
    int valor_atual = avaliar_solucao(solucao_atual, itens, capacidade).first;
    vector<int> melhor_solucao = solucao_atual;
    int melhor_valor = valor_atual;

    while (temperatura > temperatura_final) {
        for (int i = 0; i < 100; ++i) {
            vector<int> vizinho = solucao_atual;
            int pos_flip = index_dist(gen);
            vizinho[pos_flip] = 1 - vizinho[pos_flip];
            int valor_vizinho = avaliar_solucao(vizinho, itens, capacidade).first;
            double delta_valor = valor_vizinho - valor_atual;

            if (delta_valor > 0 || dis(gen) < exp(delta_valor / temperatura)) {
                solucao_atual = vizinho;
                valor_atual = valor_vizinho;
            }
            if (valor_atual > melhor_valor) {
                melhor_solucao = solucao_atual;
                melhor_valor = valor_atual;
            }
        }
        temperatura *= taxa_resfriamento;
    }
    return melhor_valor;
}

// --- 2. Busca Tabu (Mochila) ---

int busca_tabu_mochila(const vector<Item>& itens, int capacidade, int tamanho_lista_tabu, int max_iter_sem_melhora)
{
    size_t num_itens = itens.size(); 
    vector<int> solucao_atual = gerar_solucao_gulosa_mochila(itens, capacidade);
    int valor_atual = avaliar_solucao(solucao_atual, itens, capacidade).first;
    vector<int> melhor_solucao_global = solucao_atual;
    int melhor_valor_global = valor_atual;

    map<int, int> lista_tabu;
    int iter_sem_melhora = 0;
    int iter_total = 0;

    while (iter_sem_melhora < max_iter_sem_melhora) {
        iter_total++;
        iter_sem_melhora++;

        vector<int> melhor_vizinho_iteracao;
        int melhor_valor_vizinho_iteracao = -1;
        int movimento_escolhido = -1;

        for (size_t i = 0; i < num_itens; ++i) { 
            vector<int> vizinho_temp = solucao_atual;
            vizinho_temp[i] = 1 - vizinho_temp[i];
            int valor_vizinho = avaliar_solucao(vizinho_temp, itens, capacidade).first;
            if (valor_vizinho == 0) continue;

            bool is_tabu = (lista_tabu.count(i) && lista_tabu[i] > iter_total);
            bool aspiracao = (valor_vizinho > melhor_valor_global);
            
            if ((!is_tabu || aspiracao) && (valor_vizinho > melhor_valor_vizinho_iteracao)) {
                melhor_valor_vizinho_iteracao = valor_vizinho;
                melhor_vizinho_iteracao = vizinho_temp;
                movimento_escolhido = i;
            }
        }
        
        if (movimento_escolhido == -1) break;
        solucao_atual = melhor_vizinho_iteracao;
        valor_atual = melhor_valor_vizinho_iteracao;

        lista_tabu[movimento_escolhido] = iter_total + tamanho_lista_tabu;

        if (valor_atual > melhor_valor_global) {
            melhor_solucao_global = solucao_atual;
            melhor_valor_global = valor_atual;
            iter_sem_melhora = 0;
        }
    }
    return melhor_valor_global;
}

// --- 3. GRASP (Mochila) ---

static vector<int> construcao_gulosa_aleatoria_mochila(const vector<Item>& itens, int capacidade, double alpha)
{
    size_t num_itens = itens.size(); 
    vector<int> solucao(num_itens, 0);
    int peso_atual = 0;
    
    vector<int> candidatos_restantes(num_itens);
    iota(candidatos_restantes.begin(), candidatos_restantes.end(), 0);

    random_device rd;
    mt19937 gen(rd());

    while (!candidatos_restantes.empty())
    {
        double c_min_razao = numeric_limits<double>::max();
        double c_max_razao = 0.0;
        vector<pair<int, double>> candidatos_avaliados;

        for (int idx : candidatos_restantes)
        {
            if (itens[idx].peso > 0) {
                double razao = static_cast<double>(itens[idx].valor) / itens[idx].peso;
                candidatos_avaliados.push_back({idx, razao});
                if (razao < c_min_razao) c_min_razao = razao; 
                if (razao > c_max_razao) c_max_razao = razao;
            }
        }
        if (candidatos_avaliados.empty()) break;

        double limite = c_max_razao - alpha * (c_max_razao - c_min_razao);
        vector<int> lrc;
        for (const auto& cand : candidatos_avaliados) {
            if (cand.second >= limite) lrc.push_back(cand.first);
        }
        if (lrc.empty()) break;

        uniform_int_distribution<> dist_lrc(0, lrc.size() - 1);
        int item_escolhido_idx = lrc[dist_lrc(gen)];
        
        if (peso_atual + itens[item_escolhido_idx].peso <= capacidade) {
            solucao[item_escolhido_idx] = 1;
            peso_atual += itens[item_escolhido_idx].peso;
        }
        
        candidatos_restantes.erase(remove(candidatos_restantes.begin(), candidatos_restantes.end(), item_escolhido_idx), candidatos_restantes.end());
    }
    return solucao;
}

int grasp_mochila(const vector<Item>& itens, int capacidade, double alpha, int grasp_max_iter)
{
    int melhor_valor_global = 0;
    for (int i = 0; i < grasp_max_iter; ++i)
    {
        vector<int> solucao = construcao_gulosa_aleatoria_mochila(itens, capacidade, alpha);
        busca_local_mochila(solucao, itens, capacidade);
        int valor_local = avaliar_solucao(solucao, itens, capacidade).first;
        if (valor_local > melhor_valor_global) {
            melhor_valor_global = valor_local;
        }
    }
    return melhor_valor_global;
}

// --- Função Principal de Testes (Mochila) ---

void executar_testes_mochila(const string& nome_arquivo) {
    int capacidade = 0;
    vector<Item> itens = ler_arquivo_mochila(nome_arquivo, capacidade);
    if (itens.empty()) return;

    cout << "\n=====================================================" << endl;
    cout << "EXECUTANDO TESTES PARA A MOCHILA (Instancia: " << nome_arquivo << ")" << endl;
    cout << "=====================================================\n" << endl;

    const int NUM_EXECUCOES = 10;
    const string METRICA = "Valor";

    // --- 1. Testes do Simulated Annealing ---
    cout << "--- 1. Simulated Annealing (Mochila) ---" << endl;
    vector<pair<string, pair<double, double>>> configs_sa = {
        {"Temp: 10000.0, Taxa: 0.8", {10000.0, 0.8}},
        {"Temp: 20000.0, Taxa: 0.9", {20000.0, 0.9}}
    };

    for (auto const& config : configs_sa) {
        vector<double> resultados;
        vector<double> tempos;
        for (int i = 0; i < NUM_EXECUCOES; ++i) {
            auto inicio = high_resolution_clock::now();
            double valor = simulated_annealing_mochila(itens, capacidade, config.second.first, config.second.second);
            auto fim = high_resolution_clock::now();
            duration<double> duracao = fim - inicio;
            resultados.push_back(valor);
            tempos.push_back(duracao.count());
        }
        double melhor = *max_element(resultados.begin(), resultados.end());
        double pior = *min_element(resultados.begin(), resultados.end());
        double media = accumulate(resultados.begin(), resultados.end(), 0.0) / NUM_EXECUCOES;
        double tempo_medio = accumulate(tempos.begin(), tempos.end(), 0.0) / NUM_EXECUCOES;
        imprimir_tabela_estocastica(config.first, {melhor, pior, media, tempo_medio}, METRICA);
    }

    // --- 2. Testes da Busca Tabu ---
    cout << "\n--- 2. Busca Tabu (Mochila) ---" << endl;
    vector<pair<string, pair<int, int>>> configs_ts = {
        {"Duracao Tabu: 7, Iter s/ melhora: 50", {7, 50}},
        {"Duracao Tabu: 15, Iter s/ melhora: 100", {15, 100}}
    };

    for (auto const& config : configs_ts) {
        auto inicio = high_resolution_clock::now();
        double valor = busca_tabu_mochila(itens, capacidade, config.second.first, config.second.second);
        auto fim = high_resolution_clock::now();
        duration<double> duracao = fim - inicio;
        imprimir_tabela_deterministica(config.first, {valor, duracao.count()}, METRICA);
    }

    // --- 3. Testes do GRASP ---
    cout << "\n--- 3. GRASP (Mochila) ---" << endl;
    vector<pair<string, pair<double, int>>> configs_grasp = {
        {"Alpha: 0.3, Iter GRASP: 20", {0.3, 20}},
        {"Alpha: 0.5, Iter GRASP: 50", {0.5, 50}}
    };

    for (auto const& config : configs_grasp) {
        vector<double> resultados;
        vector<double> tempos;
        for (int i = 0; i < NUM_EXECUCOES; ++i) {
            auto inicio = high_resolution_clock::now();
            double valor = grasp_mochila(itens, capacidade, config.second.first, config.second.second);
            auto fim = high_resolution_clock::now();
            duration<double> duracao = fim - inicio;
            resultados.push_back(valor);
            tempos.push_back(duracao.count());
        }
        double melhor = *max_element(resultados.begin(), resultados.end());
        double pior = *min_element(resultados.begin(), resultados.end());
        double media = accumulate(resultados.begin(), resultados.end(), 0.0) / NUM_EXECUCOES;
        double tempo_medio = accumulate(tempos.begin(), tempos.end(), 0.0) / NUM_EXECUCOES;
        imprimir_tabela_estocastica(config.first, {melhor, pior, media, tempo_medio}, METRICA);
    }
}