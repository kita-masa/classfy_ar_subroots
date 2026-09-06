#include "my-root/root-system.h"

#include <queue>
#include <cassert>
#include <stdexcept>

#include "my-root/root-system.h"

std::vector<root> simple_roots_from_positives(const std::vector<root>& positive_roots) {
	std::set<math_vector> sums;
	for (const auto& r1 : positive_roots) {
		for (const auto& r2 : positive_roots) {
			sums.insert(r1.coordinates() + r2.coordinates());
		}
	}
	std::vector<root> simple_roots;
	for (const auto& r : positive_roots) {
		if (sums.find(r.coordinates()) == sums.end()) {
			simple_roots.push_back(r);
		}
	}
	return simple_roots;
}


root_system::root_system(const std::vector<root>& roots) {
	for (const auto& r : roots) {
		_roots.insert({ r.coordinates(), r });
	}
}

size_t root_system::size() const {
	return _roots.size();
}

std::set<root> root_system::all_roots() const {
	auto r = roots();
	return std::set<root>(r.begin(), r.end());
}

std::vector<root> root_system::roots_orthogonal_to(const std::vector<root>& rs) const {
	std::vector<root> result;
	for (const auto& [_, r] : _roots) {
		bool ok = true;
		for (const auto& s : rs) {
			if (r.pairing(s) != 0) {
				ok = false;
				break;
			}
		}
		if (ok) {
			result.push_back(r);
		}
	}
	return result;
}

std::vector<root> root_system::orbit(const root& r) const {
	std::set<root> used;
	std::queue<root> que;
	que.push(r);
	used.insert(r);
	while (!que.empty()) {
		root current = que.front();
		que.pop();
		for (const auto& [_, other] : _roots) {
			auto next = current.reflect_by(other);
			if (used.insert(next).second) {
				que.push(next);
			}
		}
	}
	return std::vector<root>(used.begin(), used.end());
}

std::vector<root> root_system::orbits_by_orthogonals(const std::vector<root>& rs) const {
	std::set<root> used;
	std::vector<root> result;
	for (const auto& [_, r] : _roots) {
		if (used.find(r) != used.end()) continue;
		result.push_back(r);
		std::queue<root> que;
		que.push(r);
		used.insert(r);
		while (!que.empty()) {
			root current = que.front();
			que.pop();
			for (const auto& other : rs) {
				auto next = current.reflect_by(other);
				if (used.insert(next).second) {
					que.push(next);
				}
			}
		}
	}
	return result;
}

std::vector<root> root_system::extremal_roots() const {
	root max_root = _roots.begin()->second;
	for (const auto& r : roots()) {
		if (r.coordinates().as_vector() > max_root.coordinates().as_vector()) {
			max_root = r;
		}
	}
	return orbit(max_root);
}

bool root_system::is_simple_roots(const std::vector<root> &simple_roots) const {
	for (const auto& r : simple_roots) {
		for (const auto& s : simple_roots) {
			if (r == s) continue;
			if (contains(r.coordinates() - s.coordinates())) {
				return false;
			}
		}
	}

	std::set<root> used, simples(simple_roots.begin(), simple_roots.end());
	std::queue<root> que;
	for (const auto& gen : simple_roots) {
		que.push(gen);
		used.insert(gen);
	}
	while (!que.empty()) {
		root current = que.front();
		que.pop();
		for (const auto& other: simple_roots) {
			if (current.pairing(other) >= 0) continue;
			auto next = current.reflect_by(other);
			if (simples.find(next) != simples.end()) {
				return false;
			}
			if (used.insert(next).second) {
				que.push(next);
			}
		}
	}
	if (used.size() != _roots.size() / 2) return false;
	for (const auto& r : used) {
		if (used.find(-r) != used.end()) {
			return false;
		}
	}
	return true;
}

root_system root_system::subroot_system(const std::vector<root>& generators) const {
	std::set<root> used;
	std::queue<root> que;
	for (const auto& gen : generators) {
		que.push(gen);
		used.insert(gen);
		if (used.insert(-gen).second) {
			que.push(-gen);
		}
	}
	while (!que.empty()) {
		root current = que.front();
		que.pop();
		for (const auto& other: generators) {
			for (int i = -1; i <= 1; i += 2) {
				auto next = current.coordinates() + i * other.coordinates();
				auto next_root_it = _roots.find(next);
				if (next_root_it != _roots.end() && used.insert(next_root_it->second).second) {
					que.push(next_root_it->second);
				}
			}
		}
	}
	return root_system(std::vector<root>(used.begin(), used.end()));
}

