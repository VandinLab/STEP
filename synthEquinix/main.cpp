#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <functional>

using namespace std;

inline std::vector<long long int> sublinearBinomialSample(long long int trials, double probability, std::mt19937_64 generator){
	std::vector<long long int> sampled;
	if(trials == 0)
		return sampled;
	long long int position  = 0;
	std::uniform_real_distribution<double> unif(0.0, 1.0);
	double x = unif(generator);
	double c = std::log(1-probability);
	if(x < probability)
	{
		sampled.push_back(position);
	}
	while(position < trials)
	{
		x = unif(generator);
		double y = std::log(x)/c;
		position = position + std::floor(y) + 1;
		if(position < trials)
		{
			sampled.push_back(position);
		}
	}
	return sampled;
}

long long int parseDataFile(std::string fileName, std::vector<std::vector<long long int>>& ed)
{
	std::ifstream fileG(fileName);
	long long int nodes = 0;
	if(fileG.is_open())
	{
		std::string line;
		long int ID = 0;
		int self_edges = 0;
		long long prev = 0;
		while(getline(fileG, line))
		{
			std::istringstream iss(line);
			std::vector<std::string> results(std::istream_iterator<
						std::string>{iss},std::istream_iterator<std::string>());
			if(results.size() != 3)
			{
				std::cerr << "ERROR, dataset is not in format src dst timestamp" << std::endl;
				exit(1);
			}
			else
			{
				long long int src =stoll(results[0]);
				long long int dst =stoll(results[1]);
				long long int timestamp = stoll(results[2]);
				if(src != dst)
				{
					std::vector<long long int> curredge{src, dst, timestamp};
					ed.push_back(curredge);
					nodes = max(nodes, src);
					nodes = max(nodes, dst);
					//TEdge edge = {.src=src, .dst=dst, .tim=timestamp, .id=ID++};
					//edges_.push_back(edge);
				}
				else
					self_edges++;
			}
		}
		std::cout << "removed edges is: " << self_edges << std::endl;
	}
	fileG.close();
	nodes++;
	return nodes;
}

