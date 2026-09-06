#include <cassert>

#include "my-root/symmetric-pair.h"


decorated_root_system symmetric_pair_from_grading(const decorated_root_system &system, const root& highest_root) {
	std::vector<root> result;
	for (const auto& r : system.roots()) {
		if (r.pairing(highest_root) % 2 == 0) {
			result.push_back(r);
		}
	}
	return system.subroot_system(result);
}

decorated_root_system associated_pair(const decorated_root_system& system, const std::vector<root>& symmetric_sub) {
	std::vector<root> result;
	std::set<root> symmetric_set(symmetric_sub.begin(), symmetric_sub.end());
	for (const auto& r : system.roots()) {
		if ((system.mark_of(r) == root_mark::imaginary_compact && symmetric_set.find(r) != symmetric_set.end()) || (system.mark_of(r) == root_mark::imaginary_noncompact && symmetric_set.find(r) == symmetric_set.end())) {
			result.push_back(r);
		}
	}
	return system.subroot_system(result);
}

bool is_symmetric_pair(const decorated_root_system& system, const decorated_root_system& subsystem) {
	for (const auto& r : system.roots()) {
		if (subsystem.base().contains(r.coordinates())) continue;
		for (const auto& s : system.roots()) {
			if (subsystem.base().contains(s.coordinates())) continue;
			auto sum = r.coordinates() + s.coordinates();
			if (sum.zero()) continue;
			if (system.base().contains(sum) && !subsystem.base().contains(sum)) {
				return false;
			}
		}
	}
	return true;
}
