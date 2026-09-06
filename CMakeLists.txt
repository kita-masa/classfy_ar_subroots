cmake_minimum_required(VERSION 3.20)
project(root-computation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(root-computation
    src/vector.cpp
    src/root.cpp
    src/diagram.cpp
    src/root-system.cpp
	src/root-data.cpp
	src/symmetric-pair.cpp
	src/lin_alg.cpp
	src/embedding.cpp
)

add_library(visualize
	src/visualize.cpp
)

target_link_libraries(visualize PUBLIC
	root-computation
)

target_include_directories(root-computation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(visualize PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(search_embeddings
	apps/search_embeddings.cpp
)

add_executable(convert_to_dot
	apps/convert_to_dot.cpp
)

add_executable(embedding_in_associated_dot
	apps/embedding_in_associated_dot.cpp
)

target_link_libraries(search_embeddings PRIVATE root-computation)
target_link_libraries(convert_to_dot PRIVATE root-computation visualize)
target_link_libraries(embedding_in_associated_dot PRIVATE root-computation visualize)