std::vector<root> root_system::maximal_strongly_orthogonal_subset(const std::vector<root>& rs) const {
	std::vector<root> result;
	for (const auto& r : rs) {
		bool ok = true;
		for (const auto& s : result) {
			if (!is_strongly_orthogonal(r, s)) {
				ok = false;
				break;
			}
		}
		if (ok) {
			result.push_back(r);
		}
	}
	return result;
}

root root_system::operator[](const math_vector& coords) const {
	return _roots.at(coords);
}

bool root_system::contains(const math_vector &coords) const {
	return _roots.find(coords) != _roots.end();
}

bool root_system::is_strongly_orthogonal(const root& r1, const root& r2) const {
	auto sum = r1.coordinates() + r2.coordinates();
	auto diff = r1.coordinates() - r2.coordinates();
	if (r1.pairing(r2) != 0) return false;
	return _roots.find(sum) == _roots.end() && _roots.find(diff) == _roots.end();
}

root_system root_system::commutator(const std::vector<root>& rs) const {
	std::vector<root> result;
	for (const auto& [_, r] : _roots) {
		root current = r;
		bool ok = true;
		for (const auto& s : rs) {
			if (!is_strongly_orthogonal(current, s)) {
				ok = false;
				break;
			}
		}
		if (ok) {
			result.push_back(current);
		}
	}
	return root_system(result);
}

root_system root_system::intersection(const root_system& other) const {
	std::vector<root> result;
	for (const auto& [_, r] : _roots) {
		if (other.contains(r.coordinates())) {
			result.push_back(r);
		}
	}
	return root_system(result);
}

decorated_root_system::decorated_root_system(root_system system, std::vector<root> simple_roots) : base_system(system), simple_roots(simple_roots) {
	std::vector<root> rebased_simple_roots;
	for (size_t i = 0; i < simple_roots.size(); ++i) {
		math_vector new_coords(simple_roots.size());
		math_vector coroot(simple_roots.size());
		new_coords[i] = 1;
		for (size_t j = 0; j < simple_roots.size(); ++j) {
			coroot[j] = simple_roots[j].pairing(simple_roots[i]);
		}
		rebased_simple_roots.push_back(root(new_coords, coroot));
	}
	auto rebased_system = root_system::generated_by(rebased_simple_roots);
	for (const auto& r : rebased_system.roots()) {
		math_vector old_coords(simple_roots[0].coordinates().dimension());
		for (size_t i = 0; i < simple_roots.size(); ++i) {
			old_coords = old_coords + r.coordinates()[i] * simple_roots[i].coordinates();
		}
		rebased_roots.emplace(base_system[old_coords], r);
	}
}

decorated_root_system::decorated_root_system(root_system system, std::vector<root> simple_roots, marking_data marking) : decorated_root_system(system, simple_roots) {
	set_marking(marking);
}

// for Vogan diagrams
void decorated_root_system::generate_marks() {
	if (!marking.has_value()) {
		throw std::logic_error("Marking is not set");
	}
	if (base_system.size() == 0) return;
	std::vector<std::pair<int, root>> imaginary_roots;
	math_vector dir(std::vector<ring>(simple_roots[0].coordinates().dimension(), 1));
	for (size_t i = 0; i < simple_roots.size(); ++i) {
		marking->marks[-simple_roots[i].coordinates()] = marking->marks[simple_roots[i].coordinates()];
	}
	for (const auto& r : base_system.roots()) {
		if (marking->marks.find(r.coordinates()) != marking->marks.end()) continue;
		root involuted = involution_of(r);
		if (involuted == r) {
			int level = rebased_roots.at(r).coordinates() * dir;
			if (level > 0) imaginary_roots.emplace_back(level, r);
		} else if (involuted.coordinates() == -r.coordinates()) {
			marking->marks[r.coordinates()] = root_mark::real;
		} else {
			marking->marks[r.coordinates()] = root_mark::complex;
		}
	}
	std::sort(imaginary_roots.begin(), imaginary_roots.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
	});
	for (const auto& [level, r] : imaginary_roots) {
		auto rebased = rebased_roots.at(r).coordinates();
		for (size_t i = 0; i < simple_roots.size(); ++i) {
			if (rebased[i] == 0) continue;
			auto mark = marking->marks[simple_roots[i].coordinates()];
			auto reduction = r.coordinates() - simple_roots[i].coordinates();
			if (!base_system.contains(reduction)) continue;

			if (root_mark_func::is_imaginary(mark) && base_system.contains(reduction)) {
				if (mark == root_mark::imaginary_compact) {
					marking->marks[r.coordinates()] = marking->marks[reduction];
				} else {
					marking->marks[r.coordinates()] = root_mark_func::reverse_imaginary_root(marking->marks[reduction]);
				}
				break;
			}
			if (mark == root_mark::complex) {
				reduction = reduction - marking->involuted_roots[i];
				if (base_system.contains(reduction)) {
					if (simple_roots[i].pairing(base_system[marking->involuted_roots[i]]) == 0) {
						marking->marks[r.coordinates()] = marking->marks[reduction];
					} else {
						marking->marks[r.coordinates()] = root_mark_func::reverse_imaginary_root(marking->marks[reduction]);
					}
					break;
				} else if (reduction.zero()) {
					marking->marks[r.coordinates()] = root_mark::imaginary_noncompact;
					break;
				}
			}
		}
		assert(marking->marks.find(r.coordinates()) != marking->marks.end());
		marking->marks[-r.coordinates()] = marking->marks[r.coordinates()];
	}
	assert(marking->marks.size() == base_system.roots().size());
}

