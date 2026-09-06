

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

struct vertex {
	vector<vertex*> negative_edges;
	set<vertex*> nonzero_edges;
	const root r;
	vertex(const root& r) : r(r) {}
};

class su_embedding_helper {
protected:
	const decorated_root_system& system;
	root highest_root;
	vector<vertex> graph;

	void make_graph() {
		graph.clear();
		graph.push_back(vertex(-highest_root));
		// collect all long compact roots
		for (const auto& r : system.base().orbit(highest_root)) {
			if (system.mark_of(r) == root_mark::imaginary_compact) {
				graph.push_back(vertex(r));
			}
		}
		for (size_t i = 0; i < graph.size(); ++i) {
			for (size_t j = i + 1; j < graph.size(); ++j) {
				auto pairing = graph[i].r.pairing(graph[j].r);
				if (pairing < 0) {
					graph[i].negative_edges.push_back(&graph[j]);
					graph[j].negative_edges.push_back(&graph[i]);
				}
				if (pairing != 0) {
					graph[i].nonzero_edges.insert(&graph[j]);
					graph[j].nonzero_edges.insert(&graph[i]);
				}
			}
		}
	}

	pair<vector<root>, bool> verify(const vector<vertex*>& candidates) const {
		vector<root> candidate_roots;
		for (const auto& v : candidates) {
			candidate_roots.push_back(v->r);
		}
		auto commutator = system.commutator(candidate_roots);
		if (commutator.all_roots(root_mark::imaginary_noncompact).size() > 0) {
			return { vector<root>(), false };
		}
		return { candidate_roots, true };
	}
	void dfs(vertex* v, vector<vertex*>& current, vector<vector<root>>& result) const {
		if (current.size() > 1) {
			auto [simples, ok] = verify(current);
			if (ok) {
				result.push_back(simples);
			}
		}
		if (current.size() >= system.rank()) {
			return;
		}
		for (auto next : v->negative_edges) {
			bool ok = true;
			for (size_t i = 0; i + 1 < current.size(); ++i) {
				if (next == current[i] || next->nonzero_edges.find(current[i]) != next->nonzero_edges.end()) {
					ok = false;
					break;
				}
			}
			if (ok) {
				current.push_back(next);
				dfs(next, current, result);
				current.pop_back();
			}
		}
		return;
	}
	vector<vector<root>> unique(vector<vector<root>>& embeddings) {
		set<vector<root>> used;
		vector<vector<root>> result;

		vector<root> compact_roots;
		for (const auto& r : system.all_roots(root_mark::imaginary_compact)) {
			if (r.pairing(highest_root) == 0) {
				compact_roots.push_back(r);
			}
		}

		for (const auto& embedding : embeddings) {
			if (used.insert(embedding).second) {
				result.push_back(embedding);
			} else continue;
			queue<vector<root>> que;
			que.push(embedding);
			while (!que.empty()) {
				auto current = que.front();
				que.pop();
				for (const auto& r : compact_roots) {
					vector<root> reflected;
					for (const auto& s : current) {
						reflected.push_back(s.reflect_by(r));
					}
					if (used.insert(reflected).second) {
						que.push(reflected);
					}
				}
			}
		}
		return result;
	}
public:
	su_embedding_helper(const decorated_root_system& system, const root& highest) : system(system), highest_root(highest) {
		make_graph();
	}

	vector<vector<root>> find_good_su_embeddings() {
		vector<vertex*> current = { &graph[0] };
		vector<vector<root>> result;
		dfs(&graph[0], current, result);
		cout << result.size() << endl;
		return unique(result);
	}
};




int main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " <vogan_diagram_identifier> <output_directory>" << endl;
		return 1;
	}
	filesystem::path output_dir(argv[2]);
	if (!filesystem::is_directory(output_dir)) {
		cerr << "Error: " << output_dir << " is not a directory" << endl;
		return 1;
	}
	string diagram_identifier = argv[1];
	if (!test_vogan_diagram_identifier(diagram_identifier)) {
		cerr << "Error: " << diagram_identifier << " is not a valid vogan diagram identifier" << endl;
		return 1;
	}

	auto diagram = vogan_diagram_from_string(diagram_identifier);
	decorated_root_system system(root_system::generated_by(diagram.simple_roots()), diagram.simple_roots(), diagram.marking());
	auto extremal = system.base().extremal_roots();
	root noncompact_extremal;
	for (const auto& r : extremal) {
		if (system.mark_of(r) == root_mark::imaginary_noncompact) {
			noncompact_extremal = r;
			break;
		}
	}
	if (!noncompact_extremal.valid()) {
		cout << "No noncompact extremal root found." << endl;
		return 0;
	}
	su_embedding_helper helper(system, noncompact_extremal);
	auto embeddings = helper.find_good_su_embeddings();
	cout << embeddings.size() << " su(1, r) embeddings found." << endl;
	int count = 0;
	for (const auto& embedding : embeddings) {
		string filename = (output_dir / (to_string(count) + ".txt")).string();
		ofstream out(filename);
		serialize_embedding(embedding, system, diagram_identifier, out);
		++count;
	}
	return 0;
}