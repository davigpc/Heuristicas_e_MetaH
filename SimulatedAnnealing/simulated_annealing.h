#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <vector>
#include <string>

// Estrutura para representar uma cidade com id e coordenadas.
struct cidade
{
    int id;
    double x, y;
};

/**
 * @brief Lê um arquivo de instância do TSP e retorna um vetor de cidades.
 * @param nome_arquivo O caminho para o arquivo.
 * @return Um vetor de objetos 'cidade'. Retorna um vetor vazio em caso de erro.
 */
std::vector<cidade> ler_arquivo_instancia(const std::string& nome_arquivo);

/**
 * @brief Executa o algoritmo Simulated Annealing para resolver o TSP.
 * @param cidades O vetor de cidades a serem visitadas.
 */
void simulated_annealing(const std::vector<cidade>& cidades);

#endif 