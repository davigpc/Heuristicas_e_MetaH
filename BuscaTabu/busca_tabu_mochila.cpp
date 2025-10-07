#include "busca_tabu_mochila.h"
#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <map>

using namespace std;

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
}

void busca_tabu(const std::vector<Item> &itens, int capacidade)
{
    const int MAX_ITERACOES_SEM_MELHORA = 100;
    const int TAMANHO_LISTA_TABU = 7;

    int num_itens = itens.size();

    vector<int> solucao_atual = gerar_solucao_gulosa(itens, capacidade);
    vector<int> melhor_solucao_global = solucao_atual;

    pair<int, int> resultado_inicial = avaliar_solucao(solucao_atual, itens, capacidade);
    int valor_atual = resultado_inicial.first;
    int melhor_valor_global = valor_atual;

    cout << "Solucao Inicial (gulosa) -> Valor: " << valor_atual << ", Peso: " << resultado_inicial.second << endl;

    map<int, int> lista_tabu;
    int iter_sem_melhora = 0;
    int iter_total = 0;

    while (iter_sem_melhora < MAX_ITERACOES_SEM_MELHORA)
    {
        iter_total++;
        iter_sem_melhora++;

        vector<int> melhor_vizinho_iteracao;
        int melhor_valor_vizinho_iteracao = -1;
        int movimento_escolhido = -1;

        for (int i = 0; i < num_itens; ++i)
        {
            vector<int> vizinho_temp = solucao_atual;
            vizinho_temp[i] = 1 - vizinho_temp[i];

            pair<int, int> res_vizinho = avaliar_solucao(vizinho_temp, itens, capacidade);
            int valor_vizinho = res_vizinho.first;

            if (valor_vizinho == 0) continue;

            bool is_tabu = lista_tabu.count(i) > 0;
            bool aspiracao = (valor_vizinho > melhor_valor_global);

            if (!is_tabu || aspiracao)
            {
                if (valor_vizinho > melhor_valor_vizinho_iteracao)
                {
                    melhor_valor_vizinho_iteracao = valor_vizinho;
                    melhor_vizinho_iteracao = vizinho_temp;
                    movimento_escolhido = i;
                }
            }
        }

        if (movimento_escolhido == -1) {
            cout << "Nenhum movimento valido encontrado. Encerrando." << endl;
            break;
        }

        solucao_atual = melhor_vizinho_iteracao;
        valor_atual = melhor_valor_vizinho_iteracao;

        vector<int> movimentos_para_remover;
        for (auto& par : lista_tabu) {
            par.second--;
            if (par.second == 0) {
                movimentos_para_remover.push_back(par.first);
            }
        }
        for (int mov : movimentos_para_remover) {
            lista_tabu.erase(mov);
        }

        lista_tabu[movimento_escolhido] = TAMANHO_LISTA_TABU;

        if (valor_atual > melhor_valor_global)
        {
            melhor_solucao_global = solucao_atual;
            melhor_valor_global = valor_atual;
            iter_sem_melhora = 0;
            cout << "Iter " << iter_total << ": Melhora encontrada! Novo melhor valor: " << melhor_valor_global << endl;
        }
    }

    cout << "\n--- Busca Tabu Finalizada ---" << endl;
    pair<int, int> resultado_final = avaliar_solucao(melhor_solucao_global, itens, capacidade);
    cout << "Melhor solucao encontrada:" << endl;
    cout << "  - Lucro Total: " << resultado_final.first << endl;
    cout << "  - Peso Total: " << resultado_final.second << " / " << capacidade << endl;
    cout << "  - Iteracoes totais: " << iter_total << endl;
}