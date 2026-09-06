#pragma once

#include <vector>
#include <string>

#include "root-system.h"

struct dot_node {
	std::string node_name, label, font_color;
	math_vector coordinates;
	bool is_simple;
	root_mark mark;

	dot_node(const math_vector& coordinates, const std::string& name, const std::string& label, bool is_simple, root_mark mark, const std::string& font_color = "red") : node_name(name), label(label), font_color(font_color), coordinates(coordinates), is_simple(is_simple), mark(mark) {}

	std::string to_dot() const;
};

struct dot_edge {;
	int short_root, long_root, label;

	dot_edge(int short_root, int long_root, int label) : short_root(short_root), long_root(long_root), label(label) {}

	std::string to_dot() const;
};

class dot_data {
protected:
	std::map<math_vector, int> coords_to_node;
	std::vector<dot_node> nodes;
	std::vector<dot_edge> edges;
	std::string label;
public:
	void add_node(const math_vector& coordinates, const std::string& label, bool is_simple, root_mark mark);
	void add_edge(const math_vector& short_root, const math_vector& long_root, int label);
	bool contains_node(const math_vector& coordinates) const;
	void add_node_label(const math_vector& coordinates, const std::string& label);
	void add_label(const std::string& graph_label);
	std::vector<math_vector> all_coordinates() const;

	std::string to_dot() const;
	void save_to_file(const std::string& filename) const;
};

dot_data root_system_to_dot(const root_system& system);
dot_data decorated_root_system_to_dot(const decorated_root_system& system, bool only_positive = false);
dot_data vogan_diagram_as_dot(const decorated_root_system& system, bool with_lowest_root = false);