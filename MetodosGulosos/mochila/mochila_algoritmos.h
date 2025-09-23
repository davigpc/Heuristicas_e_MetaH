#ifndef MOCHILA_ALGORITMOS_H
#define MOCHILA_ALGORITMOS_H

#include <vector>
#include <string>
#include <utility>

// Estrutura para representar um item com id, valor e peso.
struct Item
{
    int id;
    int valor;
    int peso;
};

// --- Funções ---

std::vector<Item> ler_arquivo_mochila(const std::string &nome_arquivo, int &capacidade_saida);
std::pair<int, int> avaliar_solucao(const std::vector<int> &solucao, const std::vector<Item> &itens, int capacidade);

// --- Heurísticas Gulosas ---
std::vector<int> gerar_solucao_gulosa(const std::vector<Item> &itens, int capacidade);
std::vector<int> gerar_solucao_gulosa_peso_leve(const std::vector<Item> &itens, int capacidade);


void busca_local_mochila(const std::vector<Item> &itens, int capacidade, int max_iteracoes);
void analisar_otimo_local(const std::vector<int> &solucao, const std::vector<Item> &itens, int capacidade);

#endif // MOCHILA_ALGORITMOS_H