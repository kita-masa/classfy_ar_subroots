#pragma once

#include <vector>
#include <set>
#include <map>
#include <ranges>
#include <queue>
#include <optional>
#include <initializer_list>

#include "root.h"
#include "root-mark.h"


class root_system {
protected:
	std::map<math_vector, root> _roots;
	root_system() = default;
public:
	root_system(const std::vector<root>& roots);
	size_t size() const;

	auto vectors() const {
		return std::views::keys(_roots);
	}

	auto roots() const {
		return std::views::values(_roots);
	}

	std::set<root> all_roots() const;
	std::vector<root> roots_orthogonal_to(const std::vector<root>& rs) const;
	std::vector<root> orbit(const root& r) const;
	std::vector<root> orbits_by_orthogonals(const std::vector<root>& rs) const;
	std::vector<root> extremal_roots() const;
	bool is_simple_roots(const std::vector<root> &simple_roots) const;
	root_system subroot_system(const std::vector<root>& generators) const;
	std::vector<root> maximal_strongly_orthogonal_subset(const std::vector<root>& rs) const;

	root operator[](const math_vector& coords) const;

	bool contains(const math_vector& coords) const;
	bool is_strongly_orthogonal(const root& r1, const root& r2) const;
	root_system commutator(const std::vector<root>& rs) const;
	root_system intersection(const root_system& other) const;

	template <std::ranges::input_range R> static root_system generated_by(const R& generators) {
		root_system system;
		std::queue<root> que;
		std::vector<root> g;
		for (const auto& gen : generators) {
				system._roots.insert({ gen.coordinates(), gen });
				g.push_back(gen);
				que.push(gen);
		}
		while (!que.empty()) {
			root current = que.front();
			que.pop();
			for (const auto& gen : g) {
				root reflected = current.reflect_by(gen);
				if (system._roots.insert({ reflected.coordinates(), reflected }).second) {
					que.push(reflected);
				}
			}
		}
		return system;
	}
};

class decorated_root_system {
protected:
	std::vector<root> simple_roots;
	std::optional<marking_data> marking;
	std::map<root, root> rebased_roots;
	root_system base_system;

	void generate_marks();
public:

	decorated_root_system(root_system system, std::vector<root> simple_roots);
	decorated_root_system(root_system system, std::vector<root> simple_roots, marking_data marking);

	const root_system& base() const;
	size_t rank() const;
	size_t real_rank() const;
	void set_marking(const marking_data& data);
	root involution_of(const root& r) const;
	root highest_root() const;
	std::vector<root> highest_root_non_irreducible() const;
	root_mark mark_of(const root& r) const;
	bool is_positive(const root& r) const;
	bool is_subroot_system(const std::vector<root>& rs) const;

	auto roots() const {
		return base_system.roots();
	}
	std::set<root> all_roots(root_mark mark) const;
	std::vector<std::tuple<root, root_mark, math_vector>> all_roots_with_marks() const;
	std::vector<root> simple_roots_with_mark(root_mark mark) const;
	std::vector<root> all_simple_roots() const;

	decorated_root_system cayley_transform_imaginary(const root& r) const;
	decorated_root_system subroot_system(const std::vector<root>& generators) const;
	decorated_root_system subroot_system(std::initializer_list<std::reference_wrapper<const decorated_root_system>> systems) const;
	decorated_root_system commutator(const std::vector<root>& rs) const;
	decorated_root_system intersection(const decorated_root_system& other) const;
	decorated_root_system change_positivity(const std::vector<math_vector>& ordered_dual_basis) const;
	decorated_root_system change_positivity(const std::vector<root>& new_simple_roots) const;
	decorated_root_system change_positivity(const root& reflection) const;

	static decorated_root_system construct_from_positive_direction(const root_system& system, const math_vector& direction);
};