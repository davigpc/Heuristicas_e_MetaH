#ifndef TSP_ALGORITMOS_H
#define TSP_ALGORITMOS_H

#include <vector>
#include <string>

// Estrutura para representar uma cidade com id e coordenadas.
struct cidade
{
    int id;
    double x, y;
};

// --- Funções Utilitárias ---

// Calcula a distância euclidiana arredondada entre duas cidades.
double calcular_distancia(const cidade &c1, const cidade &c2);

// Calcula o custo total de uma rota (solução).
double calcular_custo_total(const std::vector<int> &solucao, const std::vector<cidade> &cidades);

// Lê um arquivo de instância do TSP e retorna um vetor de cidades.
std::vector<cidade> ler_arquivo_instancia(const std::string &nome_arquivo);

// --- Algoritmos de Construção (Heurísticas Gulosas) ---

// Gera uma solução inicial usando a heurística do "Vizinho Mais Próximo".
std::vector<int> gerar_solucao_vizinho_proximo(const std::vector<cidade> &cidades);

// Gera uma solução inicial usando a heurística do "Vizinho Mais Distante".
std::vector<int> gerar_solucao_vizinho_distante(const std::vector<cidade> &cidades);


// --- Algoritmos de Melhoria (Busca Local) ---

// Aplica uma busca local (Hill Climbing com movimento de troca) para melhorar uma solução.
// Retorna o custo final da solução otimizada.
double busca_local(std::vector<int> &solucao, const std::vector<cidade> &cidades);


#endif // TSP_ALGORITMOS_H