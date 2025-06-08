#ifndef STEP_EDGESET_H
#define STEP_EDGESET_H

#include <ankerl/unordered_dense.h>
#include "Edge.h"

class EdgeSet {

private:
    ankerl::unordered_dense::set<Edge, edge_hashing> edge_set;

public:

    bool is_heavy(const Edge &e) const { return edge_set.find(e) != edge_set.end(); }
    void add_edge(const Edge& e) { edge_set.insert(e);}
    std::size_t size() { return edge_set.size(); }
    ankerl::unordered_dense::set<Edge, edge_hashing> get_set() { return edge_set; }
    ~EdgeSet() = default;
};

#endif //STEP_EDGESET_H
