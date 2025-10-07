#ifndef BUSCA_TABU_MOCHILA_H
#define BUSCA_TABU_MOCHILA_H

#include <vector>
#include <string>
#include <utility>

struct Item
{
    int id;
    int valor;
    int peso;
};

// --- Funções Utilitárias e Heurísticas ---
std::vector<Item> ler_arquivo_mochila(const std::string &nome_arquivo, int &capacidade_saida);
std::pair<int, int> avaliar_solucao(const std::vector<int> &solucao, const std::vector<Item> &itens, int capacidade);
std::vector<int> gerar_solucao_gulosa(const std::vector<Item> &itens, int capacidade);

// --- Meta-heurísticas ---
void busca_local_mochila(const std::vector<Item> &itens, int capacidade, int max_iteracoes);
void busca_tabu(const std::vector<Item> &itens, int capacidade);


#endif // BUSCA_TABU_MOCHILA_H