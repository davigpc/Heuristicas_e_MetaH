#ifndef TSP_MODULE_H
#define TSP_MODULE_H

#include "common_utils.h"
#include <vector>
#include <string>

struct cidade
{
    int id;
    double x, y;
};

std::vector<cidade> ler_arquivo_tsp(const std::string& nome_arquivo);

// --- Interface dos Algoritmos ---
double simulated_annealing_tsp(const std::vector<cidade>& cidades, double temp_inicial, double taxa_resfriamento);
double busca_tabu_tsp(const std::vector<cidade>& cidades, int tamanho_lista_tabu, int max_iter_sem_melhora);
double grasp_tsp(const std::vector<cidade>& cidades, double alpha, int grasp_max_iter);

// --- Função Principal de Testes ---
void executar_testes_tsp(const std::string& nome_arquivo);

#endif // TSP_MODULE_H