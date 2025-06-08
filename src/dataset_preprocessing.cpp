#include "../include/utils.h"

int main(int argc, char *argv[]) {

    if(argc < 3) {
        std::cout << "Usage: ./dataset_preprocessing "
                     "<dataset_path> "
                     "<output_path>" << std::endl;
        return 1;
    }

    std::string dataset_path = argv[1];
    std::string output_path = argv[2];

    char delimiter = ' ';
    auto temporal_edges = get_temporal_edges(dataset_path, delimiter);
    save_preprocessed_edges(temporal_edges, output_path);

    return 0;
}
