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
	bool remove_noncompact = false;
	if (argc == 4) {
		if (string(argv[3]) == "--remove-noncompact") {
			remove_noncompact = true;
		} else {
			cerr << "Unknown option: " << argv[3] << endl;
			return 1;
		}
	}

	ifstream in(input_file);
	auto [ identifier, system, embedding ] = deserialize_embedding(in);

	auto su_system = system.subroot_system(embedding);
	auto commutator = system.commutator(embedding);
	auto normalizer = system.subroot_system({ su_system, commutator });
	map<math_vector, int> root_to_index;
	auto simples = find_standard_simple_roots_su(su_system);
	auto add_simples = commutator.all_simple_roots();

	root highest_root;
	for (const auto& r : simples) {
		if (system.mark_of(r) == root_mark::imaginary_noncompact) {
			highest_root = r;
			break;
		}
	}
	for (size_t i = 0; i < simples.size(); i++) {
		root_to_index[simples[i].coordinates()] = i;
		if (simples[i] != highest_root && simples[i].pairing(highest_root) != 0) {
			root_to_index[simples[i].reflect_by(highest_root).coordinates()] = simples.size();
		}
	}
	for (size_t i = 0; i < add_simples.size(); i++) {
		root_to_index[add_simples[i].coordinates()] = i;
	}
	simples.erase(std::remove(simples.begin(), simples.end(), highest_root), simples.end());
	for (size_t i = 0; i < simples.size(); i++) {
		simples[i] = simples[i].reflect_by(highest_root);
	}
	simples.insert(simples.end(), add_simples.begin(), add_simples.end());


	auto symmetric_subsystem_by_grading = symmetric_pair_from_grading(system, highest_root);
	auto associated_subsystem = associated_pair(system, vector<root>(symmetric_subsystem_by_grading.roots().begin(), symmetric_subsystem_by_grading.roots().end()));

	embedding_root_helper helper(associated_subsystem, simples);
	auto candidates = helper.search();

	int min_noncompact = 100;
	for (const auto& good_embedding : candidates) {
		int noncompact_count = 0;
		for (const auto& r : good_embedding.additional_roots) {
			if (system.mark_of(r) == root_mark::imaginary_noncompact) {
				noncompact_count++;
			}
		}
		min_noncompact = min(min_noncompact, noncompact_count);
	}

	int count = 0;
	for (const auto& good_embedding : candidates) {
		auto good_positive_system = good_embedding.new_system;
		int noncompact_count = 0;
		for (const auto& r : good_embedding.additional_roots) {
			if (system.mark_of(r) == root_mark::imaginary_noncompact) {
				noncompact_count++;
			}
		}
		if (noncompact_count > min_noncompact) continue;
		auto data = vogan_diagram_as_dot(good_positive_system, true);
		auto highests = good_positive_system.highest_root_non_irreducible();
		auto highests_set = set<root>(highests.begin(), highests.end());
		for (const auto& r : data.all_coordinates()) {
			if (su_system.base().contains(r) && root_to_index.contains(r)) {
				data.add_node_label(r, "su_" + to_string(root_to_index[r]));
			}
			if (commutator.base().contains(r) && root_to_index.contains(r)) {
				data.add_node_label(r, "Z(su)_" + to_string(root_to_index[r]));
			}
			if (highests_set.find(system.base()[-r]) != highests_set.end()) {
				data.add_node_label(r, "lowest root");
			}
		}
		data.add_label("Rank: " + to_string(su_system.rank()));
		data.add_label("Commutator Rank: " + to_string(commutator.rank()));
		data.save_to_file(string(argv[2]) + "_" + to_string(count) + ".dot");
		count++;
	}
	cout << "Found " << count << " candidates for embedding:" << endl;
	return 0;
}