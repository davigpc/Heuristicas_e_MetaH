#include "simulated_annealing.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;

static double calcular_distancia(const cidade &c1, const cidade &c2)
{
    int xd = c1.x - c2.x;
    int yd = c1.y - c2.y;
    return static_cast<int>(sqrt(xd * xd + yd * yd) + 0.5);
}

static double calcular_custo_total(const vector<int> &solucao, const vector<cidade> &cidades)
{
    double custo = 0.0;
    for (size_t i = 0; i < solucao.size() - 1; ++i)
    {
        custo += calcular_distancia(cidades[solucao[i]], cidades[solucao[i + 1]]);
    }
    custo += calcular_distancia(cidades[solucao.back()], cidades[solucao.front()]);
    return custo;
}

static vector<int> gerar_solucao_aleatoria(int num_cidades)
{
    vector<int> solucao(num_cidades);
    iota(solucao.begin(), solucao.end(), 0);
    random_device rd;
    mt19937 g(rd());
    shuffle(solucao.begin(), solucao.end(), g);
    return solucao;
}

vector<cidade> ler_arquivo_instancia(const string& nome_arquivo)
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
        // Armazena com o ID original, mas a solução usará índices de 0 a N-1
        cidades.push_back({id, x, y});
    }
    arquivo.close();

    if (cidades.empty())
    {
        cerr << "Erro: Nenhuma cidade foi lida do arquivo." << endl;
    }
    return cidades;
}

void simulated_annealing(const vector<cidade>& cidades)
{
    // Parâmetros do SA
    double temperatura_inicial = 10000.0;
    double temperatura_final = 0.001;
    double taxa_de_resfriamento = 0.995;
    int iteracoes_por_temperatura = 500;

    // Geradores de números aleatórios
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_int_distribution<> index_dist(0, cidades.size() - 1);

    // Solução Inicial
    vector<int> solucao_atual = gerar_solucao_aleatoria(cidades.size());
    double custo_atual = calcular_custo_total(solucao_atual, cidades);

    vector<int> melhor_solucao = solucao_atual;
    double melhor_custo = custo_atual;

    cout << "Solucao Inicial Aleatoria Gerada." << endl;
    cout << "Custo Inicial: " << custo_atual << endl << endl;

    double temperatura = temperatura_inicial;

    while (temperatura > temperatura_final)
    {
        for (int i = 0; i < iteracoes_por_temperatura; ++i)
        {
            vector<int> vizinho = solucao_atual;
            int pos1 = index_dist(gen);
            int pos2 = index_dist(gen);
            while (pos1 == pos2)
            {
                pos2 = index_dist(gen);
            }
            swap(vizinho[pos1], vizinho[pos2]);

            double custo_vizinho = calcular_custo_total(vizinho, cidades);
            double delta_custo = custo_vizinho - custo_atual;

            // Critério de aceitação de Metropolis
            if (delta_custo < 0 || dis(gen) < exp(-delta_custo / temperatura))
            {
                solucao_atual = vizinho;
                custo_atual = custo_vizinho;
            }

            if (custo_atual < melhor_custo)
            {
                melhor_solucao = solucao_atual;
                melhor_custo = custo_atual;
            }
        }
        temperatura *= taxa_de_resfriamento;
    }

    cout << "\n--- Busca Finalizada ---" << endl;
    cout << "Custo Final: " << melhor_custo << endl;
    cout << "Rota Final: ";
    for (int id_idx : melhor_solucao)
    {
        cout << cidades[id_idx].id << " -> ";
    }
    cout << cidades[melhor_solucao.front()].id << endl;
}