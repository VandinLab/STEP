#ifndef STEP_ALGORITHMS_H
#define STEP_ALGORITHMS_H

#include <random>

#include "Subgraph.h"
#include "EdgeFinder.h"
#include "utils.h"

#define wedge std::pair<Edge, Edge>

std::tuple<counts, chrono_t, EdgeMap> exact_algorithm(const t_edges& temporal_edges, unsigned long long delta);

std::tuple<counts, chrono_t> sampling_algorithm(const t_edges& temporal_edges,
                                                unsigned long long delta,
                                                float p,
                                                std::default_random_engine rand_eng);

std::tuple<counts, chrono_t> step(const t_edges& temporal_edges,
                                  unsigned long long delta,
                                  float p,
                                  EdgeSet& predictor,
                                  std::default_random_engine rand_eng);

std::tuple<counts, chrono_t> online_step(const t_edges& temporal_edges,
                                  unsigned long long delta,
                                  float p,
                                  OnlineEdgeSet& predictor,
                                  std::default_random_engine rand_eng);

void count_triangles_exact(const Subgraph &sg,
                              EdgeFinder &ef,
                              const Edge &e,
                              unsigned long long delta,
                              counts &l,
                              EdgeMap& em);

void count_triangles_sampling(const Subgraph &sg,
                              EdgeFinder &ef,
                              const Edge &e,
                              unsigned long long delta,
                              counts &l);

void count_triangles_predictor(const Subgraph &sg,
                            EdgeFinder &nm,
                            const Edge &e,
                            unsigned long long delta,
                            counts &l1,
                            counts &l2,
                            counts &l3);

int check_triangles(const wedge &w, const Edge& e, unsigned long long delta);

#endif //STEP_ALGORITHMS_H