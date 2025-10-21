#include "common_utils.h"

using namespace std;

void imprimir_tabela_estocastica(const string& config_nome, const ResultadoExecucao& res, const string& metrica)
{
    cout << "--------------------------------------------------------------------------" << endl;
    cout << "Configuracao: " << config_nome << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << left << setw(20) << ("Melhor " + metrica)
         << setw(20) << ("Pior " + metrica)
         << setw(20) << ("Media " + metrica)
         << setw(20) << "Tempo Medio (s)" << endl;

    cout << fixed << setprecision(2) << left
         << setw(20) << res.melhor
         << setw(20) << res.pior
         << setw(20) << res.media
         << setprecision(5) << setw(20) << res.tempo_medio << endl;
    cout << "--------------------------------------------------------------------------" << endl;
}

void imprimir_tabela_deterministica(const string& config_nome, const ResultadoTabela& res, const string& metrica)
{
    cout << "-----------------------------------------------------" << endl;
    cout << "Configuracao: " << config_nome << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << left << setw(30) << (metrica + " Final")
         << setw(20) << "Tempo de Execucao (s)" << endl;

    cout << fixed << setprecision(2) << left
         << setw(30) << res.custo_final
         << setprecision(5) << setw(20) << res.tempo_execucao << endl;
    cout << "-----------------------------------------------------" << endl;
}

// Abre um arquivo CSV e escreve o cabeçalho
std::ofstream abrir_csv(const std::string& nome_arquivo, const std::vector<std::string>& cabecalho) {
    std::ofstream arquivo(nome_arquivo);
    if (arquivo.is_open()) {
        for (size_t i = 0; i < cabecalho.size(); ++i) {
            arquivo << cabecalho[i] << (i == cabecalho.size() - 1 ? "" : ",");
        }
        arquivo << std::endl;
    } else {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo CSV " << nome_arquivo << std::endl;
    }
    return arquivo; // Retorna o objeto ofstream 
}