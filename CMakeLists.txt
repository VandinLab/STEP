cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3")
set(UNORDERED_DENSE_LIB "${CMAKE_CURRENT_SOURCE_DIR}/unordered_dense/build/unordered_dense_install")
list(APPEND CMAKE_PREFIX_PATH "${UNORDERED_DENSE_LIB}")

project(STEP)

find_package(unordered_dense CONFIG REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(STEP src/STEP.cpp
		include/Edge.h src/Edge.cpp
		include/Subgraph.h src/Subgraph.cpp
		include/EdgeFinder.h src/EdgeFinder.cpp
		include/EdgeMap.h src/EdgeMap.cpp
		include/utils.h src/utils.cpp
		include/algorithms.h src/algorithms.cpp)

add_executable(build_predictor src/build_predictor.cpp
		include/Edge.h src/Edge.cpp
		include/Subgraph.h src/Subgraph.cpp
		include/EdgeFinder.h src/EdgeFinder.cpp
		include/EdgeMap.h src/EdgeMap.cpp
		include/utils.h src/utils.cpp
		include/algorithms.h src/algorithms.cpp)

add_executable(dataset_preprocessing src/dataset_preprocessing.cpp
		include/Edge.h src/Edge.cpp
		include/EdgeMap.h src/EdgeMap.cpp
		include/utils.h src/utils.cpp)

add_executable(STEP_online src/STEP_online.cpp
		include/Edge.h src/Edge.cpp
		include/Subgraph.h src/Subgraph.cpp
		include/EdgeFinder.h src/EdgeFinder.cpp
		include/EdgeMap.h src/EdgeMap.cpp
		include/utils.h src/utils.cpp
		include/algorithms.h src/algorithms.cpp)

target_link_libraries(STEP unordered_dense::unordered_dense)
target_link_libraries(build_predictor unordered_dense::unordered_dense)
target_link_libraries(dataset_preprocessing unordered_dense::unordered_dense)
target_link_libraries(STEP_online unordered_dense::unordered_dense)

