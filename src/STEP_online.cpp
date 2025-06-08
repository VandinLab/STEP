#include "../include/algorithms.h"

int main(int argc, char *argv[]) {

    if (argc < 5) {
        std::cout << "Usage: ./STEP_online "
                     "<dataset_path> "
                     "<delta> "
                     "<train_test_split> "
                     "<sampling_probability> "
                     "<percentage_retain> "
                     "<trials> "
                     "<seed> "
                     "<output_file> "
                     "<naive alg. sampling prob.> [optional]"<< std::endl;
        return 1;
    }

    std::string dataset_path = argv[1];
    int delta = std::stoi(argv[2]);
    float train_split = std::stof(argv[3]);
    float p = std::stof(argv[4]);
    float percentage_retain = std::stof(argv[5]);
    int trials = std::stoi(argv[6]);
    int seed = std::stoi(argv[7]);
    std::string output_file = argv[8];

    bool ground_truth = true;

    //Load edges
    char delimiter = ' ';
    t_edges edges{};
    int n_nodes = 0;
    load_preprocessed_edges(dataset_path, delimiter, edges, n_nodes);

    //Split edges in training and test set
    t_edges train_edges{};
    t_edges test_edges{};
    int train_nodes = 0;
    train_test_split(edges, train_edges, test_edges, train_nodes, train_split);

    //Clear some memory
    edges.clear();

    //Build the predictor on training data
    OnlineEdgeSet predictor;
    bool save_pred = false;

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Learning threshold on training ..." << std::endl;
    online_tmd_predictor(train_edges, train_nodes, delta, percentage_retain, predictor, save_pred);
    auto end = std::chrono::high_resolution_clock::now();
    chrono_t diff = end - start;
    std::cout << "Time to learn threshold " << diff.count() << "s" << std::endl;

    //Clear some more memory
    train_edges.clear();

    save_pred = true;

    start = std::chrono::high_resolution_clock::now();
    std::cout << "Computing temporal degrees for test set ..." << std::endl;
    online_tmd_predictor(test_edges, n_nodes, delta, 1.0, predictor, save_pred);
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "Time to compute degrees on test set " << diff.count() << "s" << std::endl;

    //Get ground truth on test set
    if(ground_truth) {
        std::cout << "Computing ground truths ..." << std::endl;
        std::tuple<counts, chrono_t, EdgeMap> res = exact_algorithm(test_edges, delta);
        counts exact_counts = std::get<0>(res);
        chrono_t exact_time = std::get<1>(res);
        save_results(exact_counts, exact_time, output_file);
    }

    //Perform STEP on test data using predictor just built

    std::cout << "Running online STEP ... " << std::endl;
    for (int i = 0; i < trials; i++) {
        std::default_random_engine rand_eng(seed + i);
        auto [predictor_counts, predictor_time] = online_step(test_edges, delta, p, predictor, rand_eng);
        save_results(predictor_counts, predictor_time, output_file);
    }
    std::cout << "Online STEP done!" << std::endl;

    //Perform sampling algorithm on test data if required
    if(argc > 9) {
        std::cout << "Running naive sampling algorithm ... " << std::endl;
        float p_naive = std::stof(argv[9]); //if needed
        for (int i = 0; i < trials; i++) {
            std::default_random_engine rand_eng(seed + i);
            auto [sampling_counts, sampling_time] = sampling_algorithm(test_edges, delta, p_naive, rand_eng);
            save_results(sampling_counts, sampling_time, output_file);
        }
        std::cout << "Naive sampling done!" << std::endl;
    }

    return 0;
}
