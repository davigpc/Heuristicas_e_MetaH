#ifndef GRASP_H
#define GRASP_H

#include <vector>
#include <string>

// Estrutura para representar uma cidade com id e coordenadas.
struct cidade
{
    int id;
    double x, y;
};

// --- Funções Utilitárias ---
std::vector<cidade> ler_arquivo_instancia(const std::string& nome_arquivo);
double calcular_custo_total(const std::vector<int>& solucao, const std::vector<cidade>& cidades);

// --- Algoritmos de Melhoria / Busca Local ---
double busca_local(std::vector<int>& solucao, const std::vector<cidade>& cidades);

// --- Meta-heurística GRASP ---

std::vector<int> construcao_gulosa_aleatoria(const std::vector<cidade>& cidades, double alpha);
void grasp_tsp(const std::vector<cidade>& cidades);

#endif // GRASP_H