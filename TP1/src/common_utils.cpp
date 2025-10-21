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