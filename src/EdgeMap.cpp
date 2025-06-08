#include "../include/EdgeMap.h"

bool compare_heavy(std::pair<Edge, int> p1, std::pair<Edge, int> p2) { return p1.second > p2.second; } //descending order

EdgeSet EdgeMap::get_predictor(float percentage_retain) {

    EdgeSet predictor;

    std::vector<std::pair<Edge, int>> tmp;
    tmp.reserve(edge_map.size());

    for(auto item : edge_map)
        tmp.emplace_back(item);

    std::sort(tmp.begin(), tmp.end(), compare_heavy);

    for(auto item : tmp)
        if(predictor.size() < (int)(percentage_retain * (float)tmp.size()))
            predictor.add_edge(item.first);
        else
            break;

    return predictor;
}

EdgeMap EdgeMap::retain(float percentage_retain) {

    EdgeMap em;

    std::vector<std::pair<Edge, int>> tmp;
    tmp.reserve(edge_map.size());
    for(auto item : edge_map)
        tmp.emplace_back(item);

    std::sort(tmp.begin(), tmp.end(), compare_heavy);

    for(auto item : tmp)
        if(em.size() < (int)(percentage_retain * (float)tmp.size()))
            em.add_edge(item.first, item.second);
        else
            break;

    return em;
}
