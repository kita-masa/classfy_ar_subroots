#pragma once

#include <vector>

#include "root.h"
#include "root-mark.h"

class cartan_matrix {
protected:
	std::vector<std::vector<ring>> matrix;
public:
	cartan_matrix(const std::vector<std::vector<ring>>& mat);
	cartan_matrix(size_t rank);
	void set_entry(int i, int j, ring value);
	std::vector<root> simple_roots() const;
	size_t rank() const;
};

class dynkin_diagram {
protected:
	cartan_matrix cartan;
public:
	dynkin_diagram(size_t rank);

	void add_edge(int short_root, int long_root, ring value);
	std::vector<root> simple_roots() const;
	size_t rank() const;
};

class vogan_diagram : public dynkin_diagram {
protected:
	std::vector<math_vector> involuted_roots;
	std::vector<root_mark> marks;
	void reset_marking();
public:
	vogan_diagram(size_t rank);
	vogan_diagram(const dynkin_diagram& diagram);

	void mark(int i, root_mark mark);
	void arrow(int i, int j);
	marking_data marking() const;
};