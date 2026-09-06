#include "my-root/diagram.h"

cartan_matrix::cartan_matrix(const std::vector<std::vector<ring>>& mat) : matrix(mat) {}
cartan_matrix::cartan_matrix(size_t _rank) : matrix(_rank, std::vector<ring>(_rank, 0)) {
	for (size_t i = 0; i < _rank; ++i) {
		matrix[i][i] = 2;
	}
}

void cartan_matrix::set_entry(int i, int j, ring value) {
	matrix[i][j] = value;
	matrix[j][i] = -1;
}

std::vector<root> cartan_matrix::simple_roots() const {
	std::vector<root> roots;
	for (size_t i = 0; i < matrix.size(); ++i) {
		math_vector coords(matrix.size());
		math_vector coroot(matrix.size());
		coords[i] = 1;
		for (size_t j = 0; j < matrix.size(); ++j) {
			coroot[j] = matrix[i][j];
		}
		roots.emplace_back(coords, coroot);
	}
	return roots;
}

size_t cartan_matrix::rank() const {
	return matrix.size();
}

dynkin_diagram::dynkin_diagram(size_t _rank) : cartan(_rank) {}

void dynkin_diagram::add_edge(int short_root, int long_root, ring value) {
	cartan.set_entry(short_root, long_root, -value);
}

std::vector<root> dynkin_diagram::simple_roots() const {
	return cartan.simple_roots();
}

size_t dynkin_diagram::rank() const {
	return cartan.rank();
}

vogan_diagram::vogan_diagram(size_t _rank) : dynkin_diagram(_rank) {
	reset_marking();
}

vogan_diagram::vogan_diagram(const dynkin_diagram& diagram) : dynkin_diagram(diagram) {
	reset_marking();
}

void vogan_diagram::reset_marking() {
	involuted_roots.resize(rank(), math_vector(rank()));
	marks.resize(rank(), root_mark::imaginary_compact);
	for (size_t i = 0; i < rank(); ++i) {
		math_vector involuted(rank());
		involuted[i] = 1;
		involuted_roots[i] = involuted;
	}
}

void vogan_diagram::mark(int i, root_mark mark) {
	marks[i] = mark;
	if (mark == root_mark::real) {
		math_vector involuted(rank());
		involuted[i] = -1;
		involuted_roots[i] = involuted;
	} else if (root_mark_func::is_imaginary(mark)) {
		math_vector involuted(rank());
		involuted[i] = 1;
		involuted_roots[i] = involuted;
	}
}

void vogan_diagram::arrow(int i, int j) {
	marks[i] = marks[j] = root_mark::complex;
	math_vector r1(rank()), r2(rank());
	r1[i] = 1;
	r2[j] = 1;
	involuted_roots[i] = r2;
	involuted_roots[j] = r1;
}

marking_data vogan_diagram::marking() const {
	marking_data data;
	for (size_t i = 0; i < rank(); ++i) {
		math_vector r(rank());
		r[i] = 1;
		data.marks[r] = marks[i];
	}
	data.involuted_roots = involuted_roots;
	return data;
}
