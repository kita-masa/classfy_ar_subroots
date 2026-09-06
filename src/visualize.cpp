
#include <sstream>
#include <fstream>
#include <cassert>

#include "my-root/visualize.h"


std::string dot_node::to_dot() const {
	std::string shape = is_simple ? "doublecircle" : "circle";
	std::string color;
	switch (mark) {
		case root_mark::real:
			color = "red";
			break;
		case root_mark::imaginary_compact:
			color = "blue";
			break;
		case root_mark::imaginary_noncompact:
			color = "purple";
			break;
		case root_mark::complex:
			color = "yellow";
			break;
	}
	return node_name + " [style=filled, label=\"" + label + "\", shape=" + shape + ", fillcolor=" + color + ", fontcolor=" + font_color + ", tooltip=\"" + coordinates.to_string() + "\"]";
}

std::string dot_edge::to_dot() const {
	std::string label_str = "", dir = "";
	if (label == 1) {
		dir = "\", dir=none";
	} else {
		label_str = std::to_string(label);
		dir = "\"";
	}
	return std::to_string(long_root) + " -> " + std::to_string(short_root) + " [label=\"" + label_str + dir + "]";
}

void dot_data::add_node(const math_vector& coordinates, const std::string& label, bool is_simple, root_mark mark) {
	coords_to_node[coordinates] = nodes.size();
	nodes.emplace_back(coordinates, std::to_string(nodes.size()), label, is_simple, mark);
}

void dot_data::add_edge(const math_vector& short_root, const math_vector& long_root, int label) {
	edges.emplace_back(coords_to_node.at(short_root), coords_to_node.at(long_root), label);
}

bool dot_data::contains_node(const math_vector& coordinates) const {
	return coords_to_node.find(coordinates) != coords_to_node.end();
}

void dot_data::add_node_label(const math_vector& coordinates, const std::string& label) {
	if (contains_node(coordinates)) {
		auto &node = nodes[coords_to_node.at(coordinates)];
		if (node.label != "") {
			node.label += ", ";
		}
		node.label += label;
	}
}

void dot_data::add_label(const std::string& graph_label) {
	if (!label.empty()) {
		label += "\\n";
	}
	label += graph_label;
}

std::vector<math_vector> dot_data::all_coordinates() const {
	std::vector<math_vector> result;
	for (const auto& node : nodes) {
		result.push_back(node.coordinates);
	}
	return result;
}

std::string dot_data::to_dot() const {
	std::stringstream result;
	result << "digraph G {" << std::endl;
	if (!label.empty()) {
		result << "  label=\"" << label << "\";" << std::endl;
	}
	for (const auto& node : nodes) {
		result << "  " << node.to_dot() << std::endl;
	}
	for (const auto& edge : edges) {
		result << "  " << edge.to_dot() << std::endl;
	}
	result << "}" << std::endl;
	return result.str();
}

void dot_data::save_to_file(const std::string& filename) const {
	std::ofstream file(filename);
	if (!file) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		assert(false);
	}
	file << to_dot();
	file.close();
}

void _add_edges_subroutine(const root_system& system, dot_data& data) {
	auto roots_set = system.roots();
	std::vector<root> roots = std::vector<root>(roots_set.begin(), roots_set.end());
	for (size_t i = 0; i < roots.size(); ++i) {
		for (size_t j = i + 1; j < roots.size(); ++j) {
			if (roots[i].coordinates() == -roots[j].coordinates()) continue;
			if (!data.contains_node(roots[i].coordinates()) || !data.contains_node(roots[j].coordinates())) continue;
			ring pairing = std::abs(roots[i].pairing(roots[j]));
			ring reverse_pairing = std::abs(roots[j].pairing(roots[i]));
			if (pairing == 0) continue;
			if (pairing >= reverse_pairing) {
				data.add_edge(roots[j].coordinates(), roots[i].coordinates(), pairing);
			} else {
				data.add_edge(roots[i].coordinates(), roots[j].coordinates(), reverse_pairing);
			}
		}
	}
}

dot_data root_system_to_dot(const root_system& system) {
	dot_data data;
	for (const auto& r : system.roots()) {
		data.add_node(r.coordinates(), "", false, root_mark::real);
	}
	_add_edges_subroutine(system, data);
	return data;
}

dot_data decorated_root_system_to_dot(const decorated_root_system& system, bool only_positive) {
	dot_data data;
	for (const auto& [r, mark, involuted_coords] : system.all_roots_with_marks()) {
		if (!only_positive || system.is_positive(r)) {
			data.add_node(r.coordinates(), "", false, mark);
		}
	}
	_add_edges_subroutine(system.base(), data);
	return data;
}

dot_data vogan_diagram_as_dot(const decorated_root_system& system, bool with_lowest_root) {
	dot_data data;
	std::vector<root> simple_roots = system.all_simple_roots();
	for (const auto& r : simple_roots) {
		data.add_node(r.coordinates(), "", true, system.mark_of(r));
	}
	if (with_lowest_root) {
		for (const auto& r : system.highest_root_non_irreducible()) {
			root lowest_root = -r;
			data.add_node(lowest_root.coordinates(), "", true, system.mark_of(lowest_root));
		}
	}
	_add_edges_subroutine(system.base(), data);
	return data;
}