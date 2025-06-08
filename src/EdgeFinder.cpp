#include "../include/EdgeFinder.h"

void EdgeFinder::add_edge(const Edge& e, bool heavy) {
    int u = e.get_src();
    int v = e.get_dst();
    unsigned long long id = edge_to_id(u, v);
    edge_finder[id].emplace_back(e, heavy);
    size++;
}

unsigned long long EdgeFinder::edge_to_id(int u, int v) {
    int nu = (u < v ? u : v);
    int nv = (u < v ? v : u);
    unsigned long long id = (MAX_N_NODES * static_cast<unsigned long long>(nu) +
                             static_cast<unsigned long long>(nv));
    return id;
}

void EdgeFinder::prune(unsigned long long curr_time, unsigned long long delta) {
    ankerl::unordered_dense::map<unsigned long long, std::vector<std::pair<Edge, bool>>> new_edge_finder;
    int new_size = 0;
    for(const auto& N: edge_finder) {
        unsigned long long id = N.first;
        std::vector<std::pair<Edge, bool>> pairs = N.second;
        for (const std::pair<Edge, bool>& p: pairs)
            if (curr_time - p.first.get_time() <= delta) {
                new_edge_finder[id].emplace_back(p.first, p.second);
                new_size++;
            }
    }
    this->edge_finder = new_edge_finder;
    this->size = new_size/2;
}

std::vector<std::pair<Edge, bool>>* EdgeFinder::get_edges(int u, int v) {

    auto edges = edge_finder.find(edge_to_id(u, v));

    if(edges != edge_finder.end())
        return &edges->second;
    else
        return nullptr;
}