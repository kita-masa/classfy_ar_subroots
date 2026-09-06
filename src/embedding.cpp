
#include <cassert>

#include "my-root/embedding.h"
#include "my-root/root-data.h"

using namespace std;

void serialize_embedding(const vector<root>& embedding, const decorated_root_system& system, const string diagram_identifier, ostream& out) {
	out << diagram_identifier << endl;
	out << embedding.size() << endl;
	for (const auto& r : embedding) {
		out << r.coordinates() << endl;
	}
}

tuple<string, decorated_root_system, vector<root>> deserialize_embedding(istream& in) {
	string diagram_identifier;
	in >> diagram_identifier;

	auto diagram = vogan_diagram_from_string(diagram_identifier);
	decorated_root_system system(root_system::generated_by(diagram.simple_roots()), diagram.simple_roots(), diagram.marking());
	size_t dim = diagram.rank();

	size_t embedding_size;
	in >> embedding_size;

	vector<root> embedding;
	for (size_t i = 0; i < embedding_size; ++i) {
		math_vector coords(dim);
		in >> coords;
		embedding.push_back(system.base()[coords]);
	}

	return { diagram_identifier, system, embedding };
}

vector<root> find_standard_simple_roots_su(const decorated_root_system& system) {
	set<root> compact_roots = system.all_roots(root_mark::imaginary_compact);
	if (compact_roots.size() == 0) {
		return vector<root>{ *system.all_roots(root_mark::imaginary_noncompact).begin() };
	}
	auto compact_system = system.subroot_system(vector<root>(compact_roots.begin(), compact_roots.end()));
	vector<root> compact_simples = compact_system.all_simple_roots();
	root noncompact_simple;
	for (const auto& r : system.all_roots(root_mark::imaginary_noncompact)) {
		bool ok = true;
		int count = 0;
		for (const auto& s : compact_simples) {
			int v = r.pairing(s);
			if (v > 0) {
				ok = false;
				break;
			}
			if (v < 0) {
				count++;
			}
		}
		if (ok && count == 1) {
			noncompact_simple = r;
			break;
		}
	}
	vector<root> result;
	set<root> used;
	result.push_back(noncompact_simple);
	root prev = noncompact_simple;
	while (used.size() < compact_simples.size()) {
		for (const auto& s : compact_simples) {
			if (prev.pairing(s) != 0 && used.insert(s).second) {
				result.push_back(s);
				prev = s;
				break;
			}
		}
	}
	return result;
}

embedding_root_helper::embedding_root_helper(const decorated_root_system& system, const vector<root>& embedded_simple_roots)
	: system(system), embedded_simple_roots(embedded_simple_roots) {
}

void embedding_root_helper::fix_lowest_root(const root& r) {
	lowest_root = r;
}

bool embedding_root_helper::can_add(const root& r, const vector<root>& test_roots) const {
	for (const auto& s : test_roots) {
		if (r == s || r == -s || system.base().contains(r.coordinates() - s.coordinates())) {
			return false;
		}
	}
	if (lowest_root.valid() && (system.base().contains(lowest_root.coordinates() - r.coordinates()) || r == lowest_root)) {
		return false;
	}
	return true;
}

void embedding_root_helper::dfs(vector<root>& additional_roots, int pos, vector<embedding_root_data>& result) {
	if (additional_roots.size() + embedded_simple_roots.size() == system.rank()) {
		vector<root> new_simple_roots = embedded_simple_roots;
		new_simple_roots.insert(new_simple_roots.end(), additional_roots.begin(), additional_roots.end());
		if (system.base().is_simple_roots(new_simple_roots)) {
			result.push_back({ system.change_positivity(new_simple_roots), additional_roots });
		}
		return;
	}
	if (pos == candidates.size()) return;
	for (int i = pos; i < candidates.size(); ++i) {
		if (can_add(candidates[i], additional_roots)) {
			additional_roots.push_back(candidates[i]);
			dfs(additional_roots, i + 1, result);
			additional_roots.pop_back();
		}
	}
}

vector<embedding_root_data> embedding_root_helper::search() {
	candidates.clear();
	for (const auto& r : system.roots()) {
		if (can_add(r, embedded_simple_roots)) {
			candidates.push_back(r);
		}
	}
	vector<embedding_root_data> result;
	vector<root> additional_roots;
	dfs(additional_roots, 0, result);
	return result;
}

root remove_edge_node_A(const vector<root>& simple_roots, const decorated_root_system& system, bool remove_noncompact = false) {
	for (const auto& r : simple_roots) {
		if (remove_noncompact) {
			if (system.mark_of(r) == root_mark::imaginary_noncompact) {
				return r;
			} else {
				continue;
			}
		}
		int count = 0;
		for (const auto& s : simple_roots) {
			if (r == s) continue;
			if (r.pairing(s) != 0) {
				count++;
			}
		}
		if (count == 1 && system.mark_of(r) == root_mark::imaginary_compact) {
			return r;
		}
	}
	assert(false);
}

vector<root> add_node_candidate(const decorated_root_system& system, const vector<root>& simples, root removed, bool remove_noncompact = false) {
	vector<root> result;
	set<vector<int>> used;
	for (const auto& r : system.roots()) {
		bool ok = true;
		vector<int> pairing_data;
		for (const auto& s : simples) {
			if (s == r || s == -r || system.base().contains(r.coordinates() - s.coordinates())) {
				ok = false;
				break;
			}
			pairing_data.push_back(r.pairing(s));
			pairing_data.push_back(s.pairing(r));
		}
		if (removed.valid() && (system.base().contains(removed.coordinates() - r.coordinates())) || r == removed) {
			ok = false;
		}
		ok = ok && ((!remove_noncompact && system.mark_of(r) == root_mark::imaginary_compact)
			|| (remove_noncompact && system.mark_of(r) == root_mark::imaginary_noncompact));
		if (ok) {
			vector<root> check = simples;
			check.push_back(r);
			if (system.base().is_simple_roots(check) && used.insert(pairing_data).second) {
				result.push_back(r);
			}
		}
	}
	return result;
}

vector<decorated_root_system> standard_positive_system_for_embedding(const vector<root>& embedding, const decorated_root_system& system, bool remove_noncompact) {
	auto embedding_system = system.subroot_system(embedding);
	auto commutator = system.commutator(embedding);

	auto simples = find_standard_simple_roots_su(embedding_system);
	auto add_simples = commutator.all_simple_roots();
	root removed;

	if (simples.size() + add_simples.size() == system.rank()) {
		removed = remove_edge_node_A(simples, system, remove_noncompact);
		simples.erase(std::remove(simples.begin(), simples.end(), removed), simples.end());
	}
	simples.insert(simples.end(), add_simples.begin(), add_simples.end());

	auto add_candidates = add_node_candidate(system, simples, removed, remove_noncompact && removed.valid());
	assert(add_candidates.size() > 0);
	cout << "Candidates for adding a node: " << add_candidates.size() << endl;

	vector<decorated_root_system> result;
	for (const auto& candidate : add_candidates) {
		vector<root> new_simple_roots = simples;
		new_simple_roots.push_back(candidate);
		result.push_back(system.change_positivity(new_simple_roots));
	}
	return result;
}