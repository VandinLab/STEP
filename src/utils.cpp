#include "../include/utils.h"

t_edges get_temporal_edges(const std::string& dataset_path, char delimiter) {

    std::ifstream file(dataset_path);
    std::string line;
    std::vector<Edge> temporal_edges;
    ankerl::unordered_dense::set<Edge, edge_hashing> edge_set;
    ankerl::unordered_dense::map<int, int> nodes_remapping;

    int n_nodes = 0;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));
        unsigned long long time = std::stoull(string_edge.at(2));

        if(!nodes_remapping.contains(src)) {
            nodes_remapping.emplace(src, n_nodes);
            n_nodes++;
        }

        if(!nodes_remapping.contains(dst)) {
            nodes_remapping.emplace(dst, n_nodes);
            n_nodes++;
        }

        //avoid self loops
        if (src == dst)
            continue;

        Edge e(nodes_remapping.at(src), nodes_remapping.at(dst), time);

        //avoid duplicate edges
        if(edge_set.find(e) != edge_set.end())
            continue;

        temporal_edges.push_back(e);
        edge_set.emplace(e);
    }

    std::cout << "Number of nodes " << n_nodes << std::endl;
    std::cout << "Number of edges " << temporal_edges.size() << std::endl;

    std::sort(temporal_edges.begin(), temporal_edges.end());
    return temporal_edges;
}


void build_tmd_predictor(t_edges& edges,
                         int n_nodes,
                         unsigned long long delta,
                         float percentage_retain,
                         EdgeSet& predictor) {

    Edge* low_idx = &edges[0];
    Edge* curr_idx = &edges[0];
    Edge* high_idx = &edges[0];
    auto *deg = new int[n_nodes];

    for(int i = 0; i < n_nodes; i++)
        deg[i] = 0;

    std::size_t queue_size;

    if(percentage_retain == 1.0)
        queue_size = edges.size();
    else
        queue_size = (std::size_t)(percentage_retain * (float)edges.size());

    FixedSizePQ<std::pair<Edge, int>, edge_comparator> queue(queue_size);

    //Initialization
    while(high_idx->get_time() < curr_idx->get_time() + delta || high_idx->get_time() == (high_idx + 1)->get_time()) {
        ++deg[high_idx->get_src()];
        ++deg[high_idx->get_dst()];
        high_idx = high_idx + 1;
    }

    ++deg[curr_idx->get_src()];
    ++deg[curr_idx->get_dst()];

    //Starting the algorithm
    while(curr_idx != &edges[edges.size()]) {

        //Until the lower edge is not inside the window, go on in the stream
        //Update counters accordingly with exiting edges

        unsigned long long lower_bound = curr_idx->get_time() > delta ? curr_idx->get_time() - delta : 0;

        while(low_idx->get_time() < lower_bound ||
              low_idx->get_time() == (low_idx + 1)->get_time())
        {
            --deg[low_idx->get_src()];
            --deg[low_idx->get_dst()];
            low_idx = low_idx + 1;
        }

        //Until the higher edge is not the farest possible, go on in the stream
        //Update counters accordingly with new entering edges
        while (high_idx->get_time() < curr_idx->get_time() + delta ||
               high_idx->get_time() == (high_idx + 1)->get_time() &&
               high_idx != &edges[edges.size() - 1])
        {
            if(high_idx == &edges[edges.size() - 1] ||
               (high_idx + 1)->get_time() > curr_idx->get_time() + delta)
                break;
            else {
                high_idx = high_idx + 1;
                ++deg[high_idx->get_src()];
                ++deg[high_idx->get_dst()];
            }
        }

        int pred = std::min(deg[curr_idx->get_src()], deg[curr_idx->get_dst()]);

        if(queue.size() < queue_size) {
            queue.push(std::make_pair(*curr_idx, pred));
        }
        else {
            if (pred > queue.top().second) { //compare current edge with lightest one in queue
                queue.pop();
                queue.push(std::make_pair(*curr_idx, pred));
            }
        }

        curr_idx++;
    }

    delete [] deg;

    while(!queue.empty()) {
        predictor.add_edge(queue.top().first);
        queue.pop();
    }
}

