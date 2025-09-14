#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <limits>

using namespace std;

struct Cidade
{
    int id;
    double x, y;
};

double calcularDistancia(const Cidade &c1, const Cidade &c2)
{
    int xd = c1.x - c2.x;
    int yd = c1.y - c2.y;
    return (int)(sqrt(xd * xd + yd * yd) + 0.5);
}

void troca(vector<int> &vizinho, int posicao1, int posicao2)
{
    int aux = vizinho[posicao1];
    vizinho[posicao1] = vizinho[posicao2];
    vizinho[posicao2] = aux;
}

double calcularCustoTotal(const vector<int> &solucao, const vector<Cidade> &cidades)
{
    double custo = 0.0;
    for (size_t i = 0; i < solucao.size() - 1; ++i)
    {
        custo += calcularDistancia(cidades[solucao[i]], cidades[solucao[i + 1]]);
    }
    // Adiciona o custo de voltar da última cidade para a primeira
    custo += calcularDistancia(cidades[solucao.back()], cidades[solucao.front()]);
    return custo;
}

// Gera solução inicial aleatória
vector<int> gerarSolucaoInicial(int numCidades)
{
    vector<int> solucao(numCidades);
    iota(solucao.begin(), solucao.end(), 0); // Preenche a partir de 0

    // Embaralha o vetor para criar uma rota aleatória
    random_device rd;
    mt19937 g(rd());
    shuffle(solucao.begin(), solucao.end(), g);

    return solucao;
}

void buscaLocal(const vector<Cidade> &cidades)
{
    // Gera Solução Inicial
    vector<int> solucaoAtual = gerarSolucaoInicial(cidades.size());
    double custoAtual = calcularCustoTotal(solucaoAtual, cidades);

    cout << "Solucao Inicial Gerada." << endl;
    cout << "Custo Inicial: " << custoAtual << endl
         << endl;

    // Critério de Parada
    int iteracoesSemMelhora = 0;

    bool melhorou = true;
    while (melhorou)
    {
        melhorou = false;
        double melhorCustoVizinho = custoAtual;
        vector<int> melhorVizinho = solucaoAtual;

        for (size_t i = 0; i < cidades.size(); ++i)
        {
            for (size_t j = i + 1; j < cidades.size(); ++j)
            {
                vector<int> vizinho = solucaoAtual;
                troca(vizinho, i, j); // Troca cidades vizinhas na solucao

                double custoVizinho = calcularCustoTotal(vizinho, cidades);

                if (custoVizinho < melhorCustoVizinho) // Melhorou
                {
                    melhorCustoVizinho = custoVizinho;
                    melhorVizinho = vizinho;
                    if (melhorCustoVizinho < custoAtual)
                    {
                        solucaoAtual = melhorVizinho;
                        custoAtual = melhorCustoVizinho;
                        melhorou = true;
                        goto fimDaBuscaNaVizinhanca;
                    }
                }
            }
        }
    fimDaBuscaNaVizinhanca:;
    }

    cout << "Custo Final: " << custoAtual << endl;
    cout << "Rota Final: ";
    for (int id : solucaoAtual)
    {
        cout << cidades[id].id << " -> ";
    }
    cout << cidades[solucaoAtual.front()].id << endl;
}

int main()
{
    string nomeArquivo;
    cout << "Digite o nome do arquivo de instancia do TSP: ";
    cin >> nomeArquivo;

    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open())
    {
        cerr << "Erro: Nao foi possivel abrir o arquivo " << nomeArquivo << endl;
        return 1;
    }

    // Ignora o cabeçalho até encontrar "NODE_COORD_SECTION"
    string linha;
    while (getline(arquivo, linha))
    {
        if (linha.find("NODE_COORD_SECTION") != string::npos)
        {
            break;
        }
    }

    // Le as coordenadas das cidades
    vector<Cidade> cidades;
    int id;
    double x, y;

    while (arquivo >> id >> x >> y)
    {
        cidades.push_back({id, x, y});
    }
    arquivo.close();

    if (cidades.empty())
    {
        cerr << "Erro: Nenhuma cidade foi lida do arquivo." << endl;
        return 1;
    }

    cout << "\nIniciando Busca Local com First Improvement..." << endl;
    buscaLocal(cidades);

    return 0;
}