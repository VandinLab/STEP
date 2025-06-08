#ifndef STEP_EDGE_H
#define STEP_EDGE_H

#include <string>
#include <iostream>

class Edge {

private:
    int src;
    int dst;
    unsigned long long t;

public:
    Edge() {};
    Edge(int source, int destination, unsigned long long time);
    int get_src() const { return src; }
    int get_dst() const { return dst; }
    unsigned long long get_time() const { return t; }
    bool operator==(const Edge& e) const;
    bool operator>(const Edge& e) const;
    bool operator<(const Edge& e) const;
    ~Edge() = default;
};

struct edge_hashing {

    size_t operator()(const Edge& p) const
    {

        auto hash1 = std::hash<int>{}(p.get_src());
        auto hash2 = std::hash<int>{}(p.get_dst());
        auto hash3 = std::hash<unsigned long long>{}(p.get_time());

        return hash1 ^ hash2 ^ hash3;
    }
};

std::ostream& operator<<(std::ostream& os, const Edge& e);

#endif //STEP_EDGE_H
