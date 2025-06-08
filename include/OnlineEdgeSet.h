#ifndef STEP_ONLINEEDGESET_H
#define STEP_ONLINEEDGESET_H

#include <ankerl/unordered_dense.h>
#include "Edge.h"

class OnlineEdgeSet {

private:
    ankerl::unordered_dense::set<Edge, edge_hashing> edge_set;
    ankerl::unordered_dense::map<Edge, int, edge_hashing> edge_map;
    int threshold = 0;

public:

    void add_edge(const Edge& e) { edge_set.insert(e);}
    bool is_heavy(const Edge& e) const { return edge_map.at(e) >= threshold; }
    std::size_t size() { return edge_set.size(); }
    void add_pair(const Edge &e, int val) { edge_map.emplace(e, val); }
    void set_threshold(int t) { threshold = t;}
    int get_threshold() const { return threshold;}
    ankerl::unordered_dense::set<Edge, edge_hashing> get_set() { return edge_set; }
    ~OnlineEdgeSet() = default;
};

#endif //STEP_ONLINEEDGESET_H
