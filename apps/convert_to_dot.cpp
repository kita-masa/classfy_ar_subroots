#include <iostream>
#include <cassert>
#include <functional>
#include <queue>
#include <set>
#include <vector>
#include <random>
#include <filesystem>
#include <fstream>

#include "my-root/diagram.h"
#include "my-root/root-data.h"
#include "my-root/root-system.h"
#include "my-root/symmetric-pair.h"
#include "my-root/visualize.h"
#include "my-root/lin_alg.h"
#include "my-root/embedding.h"

using namespace std;

tuple<vector<embedding_root_data>, map<math_vector, int>> find_all_embedding_roots(vector<root> embedding, const decorated_root_system& system) {
	vector<embedding_root_data> result;
	map<math_vector, int> root_to_index;
	auto embedding_system = system.subroot_system(embedding);
	auto commutator = system.commutator(embedding);

	auto simples = find_standard_simple_roots_su(embedding_system);
	auto add_simples = commutator.all_simple_roots();

	for (size_t i = 0; i < simples.size(); i++) {
		root_to_index[simples[i].coordinates()] = i;
	}
	for (size_t i = 0; i < add_simples.size(); i++) {
		root_to_index[add_simples[i].coordinates()] = i;
	}

	vector<root> removed;
	for (const auto& r : simples) {
		int count = 0;
		for (const auto& s : simples) {
			if (r == s) continue;
			if (r.pairing(s) != 0) {
				count++;
			}
		}
		if (count == 1 && system.mark_of(r) == root_mark::imaginary_noncompact) {
			removed.push_back(r);
		}
	}
	simples.insert(simples.end(), add_simples.begin(), add_simples.end());
	for (const auto& r : removed) {
		auto base_roots = simples;
		base_roots.erase(std::remove(base_roots.begin(), base_roots.end(), r), base_roots.end());
		embedding_root_helper helper(system, base_roots);
		helper.fix_lowest_root(r);
		auto add = helper.search();
		result.insert(result.end(), add.begin(), add.end());
	}
	return { result, root_to_index };
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " <embedding_txt> <output_dot_prefix>" << endl;
		return 1;
	}
	filesystem::path input_file(argv[1]);
	if (!filesystem::is_regular_file(input_file)) {
		cerr << "Error: " << input_file << " is not a file" << endl;
		return 1;
	}

	ifstream in(input_file);
	auto [ identifier, system, embedding ] = deserialize_embedding(in);
	auto [ data, root_to_index ] = find_all_embedding_roots(embedding, system);

	int count = 0;
	for (const auto& good_positive_system_data : data) {
		auto good_positive_system = good_positive_system_data.new_system;
		auto embedding_system = good_positive_system.subroot_system(embedding);
		auto commutator = good_positive_system.commutator(embedding);
		
		auto data = vogan_diagram_as_dot(good_positive_system, true);
		for (const auto& r : data.all_coordinates()) {
			if (embedding_system.base().contains(r) && root_to_index.contains(r)) {
				data.add_node_label(r, "su_" + to_string(root_to_index[r]));
			}
			if (commutator.base().contains(r) && root_to_index.contains(r)) {
				data.add_node_label(r, "Z(su)_" + to_string(root_to_index[r]));
			}
			if (r == -good_positive_system.highest_root().coordinates()) {
				data.add_node_label(r, "lowest root");
			}
		}
		data.add_label("Rank: " + to_string(embedding_system.rank()));
		data.add_label("Commutator Rank: " + to_string(commutator.rank()));
		data.save_to_file(string(argv[2]) + "_" + to_string(count) + ".dot");
		count++;
	}
	return 0;
}