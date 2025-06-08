#ifndef STEP_EDGEFINDER_H
#define STEP_EDGEFINDER_H

#include <string>
#include <vector>
#include <algorithm>

#include "ankerl/unordered_dense.h"
#include "Edge.h"

#define MAX_N_NODES 9000000ULL

class EdgeFinder {

private:
    ankerl::unordered_dense::map<unsigned long long, std::vector<std::pair<Edge, bool>>> edge_finder;
    int size = 0;
    static unsigned long long edge_to_id(int u, int v);

public:
    void add_edge(const Edge& e, bool heavy = false);
    void prune(unsigned long long curr_time, unsigned long long delta);
    std::vector<std::pair<Edge, bool>>* get_edges(int u, int v);
    bool contains_pair(int u, int v) {return edge_finder.find(edge_to_id(u, v)) != edge_finder.end();}
    int get_size() const { return size; }
    ~EdgeFinder() = default;

};

#endif //STEP_EDGEFINDER_H