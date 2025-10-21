#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <fstream> // Adicionado para ofstream

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

// Funções para imprimir as tabelas de resultado
void imprimir_tabela_estocastica(const std::string& config_nome, const ResultadoExecucao& res, const std::string& metrica = "Custo");
void imprimir_tabela_deterministica(const std::string& config_nome, const ResultadoTabela& res, const std::string& metrica = "Custo");

// --- Funções para escrita em CSV ---
std::ofstream abrir_csv(const std::string& nome_arquivo, const std::vector<std::string>& cabecalho);
template<typename... Args>
void escrever_linha_csv(std::ofstream& arquivo, Args... args);
template<typename T, typename... Args>
void escrever_linha_csv_impl(std::ofstream& arquivo, T valor, Args... args) {
    arquivo << valor;
    if constexpr (sizeof...(args) > 0) {
        arquivo << ",";
        escrever_linha_csv_impl(arquivo, args...);
    } else {
        arquivo << std::endl;
    }
}

template<typename... Args>
void escrever_linha_csv(std::ofstream& arquivo, Args... args) {
    if (!arquivo.is_open()) return;
    escrever_linha_csv_impl(arquivo, args...);
}


#endif // COMMON_UTILS_H