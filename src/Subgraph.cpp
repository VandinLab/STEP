#include "../include/Subgraph.h"

void Subgraph::add_edge(const Edge& e, bool heavy) {

    neighbor n = {e.get_dst(), e.get_time(), true, heavy};
    subgraph[e.get_src()].emplace_back(n);

    neighbor n_comp = {e.get_src(), e.get_time(), false, heavy};
    subgraph[e.get_dst()].emplace_back(n_comp);

    size++;
}

void Subgraph::subgraph_prune(unsigned long long curr_time, unsigned long long delta) {

    ankerl::unordered_dense::map<int, std::vector<neighbor>> new_subgraph;
    int new_size = 0;

    for(const auto& N: subgraph) {

        int node = N.first;
        std::vector<neighbor> neighborhood = N.second;

        for (const neighbor& neigh: neighborhood) {
            if (curr_time - neigh.time <= delta) {
                new_subgraph[node].emplace_back(neigh);
                new_size++;
            }
        }
    }

    this->subgraph = new_subgraph;
    this->size = new_size/2;
}