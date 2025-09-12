#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

// --- MÓDULO DE LÓGICA DO GRAFO ---

vector<vector<int>> criar_grafo_de_arquivo(const string& nome_arquivo, int& V_saida) {
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo '" << nome_arquivo << "'" << endl;
        V_saida = 0;
        return {};
    }

    int V, A;
    arquivo >> V >> A;
    V_saida = V;

    vector<vector<int>> grafo(V, vector<int>(V, 0));
    int origem, destino, valor;

    for (int i = 0; i < A; i++) {
        arquivo >> origem >> destino >> valor;
        if (origem > 0 && origem <= V && destino > 0 && destino <= V) {
            grafo[origem - 1][destino - 1] = valor;
        }
    }
    arquivo.close();
    cout << "Grafo lido com sucesso do arquivo '" << nome_arquivo << "'." << endl;
    return grafo;
}


// --- MÓDULO DE BUSCA BACKTRACKING ---

void imprimir_caminho(const vector<int>& caminho) {
    cout << "  >> "; 
    for (size_t i = 0; i < caminho.size(); ++i) {
        cout << caminho[i] + 1;
        if (i < caminho.size() - 1) {
            cout << " -> ";
        }
    }
    cout << endl;
}

int encontrar_permutacoes_recursivo(
    const vector<vector<int>>& grafo,
    vector<int>& caminho_atual,
    vector<bool>& visitado,
    int V)
{
    if (caminho_atual.size() == V) {
        imprimir_caminho(caminho_atual);
        return 1;
    }
    int caminhos_encontrados = 0;
    int ultima_cidade_no_caminho = caminho_atual.back();

    for (int proxima_cidade = 0; proxima_cidade < V; ++proxima_cidade) {
        if (!visitado[proxima_cidade] && grafo[ultima_cidade_no_caminho][proxima_cidade] != 0) {
            visitado[proxima_cidade] = true;
            caminho_atual.push_back(proxima_cidade);
            caminhos_encontrados += encontrar_permutacoes_recursivo(grafo, caminho_atual, visitado, V);
            visitado[proxima_cidade] = false;
            caminho_atual.pop_back();
        }
    }
    return caminhos_encontrados;
}

void iniciar_busca_backtracking(const vector<vector<int>>& grafo, int V) {
    cout << "\n--- Buscando todas as permutacoes validas no grafo ---" << endl;
    int total_geral = 0;

    for (int cidade_inicial = 0; cidade_inicial < V; ++cidade_inicial) {
        cout << "Caminhos comecando pela cidade " << cidade_inicial + 1 << ":" << endl;

        vector<int> caminho_atual;
        caminho_atual.push_back(cidade_inicial);
        vector<bool> visitado(V, false);
        visitado[cidade_inicial] = true;

        int caminhos_encontrados = encontrar_permutacoes_recursivo(grafo, caminho_atual, visitado, V);
        total_geral += caminhos_encontrados;
        if (caminhos_encontrados == 0) {
            cout << "  Nenhum caminho completo foi encontrado." << endl;
        }
    }
    
    cout << "\n--- Busca finalizada. Total de caminhos completos encontrados: " << total_geral << " ---" << endl;
}


// --- FUNÇÃO PRINCIPAL ---

int main(int argc, char* argv[])
{

    if (argc != 2) {
        cerr << "Uso correto: " << argv[0] << " <nome_do_arquivo>" << endl;
        return 1; 
    }

    string nome_arquivo = argv[1];
    int V = 0;

    vector<vector<int>> grafo = criar_grafo_de_arquivo(nome_arquivo, V);

    if (V == 0) {
        return 1;
    }

    iniciar_busca_backtracking(grafo, V);

    return 0;
}
