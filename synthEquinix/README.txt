## Utility to add edges to a bipartite graph

In this folder you find a simple tool that we used to add edges to the EquinixChicago Dataset (that is bipartite)

1. First compile the program, just run `g++ main.cpp -O3 -o addEdges -fopenmp`
2. Download the bipartite graph from (https://github.com/VandinLab/PRESTO/): https://drive.google.com/drive/folders/1HXMEO4wwMOT1H9icwiP6siQTp2sm5pgA
3. Run `./addEdges <path_to_EquinixChicago.txt> 1` 
4. This will produce the file `out.txt` which constitutes of the new edges to be added to the original biparite network
5. Run `./addEdges <path_to_EquinixChicago.txt> 2 out.txt` 
6. This step will merge the two files in a single dataset, sorting all edges as required and remapping ids if needed the final file will be in this folder and the name will be "EquinixChicago-new-sorted.txt" NOTE: the final file will weight more than 70Gb (tot edges: 3365718759)!

NOTE: Due to multithreading you may not obtain the same network we used as the access to the random generator is not sequential.
