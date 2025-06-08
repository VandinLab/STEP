# Efficient Approximate Temporal Triangle Counting in Streaming with Predictions (ECML-PKDD 2025)

Giorgio Venturin, Ilie Sarpe, Fabio Vandin, "Efficient Approximate Temporal Triangle Counting in Streaming with Predictions", ECML-PKDD 2025.

---

Here are the instructions for running *STEP* algorithm: **S**treaming-based temporal **T**riangle counts **E**stimation with **P**redictions. The code is deployed in *C++ 17** under *gcc 9.4.0* with optimization flags enabled.    

- **Datasets** used in our experiments: <br />
  1. Stackoverflow (SO): https://snap.stanford.edu/data/sx-stackoverflow.html <br />
  2. Bitcoin (BI): https://www.cs.cornell.edu/~arb/data/temporal-bitcoin/ <br />
  3. Reddit (RE): https://www.cs.cornell.edu/~arb/data/temporal-reddit-reply/ <br />
  4. EquinixChicago (EC): to build this dataset, please follow the instructions in the `synthEquinix/README.txt` file. <br />

- **Compile STEP** algorithm and utilities using the compile.sh script: `bash compile.sh` <br />
  This script will produce the following executables (inside the `/build` directory):
    - `./STEP` (STEP algorithm - offline)
    - `./STEP_online` (STEP algorithm - online)
    - `./dataset_preprocessing`
    - `./build_predictor` </br>
  Note: the code makes use of the *ankerl hash map* from https://github.com/martinus/unordered_dense. 
  This dependency is automatically provided within the compile.sh script. </br>

- **Dataset preprocessing**: <br />
  Execute the following command to preprocess the dataset: <br />
     `./dataset_preprocessing <dataset_path> <output_path>` <br /> <br />
  where `<dataset_path>` is the path to the dataset and `<output_path>` is the path where the preprocessed dataset will be stored.

- **Build the predictor**: <br />
    Execute the following command to build the predictor: <br />
         `./build_predictor <dataset_path>
                            <delta>
                            <percentage_retain>
                            <predictor_type>
                            <predictor_file> 
                            <edge_map_flag>
                            <edge_map_file>` <br />
      where: <br />
        - `<dataset_path>` is the path to the dataset <br />
        - `<delta>` is the time duration <br />
        - `<percentage_retain>` is the percentage of edges to retain (K/m) <br />
        - `<predictor_type>` is the type of predictor (tmd: temporal min-degree predictor, perfect: heaviness predictor) <br />
        - `<predictor_file>` is the output file containing the top-K edges <br />
        - `<edge_map_flag>` is a flag to indicate if the edge map has to be saved, loaded or ignored (X: no, S: yes, L: load) <br />
           - Note: saving and loading edge maps is useful to avoid the computation of the exact solution from scratch. <br />
        - `<edge_map_file>` is the output file containing the edge map <br />

  - **Run the STEP algorithm**: <br />
    - _Offline_ mode: <br />
      Execute the following command to run the STEP algorithm in offline mode: <br />
        `./STEP <dataset_path> <delta> exact <output_file>` runs the exact algorithm <br />
        `./STEP <dataset_path> <delta> sampling <sampling_probability> <trials> <seed> <output_file>` runs the naive sampling algorithm <br />
        `./STEP <dataset_path> <delta> predictor <sampling_probability> <predictor_file> <trials> <seed> <output_file>` runs the STEP algorithm with specified predictor in <predictor_file> <br />
    - _Online_ mode: <br />
      Execute the following command to run the STEP algorithm in online mode: <br />
        `./STEP_online <dataset_path>
                       <delta> 
                       <train_test_split> 
                       <sampling_probability> 
                       <percentage_retain> 
                       <trials> 
                       <seed> 
                       <output_file> 
                       <naive alg. sampling prob.> [optional]` <br />
      The output file will contain, in order: the exact counts (first line), the STEP counts (#trials lines) and the Naive Sampling counts (#trials line). <br />
    
  - **Baselines**: <br />
    - Degeneracy implementation: https://github.com/nojanp/temporal-triangle-counting
    - EWS implementation: https://github.com/jingjing-cs/Temporal-Motif-Counting
    - Fast-Tri implementation: https://github.com/steven-ccq/FAST-temporal-motif
    - MoTTo implementation: https://github.com/bone38ljtnn/motif-counting
    - Note: EWS code was modified in order to collect the time required to perform the sampling (lines 145-158 of ews.cpp file)
  
  - **Memory measurements**: <br />
    - All memory measurements were performed using the script in `memory_measurements.sh` **contextually** to the execution of the STEP algorithm or baselines. <br />
    - The script requires a process ID in input and outputs measurements of memory consumption in MB. <br />
    - Note: we suggest to run this script within a Linux environment. <br />
