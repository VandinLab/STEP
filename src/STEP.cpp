#include "../include/algorithms.h"

int main(int argc, char *argv[]) {

    if(argc < 5) {
        std::cout << "Usage:\n./STEP <dataset_path> <delta> exact <output_file>\n"
                     "./STEP <dataset_path> <delta> sampling <sampling probability> <trials> <seed> <output_file>\n"
                     "./STEP <dataset_path> <delta> predictor <sampling probability> <predictor_file> <trials> <seed> <output_file>\n"
                  << std::endl;
        return 1;
    }

    std::string dataset_path = argv[1];
    int delta = std::stoi(argv[2]);

    auto start = std::chrono::high_resolution_clock::now();
    char delimiter = ' ';
    t_edges temporal_edges;
    int n_nodes = 0;
    load_preprocessed_edges(dataset_path, delimiter, temporal_edges, n_nodes);
    auto end = std::chrono::high_resolution_clock::now();
    chrono_t diff = end - start;
    std::cout << "Time to load edges " << diff.count() << std::endl;

    std::cout << "Dataset: " << dataset_path << std::endl;
    std::cout << "Edges: " << temporal_edges.size() << ", Nodes: "<< n_nodes << std::endl;
    std::cout << "delta : " << delta << " s" << std::endl;

    std::string algorithm = argv[3];

    if(algorithm == "exact") {

        std::cout << "Running exact algorithm" << std::endl;
        std::string exact_out_file = argv[4];
        std::tuple<counts, chrono_t, EdgeMap> res = exact_algorithm(temporal_edges, delta);
        counts exact_counts = std::get<0>(res);
        chrono_t exact_time = std::get<1>(res);

        save_results(exact_counts,exact_time,exact_out_file);

        std::cout << "Exact algorithm done" << std::endl;
    }

    else if(algorithm == "sampling") {

        if(argc < 8) {
            std::cout << "Usage:./STEP <dataset_path> <delta> sampling <sampling probability> <trials> <seed> <output_file>\n";
            return 1;
        }

        float p = std::stof(argv[4]);
        int trials = std::stoi(argv[5]);
        int seed = std::stoi(argv[6]);
        std::string sampling_out_file = argv[7];

        std::cout << "Running naive sampling algorithm" << std::endl;
        std::cout << "sampling probability: " << p << std::endl;
        std::cout << "trials: " << trials << std::endl;
        std::cout << "seed: " << seed << std::endl;

        for (int i = 0; i < trials; i++) {

            std::default_random_engine rand_eng(seed + i);

            auto [sampling_counts, sampling_time] = sampling_algorithm(temporal_edges, delta, p, rand_eng);
            save_results(sampling_counts, sampling_time, sampling_out_file);
        }

        std::cout << "Naive sampling algorithm done" << std::endl;
    }

    else {

        if(argc < 9) {
            std::cout << "Usage:./STEP <dataset_path> <delta> predictor <sampling probability> <predictor_file> <trials> <seed> <output_file>\n";
            return 1;
        }

        float p = std::stof(argv[4]);
        std::string predictor_file = argv[5];
        int trials = std::stoi(argv[6]);
        int seed = std::stoi(argv[7]);
        std::string predictor_out_file = argv[8];

        std::cout << "Running STEP" << std::endl;
        std::cout << "sampling probability: " << p << std::endl;
        std::cout << "trials: " << trials << std::endl;
        std::cout << "seed: " << seed << std::endl;

        auto predictor = load_predictor(predictor_file);
        std::cout << "Predictor size (K) " << predictor.size() << std::endl;

        for (int i = 0; i < trials; i++) {

            std::default_random_engine rand_eng(seed + i);

            auto [predictor_counts, predictor_time] = step(temporal_edges, delta, p, predictor, rand_eng);
            save_results(predictor_counts, predictor_time, predictor_out_file);

        }
        std::cout << "STEP algorithm done" << std::endl;
    }
    return 0;
}