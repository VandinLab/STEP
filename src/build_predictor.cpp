#include "../include/algorithms.h"

int main(int argc, char *argv[]) {

    if(argc < 5) {
        std::cout << "Usage: ./build_predictor "
                     "<dataset_path> "
                     "<delta> "
                     "<percentage_retain> "
                     "<predictor type> [tmd, perfect] "
                     "<predictor_file> "
                     "<edge_map_flag> [S: save, L: load, X: ignore edge_map] "
                     "<edge_map_file>" << std::endl;
        return 1;
    }

    std::string dataset_path = argv[1];
    int delta = std::stoi(argv[2]);
    float percentage_retain = std::stof(argv[3]);
    std::string predictor_type = argv[4];
    std::string predictor_file = argv[5];
    char edge_map_flag = argv[6][0];

    if(predictor_type == "tmd") {

        char delimiter = ' ';
        t_edges edges;
        int n_nodes = 0;
        load_preprocessed_edges(dataset_path, delimiter, edges, n_nodes);

        EdgeSet predictor;
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Building predictor ..." << std::endl;
        build_tmd_predictor(edges, n_nodes, delta, percentage_retain, predictor);
        auto end = std::chrono::high_resolution_clock::now();
        chrono_t diff = end - start;
        std::cout << "Time to build predictor " << diff.count() << "s" << std::endl;
        save_predictor(predictor, predictor_file);
    }
    else {

        EdgeMap edge_map;

        if(edge_map_flag == 'L' && argc > 7) {
            std::string edge_map_file = argv[7];
            edge_map = load_edge_map(edge_map_file);

            if(edge_map.size() == 0) {
		        std::cout << "Error in reading edge_map" << std::endl;
		        return 1;
            }

            std::cout << "Loaded edge_map successfully" << std::endl;
        }
        else {

            char delimiter = ' ';
            t_edges edges;
            int n_nodes = 0;
            load_preprocessed_edges(dataset_path, delimiter, edges, n_nodes);
            std::cout << "Edges loaded" << std::endl;

            std::tuple<counts, chrono_t, EdgeMap> res = exact_algorithm(edges, delta);
            edge_map = std::get<2>(res);

	        if(edge_map_flag == 'S' && argc > 7) {
                std::string edge_map_file = argv[7];
		        edge_map_file = argv[7];
	    	    save_edge_map(edge_map.retain(percentage_retain), edge_map_file);
                std::cout << "Saved edge_map successfully" << std::endl;
	        }
        }

        EdgeSet predictor = edge_map.get_predictor(percentage_retain);
        save_predictor(predictor, predictor_file);
    }

    return 0;
}