const root_system& decorated_root_system::base() const {
	return base_system;
}

size_t decorated_root_system::rank() const {
	return simple_roots.size();
}

size_t decorated_root_system::real_rank() const {
	decorated_root_system system = *this;
	int count = 0;
	while (system.all_roots(root_mark::imaginary_noncompact).size() > 0) {
		auto r = *system.all_roots(root_mark::imaginary_noncompact).begin();
		system = system.cayley_transform_imaginary(r);
		count++;
	}
	return count;
}

void decorated_root_system::set_marking(const marking_data& data) {
	marking = data;
	generate_marks();
}

root decorated_root_system::involution_of(const root& r) const {
	if (!marking.has_value()) {
		throw std::logic_error("Marking is not set");
	}
	math_vector new_coords(r.coordinates().dimension());
	root rebased = rebased_roots.at(r);
	for (size_t i = 0; i < simple_roots.size(); ++i) {
		new_coords = new_coords + rebased.coordinates()[i] * marking->involuted_roots[i];
	}
	return base_system[new_coords];
}

root decorated_root_system::highest_root() const {
	root max_root = base_system.roots().front();
	math_vector max_coords = rebased_roots.at(max_root).coordinates();
	for (const auto& r : base_system.roots()) {
		auto rebased_coords = rebased_roots.at(r).coordinates();
		if (max_coords < rebased_coords) {
			max_root = r;
			max_coords = rebased_coords;
		}
	}
	// int count = 0;
	// for (const auto& s : roots()) {
	// 	bool ok = false;
	// 	for (const auto& r : simple_roots) {
	// 		ok = ok || r == s || r == -s;
	// 		ok = ok || base().contains(s.coordinates() + r.coordinates());
	// 	}
	// 	if (!ok) count++;
	// }
	// assert(count == 1);
	return max_root;
}

std::vector<root> decorated_root_system::highest_root_non_irreducible() const {
	std::vector<root> result;
	for (const auto& s : roots()) {
		bool ok = false;
		for (const auto& r : simple_roots) {
			ok = ok || r == s || r == -s;
			ok = ok || base().contains(s.coordinates() + r.coordinates());
		}
		if (!ok) {
			result.push_back(s);
		}
	}
	return result;
}

root_mark decorated_root_system::mark_of(const root& r) const {
	if (!marking.has_value()) {
		throw std::logic_error("Marking is not set");
	}
	return marking->marks.at(r.coordinates());
}

bool decorated_root_system::is_positive(const root& r) const {
	root rebased = rebased_roots.at(r);
	for (auto coord : rebased.coordinates().as_vector()) {
		if (coord != 0) {
			return coord > 0;
		}
	}
	return false;
}

bool decorated_root_system::is_subroot_system(const std::vector<root>& rs) const {
	auto sub_system = base_system.subroot_system(rs);
	return sub_system.size() == rs.size();
}

std::set<root> decorated_root_system::all_roots(root_mark mark) const {
	std::set<root> result;
	for (const auto& r : base_system.roots()) {
		if (mark_of(r) == mark) {
			result.insert(r);
		}
	}
	return result;
}

std::vector<std::tuple<root, root_mark, math_vector>> decorated_root_system::all_roots_with_marks() const {
	std::vector<std::tuple<root, root_mark, math_vector>> result;
	for (const auto& r : base_system.roots()) {
		result.emplace_back(r, mark_of(r), involution_of(r).coordinates());
	}
	return result;
}

std::vector<root> decorated_root_system::simple_roots_with_mark(root_mark mark) const {
	std::vector<root> result;
	for (const auto& r : simple_roots) {
		if (mark_of(r) == mark) {
			result.push_back(r);
		}
	}
	return result;
}

std::vector<root> decorated_root_system::all_simple_roots() const {
	return simple_roots;
}

