#ifndef STEP_EDGEMAP_H
#define STEP_EDGEMAP_H

#include <vector>
#include <algorithm>
#include "EdgeSet.h"

class EdgeMap {

private:
    ankerl::unordered_dense::map<Edge, int, edge_hashing> edge_map;
public:
    void add_edge(const Edge& e, int value = 0) { edge_map[e] = value; }
    void increment_value(const Edge& e, int val = 1) { edge_map.at(e) += val; }
    EdgeSet get_predictor(float percentage_retain);
    std::size_t size() { return edge_map.size(); }
    ankerl::unordered_dense::map<Edge, int, edge_hashing> get_map() const { return edge_map; }
    EdgeMap retain(float percentage_retain);
    ~EdgeMap() = default;

};

#endif