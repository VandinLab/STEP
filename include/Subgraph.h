#ifndef STEP_SUBGRAPH_H
#define STEP_SUBGRAPH_H

#include <string>
#include <vector>
#include <algorithm>

#include "ankerl/unordered_dense.h"
#include "Edge.h"

struct neighbor {
    int id;
    unsigned long long time;
    bool direction;
    bool heavy = false;
    bool operator==(const neighbor& n) const { return (this->id == n.id &&
                                                       this->time == n.time &&
                                                       this->direction == n.direction); }
};

class Subgraph {

private:
    ankerl::unordered_dense::map<int, std::vector<neighbor>> subgraph;
    int size = 0;
public:
    void add_edge(const Edge& e, bool heavy = false);
    void subgraph_prune(unsigned long long curr_time, unsigned long long delta);
    std::vector<neighbor> get_neighborhood(int node) const { return subgraph.at(node); }
    bool contains_node(int node) const { return subgraph.find(node) != subgraph.end(); }
    int get_size() const { return size; }
    void remove_edge(const Edge& e);
    ~Subgraph() = default;
};


#endif //STEP_SUBGRAPH_H