decorated_root_system decorated_root_system::cayley_transform_imaginary(const root& r) const {
	if (mark_of(r) != root_mark::imaginary_noncompact) {
		throw std::invalid_argument("Root is not imaginary noncompact");
	}
	if (!marking.has_value()) {
		throw std::logic_error("Marking is not set");
	}
	marking_data new_marking;
	new_marking.involuted_roots = marking->involuted_roots;
	
	for (size_t i = 0; i < simple_roots.size(); ++i) {
		new_marking.involuted_roots[i] = base_system[marking->involuted_roots[i]].reflect_by(r).coordinates();
	}
	for (auto& [root_coords, mark] : marking->marks) {
		auto involuted = involution_of(base_system[root_coords]).reflect_by(r).coordinates();
		if (involuted == -root_coords) {
			new_marking.marks[root_coords] = root_mark::real;
		} else if (involuted != root_coords) {
			new_marking.marks[root_coords] = root_mark::complex;
		} else if (base_system.contains(root_coords + r.coordinates())) {
			new_marking.marks[root_coords] = root_mark_func::reverse_imaginary_root(mark);
		} else {
			new_marking.marks[root_coords] = mark;
		}
	}
	return decorated_root_system(base_system, simple_roots, new_marking);
}

decorated_root_system decorated_root_system::subroot_system(const std::vector<root>& generators) const {
	root_system new_system = base_system.subroot_system(generators);
	std::vector<root> positive_roots;
	for (const auto& r : new_system.roots()) {
		if (is_positive(r)) {
			positive_roots.push_back(r);
		}
	}
	std::vector<root> simple_roots = simple_roots_from_positives(positive_roots);
	if (marking.has_value()) {
		marking_data new_marking;
		for (const auto& r : new_system.roots()) {
			new_marking.marks[r.coordinates()] = mark_of(r);
		}
		for (const auto& r : simple_roots) {
			auto involuted = involution_of(r);
			assert(new_system.contains(involuted.coordinates()));
			new_marking.involuted_roots.push_back(involuted.coordinates());
		}
		return decorated_root_system(new_system, simple_roots, new_marking);
	}
	return decorated_root_system(new_system, simple_roots);
}

decorated_root_system decorated_root_system::subroot_system(std::initializer_list<std::reference_wrapper<const decorated_root_system>> systems) const {
	std::set<root> generators;
	for (const auto& system : systems) {
		auto roots = system.get().roots();
		generators.insert(roots.begin(), roots.end());
	}
	return subroot_system(std::vector<root>(generators.begin(), generators.end()));
}

decorated_root_system decorated_root_system::commutator(const std::vector<root>& rs) const {
	auto new_base = base_system.commutator(rs);
	auto new_roots = new_base.roots();
	return subroot_system(std::vector<root>(new_roots.begin(), new_roots.end()));
}

decorated_root_system decorated_root_system::intersection(const decorated_root_system& other) const {
	auto new_base = base_system.intersection(other.base());
	auto new_roots = new_base.roots();
	return subroot_system(std::vector<root>(new_roots.begin(), new_roots.end()));
}

decorated_root_system decorated_root_system::change_positivity(const std::vector<math_vector>& ordered_dual_basis) const {
	std::vector<root> positive_roots;
	for (const auto& r : base_system.roots()) {
		bool is_positive = true;
		for (const auto& b : ordered_dual_basis) {
			ring v = r.coordinates() * b;
			if (v > 0) {
				break;
			} else if (v < 0) {
				is_positive = false;
				break;
			}
		}
		if (is_positive) {
			positive_roots.push_back(r);
		}
	}
	std::vector<root> new_simple_roots = simple_roots_from_positives(positive_roots);
	return change_positivity(new_simple_roots);
}

decorated_root_system decorated_root_system::change_positivity(const std::vector<root>& new_simple_roots) const {
	marking_data new_marking;
	new_marking.marks = marking->marks;
	for (const auto& r : new_simple_roots) {
		auto involuted = involution_of(r);
		assert(base_system.contains(involuted.coordinates()));
		new_marking.involuted_roots.push_back(involuted.coordinates());
	}
	return decorated_root_system(base_system, new_simple_roots, new_marking);
}
	
decorated_root_system decorated_root_system::change_positivity(const root& reflection) const {
	std::vector<root> new_simple_roots = simple_roots;
	for (size_t i = 0; i < new_simple_roots.size(); ++i) {
		new_simple_roots[i] = new_simple_roots[i].reflect_by(reflection);
	}
	return change_positivity(new_simple_roots);
}

decorated_root_system decorated_root_system::construct_from_positive_direction(const root_system& system, const math_vector& direction) {
	std::vector<root> positive_roots;
	for (const auto& r : system.roots()) {
		ring d = r.coordinates() * direction;
		if (d > 0) {
			positive_roots.push_back(r);
		}
		if (d == 0) {
			throw std::invalid_argument("Direction is orthogonal to some root");
		}
	}
	std::vector<root> simple_roots = simple_roots_from_positives(positive_roots);
	return decorated_root_system(system, simple_roots);
}
