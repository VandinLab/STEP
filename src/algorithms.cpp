#include "../include/algorithms.h"

std::tuple<counts, chrono_t, EdgeMap> exact_algorithm(const t_edges& temporal_edges, unsigned long long delta) {

    auto start_time = std::chrono::high_resolution_clock::now();

    counts exact_counts{};
    EdgeMap edge_map;
    EdgeFinder ef;
    Subgraph sg;

    unsigned long long c = 0;
    unsigned long long prev_t = temporal_edges[0].get_time();

    for(const Edge &e: temporal_edges) {

        c++;
        if(c % ((unsigned long long)((double)temporal_edges.size() * 0.1)) == 0)
            std::cout<< "Processed " << c << " edges" << std::endl;

        if(e.get_time() - prev_t >= delta) {
            sg.subgraph_prune(e.get_time(), delta);
            prev_t = e.get_time();
        }

        edge_map.add_edge(e);
        sg.add_edge(e);
        ef.add_edge(e);

        count_triangles_exact(sg, ef, e, delta, exact_counts, edge_map);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    chrono_t time = end_time - start_time;

    return {exact_counts, time, edge_map};
}


std::tuple<counts, chrono_t> sampling_algorithm(const t_edges& temporal_edges,
                                                unsigned long long delta,
                                                float p,
                                                std::default_random_engine rand_eng) {

    auto start_time = std::chrono::high_resolution_clock::now();

    counts sampling_counts = {0, 0, 0, 0, 0, 0, 0, 0};
    counts triangle_counts = {0, 0, 0, 0, 0, 0, 0, 0};
    Subgraph sg;
    EdgeFinder ef;

    std::uniform_real_distribution<double> uni_dist(0.0,1.0);

    unsigned long long prev_t = temporal_edges[0].get_time();

    for(const auto &e: temporal_edges) {

        count_triangles_sampling(sg, ef, e, delta, triangle_counts);

        if(e.get_time() - prev_t > delta) {
            sg.subgraph_prune(e.get_time(), delta);
            ef.prune(e.get_time(), delta);
            prev_t = e.get_time();
        }

        double rnd = uni_dist(rand_eng);

        if(p >= rnd) {
            sg.add_edge(e);
            ef.add_edge(e);
        }
    }

    for(int i = 0; i < 8; i++)
        sampling_counts[i] = triangle_counts[i] / (p * p);

    auto end_time = std::chrono::high_resolution_clock::now();
    chrono_t time = end_time - start_time;

    return {sampling_counts, time};
}

std::tuple<counts, chrono_t> step(const t_edges& temporal_edges,
                                  unsigned long long delta,
                                  float p,
                                  EdgeSet& predictor,
                                  std::default_random_engine rand_eng) {

    auto start_time = std::chrono::high_resolution_clock::now();

    counts predictor_counts = {0, 0, 0, 0, 0, 0, 0, 0};
    counts l1 = {0, 0, 0, 0, 0, 0, 0, 0};
    counts l2 = {0, 0, 0, 0, 0, 0, 0, 0};
    counts l3 = {0, 0, 0, 0, 0, 0, 0, 0};
    Subgraph sg;
    EdgeFinder ef;

    std::uniform_real_distribution<double> uni_dist(0.0,1.0);

    unsigned long long prev_t = temporal_edges[0].get_time();

    for(const auto& e: temporal_edges) {

        count_triangles_predictor(sg, ef, e, delta, l1, l2, l3);

        if(e.get_time() - prev_t > delta) {
            sg.subgraph_prune(e.get_time(), delta);
            ef.prune(e.get_time(), delta);
            prev_t = e.get_time();
        }

        if(predictor.is_heavy(e)) {
            sg.add_edge(e, true);
            ef.add_edge(e, true);
        }
        else {
            double rnd = uni_dist(rand_eng);
            if (p >= rnd) {
                sg.add_edge(e, false);
                ef.add_edge(e, false);
            }
        }
    }

    for(int i = 0; i < 8; i++)
        predictor_counts[i] = l1[i] / (p * p) + l2[i] / p + l3[i];

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = end_time - start_time;

    return {predictor_counts, time};
}

std::tuple<counts, chrono_t> online_step(const t_edges& temporal_edges,
                                  unsigned long long delta,
                                  float p,
                                  OnlineEdgeSet& predictor,
                                  std::default_random_engine rand_eng) {

    auto start_time = std::chrono::high_resolution_clock::now();

    counts predictor_counts = {0, 0, 0, 0, 0, 0, 0, 0};
    counts l1 = {0, 0, 0, 0, 0, 0, 0, 0};
    counts l2 = {0, 0, 0, 0, 0, 0, 0, 0};
    counts l3 = {0, 0, 0, 0, 0, 0, 0, 0};
    Subgraph sg;
    EdgeFinder ef;

    std::uniform_real_distribution<double> uni_dist(0.0,1.0);

    unsigned long long prev_t = temporal_edges[0].get_time();

    for(const auto& e: temporal_edges) {

        count_triangles_predictor(sg, ef, e, delta, l1, l2, l3);

        if(e.get_time() - prev_t > delta) {
            sg.subgraph_prune(e.get_time(), delta);
            ef.prune(e.get_time(), delta);
            prev_t = e.get_time();
        }

        if(predictor.is_heavy(e)) {
            sg.add_edge(e, true);
            ef.add_edge(e, true);
        }
        else {
            double rnd = uni_dist(rand_eng);
            if (p >= rnd) {
                sg.add_edge(e, false);
                ef.add_edge(e, false);
            }
        }
    }

    for(int i = 0; i < 8; i++)
        predictor_counts[i] = l1[i] / (p * p) + l2[i] / p + l3[i];

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = end_time - start_time;

    return {predictor_counts, time};
}

void count_triangles_exact(const Subgraph &sg,
                     EdgeFinder &ef,
                     const Edge &e,
                     unsigned long long delta,
                     counts &l,
                     EdgeMap& em) {

    std::vector<neighbor> neighborhood_i, neighborhood_j;

    if (sg.contains_node(e.get_src()) && sg.contains_node(e.get_dst())) {

        neighborhood_i = sg.get_neighborhood(e.get_src());
        neighborhood_j = sg.get_neighborhood(e.get_dst());

        int x, y;
        std::vector<neighbor> neighborhood_x;

        std::size_t min_deg = std::min(neighborhood_i.size(), neighborhood_j.size());

        if (min_deg == neighborhood_i.size()) {
            x = e.get_src();
            y = e.get_dst();
            neighborhood_x = neighborhood_i;
        } else {
            x = e.get_dst();
            y = e.get_src();
            neighborhood_x = neighborhood_j;
        }

        for(int i = (int)neighborhood_x.size() - 1; i >= 0; i--) {

            neighbor z = neighborhood_x[i];

            if(e.get_time() - z.time >= delta)
                break;

            if(z.id == y)
                continue;

            std::vector<std::pair<Edge, bool>>* pairs = ef.get_edges(y, z.id);

            if(!pairs)
                continue;

            for (auto pair: *pairs) {

                if(e.get_time() - pair.first.get_time() >= delta)
                    continue;

                Edge e_x;
                Edge e_y = pair.first;

                if (z.direction)
                    e_x = Edge(x, z.id, z.time);
                else
                    e_x = Edge(z.id, x, z.time);

                wedge w;

                if (e_x.get_time() < e_y.get_time())
                    w = {e_x, e_y};
                else
                    w = {e_y, e_x};

                int tr = check_triangles(w, e, delta);
                if(tr != -1) {
                    l[tr]++;
                    em.increment_value(w.first);
                    em.increment_value(w.second);
                    em.increment_value(e);
                }
            }
        }
    }
}

void count_triangles_sampling(const Subgraph &sg,
                              EdgeFinder &ef,
                              const Edge &e,
                              unsigned long long delta,
                              counts &l) {

    std::vector<neighbor> neighborhood_i, neighborhood_j;

    if (sg.contains_node(e.get_src()) && sg.contains_node(e.get_dst())) {

        neighborhood_i = sg.get_neighborhood(e.get_src());
        neighborhood_j = sg.get_neighborhood(e.get_dst());

        int x, y;
        std::vector<neighbor> neighborhood_x;

        std::size_t min_deg = std::min(neighborhood_i.size(), neighborhood_j.size());

        if (min_deg == neighborhood_i.size()) {
            x = e.get_src();
            y = e.get_dst();
            neighborhood_x = neighborhood_i;
        } else {
            x = e.get_dst();
            y = e.get_src();
            neighborhood_x = neighborhood_j;
        }

        for(int i = (int)neighborhood_x.size() - 1; i >= 0; i--) {

            neighbor z = neighborhood_x[i];

            if(e.get_time() - z.time >= delta)
                break;

            if(z.id == y)
                continue;

            std::vector<std::pair<Edge, bool>>* pairs = ef.get_edges(y, z.id);

            if(!pairs)
                continue;

            for (auto pair: *pairs) {

                if(e.get_time() - pair.first.get_time() >= delta)
                    continue;

                Edge e_x;
                Edge e_y = pair.first;

                if (z.direction)
                    e_x = Edge(x, z.id, z.time);
                else
                    e_x = Edge(z.id, x, z.time);

                wedge w;

                if (e_x.get_time() < e_y.get_time())
                    w = {e_x, e_y};
                else
                    w = {e_y, e_x};

                int tr = check_triangles(w, e, delta);

                if(tr != -1)
                    l[tr]++;
            }
        }
    }
}

void count_triangles_predictor(const Subgraph &sg,
                            EdgeFinder &ef,
                            const Edge &e,
                            unsigned long long delta,
                            counts &l1,
                            counts &l2,
                            counts &l3) {

    std::vector<neighbor> neighborhood_i, neighborhood_j;

    if (sg.contains_node(e.get_src()) && sg.contains_node(e.get_dst())) {

        neighborhood_i = sg.get_neighborhood(e.get_src());
        neighborhood_j = sg.get_neighborhood(e.get_dst());

        int x, y;
        std::vector<neighbor> neighborhood_x;

        std::size_t min_deg = std::min(neighborhood_i.size(), neighborhood_j.size());

        if (min_deg == neighborhood_i.size()) {
            x = e.get_src();
            y = e.get_dst();
            neighborhood_x = neighborhood_i;
        } else {
            x = e.get_dst();
            y = e.get_src();
            neighborhood_x = neighborhood_j;
        }

        for(int i = (int)neighborhood_x.size() - 1; i >= 0; i--) {

            neighbor z = neighborhood_x[i];

            if(e.get_time() - z.time >= delta)
                break;

            if(z.id == y)
                continue;

            std::vector<std::pair<Edge, bool>>* pairs = ef.get_edges(y, z.id);

            if(!pairs)
                continue;

            for (auto pair: *pairs) {

                if(e.get_time() - pair.first.get_time() >= delta)
                    continue;

                Edge e_x;
                Edge e_y = pair.first;

                if (z.direction)
                    e_x = Edge(x, z.id, z.time);
                else
                    e_x = Edge(z.id, x, z.time);

                wedge w;

                if (e_x.get_time() < e_y.get_time())
                    w = {e_x, e_y};
                else
                    w = {e_y, e_x};

                if (z.heavy && pair.second) {
                    int tr = check_triangles(w, e, delta);
                    if(tr != -1)
                        l3[tr]++;
                }
                else if (z.heavy || pair.second) {
                    int tr = check_triangles(w, e, delta);
                    if(tr != -1)
                        l2[tr]++;
                }
                else {
                    int tr = check_triangles(w, e, delta);
                    if(tr != -1)
                        l1[tr]++;
                }
            }
        }
    }
}

int check_triangles(const wedge& w, const Edge &e, unsigned long long delta) {

    if(e.get_time() - w.first.get_time() >= delta)
        return -1;

    //Mackey's definition of delta-instance does not allow for edges with same timestamps
    if(e.get_time() == w.first.get_time() ||
       e.get_time() == w.second.get_time() ||
       w.first.get_time() == w.second.get_time())
        return -1;

    int u_1 = w.first.get_src();
    int v_1 = w.first.get_dst();
    int u_2 = w.second.get_src();
    int v_2 = w.second.get_dst();
    int u_3 = e.get_src();
    int v_3 = e.get_dst();

    int idx;

    if ((u_1 == u_3) && (v_1 == v_2) && (u_2 == v_3))
        idx = 0;
    else if((u_1 == v_3) && (v_1 == v_2) && (u_2 == u_3))
        idx = 1;
    else if ((u_1 == u_3) && (v_1 == u_2) && (v_2 == v_3))
        idx = 2;
    else if ((v_1 == u_2) && (v_2 == u_3) && (v_3 == u_1))
        idx = 3;
    else if ((u_1 == u_2) && (v_1 == v_3) && (v_2 == u_3))
        idx = 4;
    else if ((u_1 == v_2) && (v_1 == v_3) && (u_2 == u_3))
        idx = 5;
    else if ((u_1 == u_2) && (v_1 == u_3) && (v_2 == v_3))
        idx = 6;
    else if ((u_1 == v_2) && (v_1 == u_3) && (v_3 == u_2))
        idx = 7;
    else
        idx = -1;

    return idx;
}