void online_tmd_predictor(t_edges& edges,
                         int n_nodes,
                         unsigned long long delta,
                         float percentage_retain,
                         OnlineEdgeSet& predictor,
                         bool save_pred) {

    Edge* low_idx = &edges[0];
    Edge* curr_idx = &edges[0];
    Edge* high_idx = &edges[0];
    auto *deg = new int[n_nodes];

    for(int i = 0; i < n_nodes; i++)
        deg[i] = 0;

    std::size_t queue_size;

    if(percentage_retain == 1.0)
        queue_size = edges.size();
    else
        queue_size = (std::size_t)(percentage_retain * (float)edges.size());

    FixedSizePQ<std::pair<Edge, int>, edge_comparator> queue(queue_size);

    //Initialization
    while(high_idx->get_time() < curr_idx->get_time() + delta || high_idx->get_time() == (high_idx + 1)->get_time()) {
        ++deg[high_idx->get_src()];
        ++deg[high_idx->get_dst()];
        high_idx = high_idx + 1;
    }

    ++deg[curr_idx->get_src()];
    ++deg[curr_idx->get_dst()];

    //Starting the algorithm
    while(curr_idx != &edges[edges.size()]) {

        //Until the lower edge is not inside the window, go on in the stream
        //Update counters accordingly with exiting edges

        unsigned long long lower_bound = curr_idx->get_time() > delta ? curr_idx->get_time() - delta : 0;

        while(low_idx->get_time() < lower_bound ||
              low_idx->get_time() == (low_idx + 1)->get_time())
        {
            --deg[low_idx->get_src()];
            --deg[low_idx->get_dst()];
            low_idx = low_idx + 1;
        }

        //Until the higher edge is not the farest possible, go on in the stream
        //Update counters accordingly with new entering edges
        while (high_idx->get_time() < curr_idx->get_time() + delta ||
               high_idx->get_time() == (high_idx + 1)->get_time() &&
               high_idx != &edges[edges.size() - 1])
        {
            if(high_idx == &edges[edges.size() - 1] ||
               (high_idx + 1)->get_time() > curr_idx->get_time() + delta)
                break;
            else {
                high_idx = high_idx + 1;
                ++deg[high_idx->get_src()];
                ++deg[high_idx->get_dst()];
            }
        }

        int pred = std::min(deg[curr_idx->get_src()], deg[curr_idx->get_dst()]);

        if(queue.size() < queue_size) {
            queue.push(std::make_pair(*curr_idx, pred));
        }
        else {
            if (pred > queue.top().second) { //compare current edge with lightest one in queue
                queue.pop();
                queue.push(std::make_pair(*curr_idx, pred));
            }
        }

        curr_idx++;
    }

    delete [] deg;

    if(save_pred) {
        while(!queue.empty()) {
            predictor.add_pair(queue.top().first, queue.top().second);
            predictor.add_edge(queue.top().first);
            queue.pop();
        }
    }
    else
        predictor.set_threshold(queue.top().second);
}

void save_predictor(EdgeSet& predictor, const std::string& file) {
    std::ofstream of(file);
    for(const auto& elem : predictor.get_set())
        of << elem << "\n";
    of.close();
}

EdgeSet load_predictor(const std::string& predictor_file) {

    std::ifstream file(predictor_file);
    std::string line;
    std::string delimiter = " ";
    EdgeSet predictor;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter[0]))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));
        unsigned long long time = std::stoull(string_edge.at(2));

        Edge e(src, dst, time);
        predictor.add_edge(e);
    }

    file.close();

    return predictor;
}

void save_edge_map(const EdgeMap& edge_map, const std::string& file) {
    std::ofstream of(file);
    std::string delimiter = " ";
    for(auto elem : edge_map.get_map())
        of << elem.first << delimiter << elem.second << "\n";
    of.close();
}

EdgeMap load_edge_map(const std::string& edge_map_file) {

    std::ifstream file(edge_map_file);
    std::string line;
    char delimiter = ' ';
    EdgeMap edge_map;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));
        unsigned long long time = std::stoull(string_edge.at(2));
        int heaviness = std::stoi(string_edge.at(3));

        Edge e(src, dst, time);
        edge_map.add_edge(e);
        edge_map.increment_value(e, heaviness);
    }

    file.close();

    return edge_map;
}

void save_results(counts count_res, chrono_t time, const std::string& file) {

    std::ofstream of(file, std::ios_base::app);
    std::string delimiter = " ";

    for(int i = 0; i < 8; i++)
        of << count_res[i] << delimiter;
    of << time.count() << "\n";

    of.close();
}

void save_preprocessed_edges(const std::vector<Edge>& edges, const std::string& file) {

    std::ofstream of(file);
    std::string delimiter = " ";

    for(Edge e : edges)
        of << e.get_src() << delimiter << e.get_dst() << delimiter << e.get_time() << "\n";

    of.close();
}

void load_preprocessed_edges(const std::string& preprocessed_dataset, char delimiter, t_edges& edges, int& n_nodes) {

    std::ifstream file(preprocessed_dataset);
    std::string line;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));
        unsigned long long time = std::stoull(string_edge.at(2));

        edges.emplace_back(src, dst, time);

        if(n_nodes < std::max(src, dst))
            n_nodes = std::max(src, dst);
    }

    ++n_nodes;
}

std::size_t get_num_nodes(const std::string& preprocessed_dataset, char delimiter) {

    std::vector<Edge> temporal_edges;
    std::ifstream file(preprocessed_dataset);
    std::string line;
    int n_nodes = 0;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));

        if(n_nodes < std::max(src, dst))
            n_nodes = std::max(src, dst);

    }

    return ++n_nodes;
}

void train_test_split(t_edges& edges, t_edges& train_set, t_edges& test_set, int& train_nodes, float train_split) {

    auto n_train = (std::size_t)(train_split * (float)edges.size());

    unsigned long long time_rescale = edges[0].get_time();

    for(std::size_t i = 0; i < n_train; i++) {

        int u = edges[i].get_src();
        int v = edges[i].get_dst();

        Edge e = Edge(u,v,edges[i].get_time() - time_rescale);
        train_set.emplace_back(e);

        if(train_nodes < std::max(u, v))
            train_nodes = std::max(u, v);
    }

    ++train_nodes;

    time_rescale = edges[n_train].get_time();

    for(std::size_t i = n_train; i < edges.size(); i++) {
        Edge e = Edge(edges[i].get_src(),edges[i].get_dst(),edges[i].get_time() - time_rescale);
        test_set.emplace_back(e);
    }
}
