#ifndef STEP_UTILS_H
#define STEP_UTILS_H

#include <fstream>
#include <sstream>
#include <iomanip>

#include <cmath>
#include <chrono>

#include "EdgeMap.h"
#include "OnlineEdgeSet.h"
#include "FixedSizePQ.h"

#define counts std::array<double, 8>
#define t_edges std::vector<Edge>
#define chrono_t std::chrono::duration<double>

struct edge_comparator {
    bool operator()(const std::pair<Edge, int> &a, const std::pair<Edge, int> &b) const {
        return a.second > b.second;
    }
};

t_edges get_temporal_edges(const std::string& dataset_path, char delimiter);
void save_preprocessed_edges(const std::vector<Edge>& edges, const std::string& file);
void load_preprocessed_edges(const std::string& preprocessed_dataset,
                                                char delimiter,
                                                t_edges& edges,
                                                int& n_nodes);
std::size_t get_num_nodes(const std::string& preprocessed_dataset, char delimiter);

void build_tmd_predictor(t_edges& edges,
                         int n_nodes,
                         unsigned long long delta,
                         float percentage_retain,
                         EdgeSet& predictor);

void online_tmd_predictor(t_edges& edges,
                         int n_nodes,
                         unsigned long long delta,
                         float percentage_retain,
                         OnlineEdgeSet& predictor,
                         bool save_pred);

void save_results(counts count_res, chrono_t time, const std::string& file);

void save_edge_map(const EdgeMap& edge_map, const std::string& file);
EdgeMap load_edge_map(const std::string& edge_map_file);

void save_predictor(EdgeSet& predictor, const std::string& file);
EdgeSet load_predictor(const std::string& predictor_file);

void train_test_split(t_edges& edges, t_edges& train_set, t_edges& test_set, int& train_nodes, float train_split);

#endif //STEP_UTILS_H
