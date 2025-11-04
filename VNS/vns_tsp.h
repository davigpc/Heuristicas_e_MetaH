#ifndef VNS_TSP_H
#define VNS_TSP_H

#include <vector>
#include <string>

// Estrutura para representar uma cidade
struct cidade
{
    int id;
    double x, y;
};

/**
 * @brief Lê um arquivo de instância do TSP.
 * @param nome_arquivo Caminho para o arquivo.
 * @return Vetor de cidades ou vetor vazio em caso de erro.
 */
std::vector<cidade> ler_arquivo_tsp(const std::string& nome_arquivo);

/**
 * @brief Executa o algoritmo Variable Neighborhood Search (VNS) para o TSP.
 * @param cidades Vetor de cidades.
 * @param vns_max_iter Número máximo de iterações SEM MELHORA (critério de parada).
 * @return Custo da melhor solução encontrada.
 */
double variable_neighborhood_search_tsp(const std::vector<cidade>& cidades, int vns_max_iter);

#endif // VNS_TSP_H