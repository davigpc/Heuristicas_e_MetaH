#ifndef MOCHILA_MODULE_H
#define MOCHILA_MODULE_H

#include "common_utils.h"
#include <vector>
#include <string>

struct Item
{
    int id;
    int valor;
    int peso;
};

std::vector<Item> ler_arquivo_mochila(const std::string& nome_arquivo, int& capacidade_saida);

// --- Interface dos Algoritmos ---
int simulated_annealing_mochila(const std::vector<Item>& itens, int capacidade, double temp_inicial, double taxa_resfriamento);
int busca_tabu_mochila(const std::vector<Item>& itens, int capacidade, int tamanho_lista_tabu, int max_iter_sem_melhora);
int grasp_mochila(const std::vector<Item>& itens, int capacidade, double alpha, int grasp_max_iter);

// --- Função Principal de Testes ---
void executar_testes_mochila(const std::string& nome_arquivo);

#endif // MOCHILA_MODULE_H