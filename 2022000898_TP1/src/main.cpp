#include "tsp_module.h"
#include "mochila_module.h"
#include <iostream>
#include <string>

using namespace std;

void imprimir_ajuda() {
    cerr << "Uso correto: ./trabalho1 <flag> <arquivo_instancia>" << endl;
    cerr << "Flags:" << endl;
    cerr << "  --tsp      <arquivo.tsp>  : Executa todos os testes para o TSP." << endl;
    cerr << "  --mochila <arquivo.txt>  : Executa todos os testes para a Mochila." << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        imprimir_ajuda();
        return 1;
    }

    string flag = argv[1];
    string nome_arquivo = argv[2];

    if (flag == "--tsp") {
        executar_testes_tsp(nome_arquivo);
    } else if (flag == "--mochila") {
        executar_testes_mochila(nome_arquivo);
    } else {
        cerr << "Erro: Flag '" << flag << "' desconhecida." << endl;
        imprimir_ajuda();
        return 1;
    }

    return 0;
}