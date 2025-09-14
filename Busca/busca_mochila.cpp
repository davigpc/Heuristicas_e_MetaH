#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <numeric>

using namespace std;

struct Item {
    int id;
    int valor;
    int peso;
};

// --- MÓDULO DE LEITURA E AVALIAÇÃO ---

vector<Item> ler_arquivo_mochila(const string& nome_arquivo, int& capacidade_saida) {
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo '" << nome_arquivo << "'" << endl;
        return {};
    }
    int num_itens;
    arquivo >> num_itens >> capacidade_saida;
    vector<Item> itens;
    int v, p;
    for (int i = 0; i < num_itens; ++i) {
        arquivo >> v >> p;
        itens.push_back({i + 1, v, p});
    }
    arquivo.close();
    cout << "Arquivo '" << nome_arquivo << "' lido com sucesso." << endl;
    cout << "Itens: " << num_itens << ", Capacidade da Mochila: " << capacidade_saida << endl;
    return itens;
}

pair<int, int> avaliar_solucao(const vector<int>& solucao, const vector<Item>& itens, int capacidade) {
    int valor_total = 0;
    int peso_total = 0;
    for (size_t i = 0; i < solucao.size(); ++i) {
        if (solucao[i] == 1) {
            valor_total += itens[i].valor;
            peso_total += itens[i].peso;
        }
    }
    if (peso_total > capacidade) {
        return {0, peso_total}; // Retorna {lucro, peso}
    }
    return {valor_total, peso_total};
}

vector<int> gerar_solucao_inicial_valida(int num_itens) {
    return vector<int>(num_itens, 0);
}


/**
 * @brief Imprime uma análise detalhada da vizinhança de uma solução para explicar por que ela é um ótimo local.
 */
void analisar_otimo_local(const vector<int>& solucao, const vector<Item>& itens, int capacidade) {
    int valor_solucao = avaliar_solucao(solucao, itens, capacidade).first;
    
    cout << "\n--- Analise do Otimo Local Encontrado ---" << endl;
    cout << "A solucao acima e um \"otimo local\". Isso significa que nenhum vizinho direto" << endl;
    cout << "e melhor que ela:" << endl << endl;
    cout << "Avaliando vizinhos da solucao ";
    for(int bit : solucao) cout << bit;
    cout << " (Lucro: " << valor_solucao << "):" << endl;

    for (size_t i = 0; i < itens.size(); ++i) {
        vector<int> vizinho = solucao;
        vizinho[i] = 1 - vizinho[i]; // Flip

        pair<int, int> resultado_vizinho = avaliar_solucao(vizinho, itens, capacidade);
        int valor_vizinho = resultado_vizinho.first;
        int peso_vizinho = resultado_vizinho.second;

        cout << "  - Vizinho ";
        for(int bit : vizinho) cout << bit;
        cout << " (flip no item " << i + 1 << "): Lucro: " << valor_vizinho << ", Peso: " << peso_vizinho;

        if (peso_vizinho > capacidade) {
            cout << ". Status: Invalido (excede capacidade)." << endl;
        } else if (valor_vizinho <= valor_solucao) {
            cout << ". Status: Pior ou igual." << endl;
        }
    }
    cout << "\nConclusao: Como nenhum vizinho tem lucro > " << valor_solucao << ", o algoritmo parou corretamente." << endl;
    cout << "Este pode ou nao ser o melhor resultado global possivel." << endl;
}

// --- MÓDULO PRINCIPAL DE BUSCA ---

void busca_local_mochila(const vector<Item>& itens, int capacidade, int max_iteracoes) {
    int num_itens = itens.size();
    vector<int> solucao_atual = gerar_solucao_inicial_valida(num_itens);
    int valor_atual = avaliar_solucao(solucao_atual, itens, capacidade).first;
    
    vector<int> melhor_solucao_global = solucao_atual;
    int melhor_valor_global = valor_atual;

    cout << "\nIniciando busca local..." << endl;
    cout << "Solucao Inicial (mochila vazia) -> Valor: " << valor_atual << endl;

    for (int i = 0; i < max_iteracoes; ++i) {
        vector<int> melhor_vizinho;
        int melhor_valor_vizinho = -1;

        for (int j = 0; j < num_itens; ++j) {
            vector<int> vizinho_temp = solucao_atual;
            vizinho_temp[j] = 1 - vizinho_temp[j];
            int valor_vizinho = avaliar_solucao(vizinho_temp, itens, capacidade).first;

            if (valor_vizinho > melhor_valor_vizinho) {
                melhor_valor_vizinho = valor_vizinho;
                melhor_vizinho = vizinho_temp;
            }
        }

        if (melhor_valor_vizinho > valor_atual) {
            solucao_atual = melhor_vizinho;
            valor_atual = melhor_valor_vizinho;
            if (valor_atual > melhor_valor_global) {
                melhor_valor_global = valor_atual;
                melhor_solucao_global = solucao_atual;
            }
            cout << "Iteracao " << i + 1 << ": Melhora encontrada. Novo valor: " << valor_atual << endl;
        } else {
            cout << "Iteracao " << i + 1 << ": Nenhum vizinho melhor. Otimo local atingido." << endl;
            break;
        }
    }

    cout << "\n--- Busca Finalizada ---" << endl;
    pair<int, int> resultado_final = avaliar_solucao(melhor_solucao_global, itens, capacidade);
    cout << "Melhor solucao encontrada:" << endl;
    cout << "  - Representacao binaria: ";
    for (int bit : melhor_solucao_global) cout << bit;
    cout << endl;
    
    cout << "  - Itens na mochila (ID): ";
    for(size_t i = 0; i < melhor_solucao_global.size(); ++i){
        if(melhor_solucao_global[i] == 1){
            cout << itens[i].id << " ";
        }
    }
    cout << endl;
    cout << "  - Lucro Total: " << resultado_final.first << endl;
    cout << "  - Peso Total: " << resultado_final.second << " / " << capacidade << endl;

    if(num_itens <= 15) 
    analisar_otimo_local(melhor_solucao_global, itens, capacidade);
}

// ------

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso correto: " << argv[0] << " <arquivo_de_instancia>" << endl;
        return 1;
    }
    string nome_arquivo = argv[1];
    int capacidade = 0;
    vector<Item> itens = ler_arquivo_mochila(nome_arquivo, capacidade);
    if (itens.empty()) {
        return 1;
    }
    const int MAX_ITERACOES = 1000;
    busca_local_mochila(itens, capacidade, MAX_ITERACOES);
    return 0;
}