int main(int argc, char *argv[])
{
	// Read dataset
	std::string type{argv[2]};
	if(type == "1")
	{
		std::vector<std::vector<long long int>> edges;
		std::vector<std::vector<long long int>> AdjMapOut;
		std::vector<std::vector<long long int>> AdjMapIn;
		std::vector<std::unordered_map<int, std::vector<long long int> > > times;
		std::string filename{argv[1]};
		long long int nodes = parseDataFile(filename, std::ref(edges));
		std::cout << "Tot nodes: " << nodes << '\n';
		AdjMapOut.resize(nodes);
		AdjMapIn.resize(nodes);
		times.resize(nodes);
		for(auto& edge : edges)
		{
			AdjMapOut[edge[0]].push_back(edge[1]);	
			AdjMapIn[edge[1]].push_back(edge[0]);	
			if(times[edge[0]].find(edge[1]) !=times[edge[0]].end())
				times[edge[0]][edge[1]].push_back(edge[2]);
			else
			{
				times[edge[0]].insert(std::make_pair(edge[1], std::vector<long long int>{edge[2]}));
			}

		}
		std::vector<std::vector<long long int>> toAdd;
		std::mt19937_64 gen(200);
		std::bernoulli_distribution coin(0.5); // unbiased coin, if 0 add edge src -> dst, else dst <- src
		std::bernoulli_distribution coinSkipFullNode(0.0001);
			
		std::string fout = "out.txt";
		ofstream fileout;
		fileout.open(fout);
		long long int const TOEXPLORE = 4;
		long long int const TOEXPLORENEI = 4;
		long long int const SAMPLETIMES = 4;
		long long int const TOTTIMES = 16;
		long long int percentSize = nodes/100;
		#pragma omp parallel for num_threads(10)
		for(long long int i=0; i < nodes; i++)
		{
			// i is the src node
			if((i%(5*percentSize) == 0))
				#pragma omp critical
				std::cout << "Current percent is: " << i/percentSize << " and i: " << i<< std::endl;

			if(coinSkipFullNode(gen))
				continue;

			double currentProb{1.0};
			if(AdjMapOut[i].size() > TOEXPLORE)
			{
				currentProb = TOEXPLORE/(1.0*AdjMapOut[i].size());
			}
			auto positions = sublinearBinomialSample(AdjMapOut[i].size(), currentProb, gen);

			for(auto pos1 : positions) // Expected 4 neighbors
			{

				auto node1 = *(AdjMapOut[i].cbegin()+pos1);
				std::unordered_set<long long int> uniqueNeigh;

				double probOutNei{1.0};
				if(AdjMapOut[node1].size() > TOEXPLORENEI)
				{
					probOutNei = TOEXPLORENEI/(1.0*AdjMapOut[node1].size());
				}
				auto positionsOut = sublinearBinomialSample(AdjMapOut[node1].size(), probOutNei, gen);
				for(auto pos2 : positionsOut)
				{
					auto node2 = *(AdjMapOut[node1].cbegin()+pos2);
					if((node1 == node2) || (node2 ==i))
						continue;
					if(uniqueNeigh.find(node2) == uniqueNeigh.end())
						uniqueNeigh.emplace(node2);
				}

				double probInNei{1.0};
				if(AdjMapIn[node1].size() > TOEXPLORENEI)
				{
					probInNei = TOEXPLORENEI/(1.0*AdjMapIn[node1].size());
				}
				auto positionsIn = sublinearBinomialSample(AdjMapIn[node1].size(), probInNei, gen);
				for(auto pos2 : positionsIn)
				{
					auto node2 = *(AdjMapIn[node1].cbegin()+pos2);
					if((node1 == node2) || (node2 ==i))
						continue;
					if(uniqueNeigh.find(node2) == uniqueNeigh.end())
						uniqueNeigh.emplace(node2);
				}
				//std::cout << "Size of second neighborhood: " << uniqueNeigh.size() << std::endl;

				for(auto& node2 : uniqueNeigh) // should be 4 nodes in expectation
				{
					auto timesI1 = times[i][node1];
					std::vector<long long int> times12;
					if(times[node1].find(node2) != times[node1].end())
						times12.insert(times12.end(), times[node1][node2].begin(), times[node1][node2].end());
					if(times[node2].find(node1) != times[node2].end())
						times12.insert(times12.end(), times[node2][node1].begin(), times[node2][node1].end());
					long long int totTims = timesI1.size() * times12.size();
					if(totTims > TOTTIMES)
					{
						if(timesI1.size() > SAMPLETIMES)
						{
							double probt1{SAMPLETIMES/(1.*timesI1.size())};
							auto positionsT1 = sublinearBinomialSample(timesI1.size(), probt1, gen);
							std::vector<long long int> newT1;
							for(auto pt1 : positionsT1)
							{
								newT1.push_back(*(timesI1.cbegin()+pt1));
							}
							timesI1 = newT1;
						}
						if(times12.size() > SAMPLETIMES)
						{
							double probt2{SAMPLETIMES/(1.*times12.size())};
							auto positionsT2 = sublinearBinomialSample(times12.size(), probt2, gen);
							std::vector<long long int> newT2;
							for(auto pt2 : positionsT2)
							{
								newT2.push_back(*(times12.cbegin()+pt2));
							}
							times12 = newT2;
						}
					}
					for(auto tim1 : timesI1)
					{
						for(auto tim2 : times12)
						{
							auto mintim = (tim1 <= tim2) ? tim1 : tim2;
							auto maxtim = (mintim == tim1) ? tim2 : tim1;
							std::uniform_int_distribution<long long int> timeDistr(mintim, maxtim);
							auto mytim = timeDistr(gen);
							if(coin(gen))
							{
								//toAdd.push_back(std::vector<long long int>{i, node2, mytim});
								#pragma omp critical
								fileout << i << " " << node2 << " " << mytim << '\n';
							}
							else
							{
								#pragma omp critical
								fileout << node2 << " " << i << " " << mytim << '\n';
							}
								//toAdd.push_back(std::vector<long long int>{node2, i, mytim});
						}
					}
				}
			}

			currentProb=1.0;
			if(AdjMapIn[i].size() > TOEXPLORE)
			{
				currentProb = TOEXPLORE/(1.0*AdjMapIn[i].size());
			}
			positions = sublinearBinomialSample(AdjMapIn[i].size(), currentProb, gen);

			for(auto pos1 : positions) // Expected 4 neighbors
			{

				auto node1 = *(AdjMapIn[i].cbegin()+pos1);
				std::unordered_set<long long int> uniqueNeigh;

				double probOutNei{1.0};
				if(AdjMapOut[node1].size() > TOEXPLORENEI)
				{
					probOutNei = TOEXPLORENEI/(1.0*AdjMapOut[node1].size());
				}
				auto positionsOut = sublinearBinomialSample(AdjMapOut[node1].size(), probOutNei, gen);
				for(auto pos2 : positionsOut)
				{
					auto node2 = *(AdjMapOut[node1].cbegin()+pos2);
					if((node1 == node2) || (node2 ==i))
						continue;
					if(uniqueNeigh.find(node2) == uniqueNeigh.end())
						uniqueNeigh.emplace(node2);
				}

				double probInNei{1.0};
				if(AdjMapIn[node1].size() > TOEXPLORENEI)
				{
					probInNei = TOEXPLORENEI/(1.0*AdjMapIn[node1].size());
				}
				auto positionsIn = sublinearBinomialSample(AdjMapIn[node1].size(), probInNei, gen);
				for(auto pos2 : positionsIn)
				{
					auto node2 = *(AdjMapIn[node1].cbegin()+pos2);
					if((node1 == node2) || (node2 ==i))
						continue;
					if(uniqueNeigh.find(node2) == uniqueNeigh.end())
						uniqueNeigh.emplace(node2);
				}
				//std::cout << "Size of second neighborhood: " << uniqueNeigh.size() << std::endl;

				for(auto& node2 : uniqueNeigh) // should be 4 nodes in expectation
				{
					auto timesI1 = times[i][node1];
					std::vector<long long int> times12;
					if(times[node1].find(node2) != times[node1].end())
						times12.insert(times12.end(), times[node1][node2].begin(), times[node1][node2].end());
					if(times[node2].find(node1) != times[node2].end())
						times12.insert(times12.end(), times[node2][node1].begin(), times[node2][node1].end());
					long long int totTims = timesI1.size() * times12.size();
					if(totTims > TOTTIMES)
					{
						if(timesI1.size() > SAMPLETIMES)
						{
							double probt1{SAMPLETIMES/(1.*timesI1.size())};
							auto positionsT1 = sublinearBinomialSample(timesI1.size(), probt1, gen);
							std::vector<long long int> newT1;
							for(auto pt1 : positionsT1)
							{
								newT1.push_back(*(timesI1.cbegin()+pt1));
							}
							timesI1 = newT1;
						}
						if(times12.size() > SAMPLETIMES)
						{
							double probt2{SAMPLETIMES/(1.*times12.size())};
							auto positionsT2 = sublinearBinomialSample(times12.size(), probt2, gen);
							std::vector<long long int> newT2;
							for(auto pt2 : positionsT2)
							{
								newT2.push_back(*(times12.cbegin()+pt2));
							}
							times12 = newT2;
						}
					}
					for(auto tim1 : timesI1)
					{
						for(auto tim2 : times12)
						{
							auto mintim = (tim1 <= tim2) ? tim1 : tim2;
							auto maxtim = (mintim == tim1) ? tim2 : tim1;
							std::uniform_int_distribution<long long int> timeDistr(mintim, maxtim);
							auto mytim = timeDistr(gen);
							if(coin(gen))
							{
								//toAdd.push_back(std::vector<long long int>{i, node2, mytim});
								#pragma omp critical
								fileout << i << " " << node2 << " " << mytim << '\n';
							}
							else
							{
								#pragma omp critical
								fileout << node2 << " " << i << " " << mytim << '\n';
							}
								//toAdd.push_back(std::vector<long long int>{node2, i, mytim});
						}
					}
				}
			}
		}

		fileout.close();
		return 0;
	}
	else if(type == "2")
	{
		std::vector<std::vector<long long int>> edges;
		std::unordered_map<long long int, long long int> remapID;
		long long int ID = 0;

		std::string filename{argv[1]};
		long long int nodes = parseDataFile(filename, std::ref(edges));

		std::string filename2{argv[3]};
		long long int nodes1 = parseDataFile(filename2, std::ref(edges));

		std::sort(edges.begin(), edges.end(), [](auto& left, auto& right){return left[2] <= right[2];});
		std::cout << "edges to be added: " << edges.size() << '\n';

		std::string fout = "EquinixChicago-new-sorted.txt";
		ofstream fileout;
		fileout.open(fout);

		for(auto& edge : edges)
		{
			auto src = edge[0];
			auto dst = edge[1];
			if(remapID.find(src) == remapID.end())
			{
				remapID[src] = ID++;
			}
			if(remapID.find(dst) == remapID.end())
			{
				remapID[dst] = ID++;
			}
			fileout << remapID[src] << " " << remapID[dst] << " " << edge[2] << '\n';
		}
		fileout.close();
		return 0;
		/*
		for(auto& e : toAdd)
		{
			std::cout << "src; " << e[0] << " dst: " << e[1] << " tim: " << e[2] << '\n';	
		}
		*/
		//	std::cout << edge[0] << " " << edge[1] << " " << edge[2] << '\n';
	}
	else
		std::cout << "Unknown parameters\n";
}
