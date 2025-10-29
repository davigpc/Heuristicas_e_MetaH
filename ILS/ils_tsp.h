#ifndef ILS_TSP_H
#define ILS_TSP_H

#include <vector>
#include <string>

struct cidade
{
    int id;
    double x, y;
};

std::vector<cidade> ler_arquivo_tsp(const std::string& nome_arquivo);

double iterated_local_search_tsp(const std::vector<cidade>& cidades, int ils_max_iter);

#endif // ILS_TSP_H