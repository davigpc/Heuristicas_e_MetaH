#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <chrono>

// Estrutura para resultados de algoritmos estocásticos (SA, GRASP)
struct ResultadoExecucao
{
    double melhor;
    double pior;
    double media;
    double tempo_medio;
};

// Estrutura para resultados de algoritmos determinísticos (Tabu)
struct ResultadoTabela
{
    double custo_final;
    double tempo_execucao;
};

// Funções genéricas para imprimir as tabelas de resultado
void imprimir_tabela_estocastica(const std::string& config_nome, const ResultadoExecucao& res, const std::string& metrica = "Custo");
void imprimir_tabela_deterministica(const std::string& config_nome, const ResultadoTabela& res, const std::string& metrica = "Custo");

#endif // COMMON_UTILS_H