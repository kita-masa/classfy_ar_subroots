
#include <regex>
#include <map>

#include "my-root/root-data.h"


dynkin_diagram dynkin_diagram_A(size_t rank) {
	dynkin_diagram diagram(rank);
	for (size_t i = 0; i < rank - 1; ++i) {
		diagram.add_edge(i, i + 1, 1);
	}
	return diagram;
}

dynkin_diagram dynkin_diagram_B(size_t rank) {
	dynkin_diagram diagram(rank);
	for (size_t i = 0; i < rank - 2; ++i) {
		diagram.add_edge(i, i + 1, 1);
	}
	diagram.add_edge(rank - 1, rank - 2, 2);
	return diagram;
}

dynkin_diagram dynkin_diagram_C(size_t rank) {
	dynkin_diagram diagram(rank);
	for (size_t i = 0; i < rank - 2; ++i) {
		diagram.add_edge(i, i + 1, 1);
	}
	diagram.add_edge(rank - 2, rank - 1, 2);
	return diagram;
}

dynkin_diagram dynkin_diagram_D(size_t rank) {
	dynkin_diagram diagram(rank);
	for (size_t i = 0; i < rank - 2; ++i) {
		diagram.add_edge(i, i + 1, 1);
	}
	diagram.add_edge(rank - 3, rank - 1, 1);
	return diagram;
}

dynkin_diagram dynkin_diagram_G2() {
	dynkin_diagram diagram(2);
	diagram.add_edge(0, 1, 3);
	return diagram;
}

dynkin_diagram dynkin_diagram_F4() {
	dynkin_diagram diagram(4);
	diagram.add_edge(0, 1, 1);
	diagram.add_edge(2, 1, 2);
	diagram.add_edge(2, 3, 1);
	return diagram;
}

dynkin_diagram dynkin_diagram_E6() {
	dynkin_diagram diagram(6);
	diagram.add_edge(0, 2, 1);
	diagram.add_edge(2, 3, 1);
	diagram.add_edge(3, 4, 1);
	diagram.add_edge(4, 5, 1);
	diagram.add_edge(1, 3, 1);
	return diagram;
}

dynkin_diagram dynkin_diagram_E7() {
	dynkin_diagram diagram(7);
	diagram.add_edge(0, 2, 1);
	diagram.add_edge(2, 3, 1);
	diagram.add_edge(3, 4, 1);
	diagram.add_edge(4, 5, 1);
	diagram.add_edge(5, 6, 1);
	diagram.add_edge(1, 3, 1);
	return diagram;
}

dynkin_diagram dynkin_diagram_E8() {
	dynkin_diagram diagram(8);
	diagram.add_edge(0, 2, 1);
	diagram.add_edge(2, 3, 1);
	diagram.add_edge(3, 4, 1);
	diagram.add_edge(4, 5, 1);
	diagram.add_edge(5, 6, 1);
	diagram.add_edge(6, 7, 1);
	diagram.add_edge(1, 3, 1);
	return diagram;
}

vogan_diagram vogan_diagram_sl_real(int n) {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_A(n - 1));
	for (int i = 0; i < n - 1; ++i) {
		diagram.mark(i, root_mark::complex);
		if (i < n - 2 - i) {
			diagram.arrow(i, n - 2 - i);
		}
	}
	if (n % 2 == 0) {
		diagram.mark(n / 2 - 1, root_mark::imaginary_noncompact);
	}
	return diagram;
}

vogan_diagram vogan_diagram_su(int p, int q) {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_A(p + q - 1));
	for (int i = 0; i < p + q - 1; ++i) {
		diagram.mark(i, root_mark::imaginary_compact);
	}
	diagram.mark(p - 1, root_mark::imaginary_noncompact);
	return diagram;
}

// p, q \neq 0
vogan_diagram vogan_diagram_so(int p, int q) {
	int rank = (p + q) / 2;
	if ((p + q) % 2 != 0) {
		if (q % 2 == 0) {
			std::swap(p, q);
		}
		vogan_diagram diagram = vogan_diagram(dynkin_diagram_B(rank));
		for (int i = 0; i < rank; ++i) {
			diagram.mark(i, root_mark::imaginary_compact);
		}
		diagram.mark(p / 2 - 1, root_mark::imaginary_noncompact);
		return diagram;
	} else {
		vogan_diagram diagram = vogan_diagram(dynkin_diagram_D(rank));
		if (p > q) {
			std::swap(p, q);
		}
		for (int i = 0; i < rank; ++i) {
			diagram.mark(i, root_mark::imaginary_compact);
		}
		if (p % 2 != 0) {
			diagram.arrow(rank - 1, rank - 2);
		}
		if (p > 1) diagram.mark(p / 2 - 1, root_mark::imaginary_noncompact);
		return diagram;
	}
}

// G2(2), GI
vogan_diagram vogan_diagram_G2_split() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_G2());
	diagram.mark(0, root_mark::imaginary_compact);
	diagram.mark(1, root_mark::imaginary_noncompact);
	return diagram;
}

// F4(4), FI
vogan_diagram vogan_diagram_F4_split() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_F4());
	diagram.mark(0, root_mark::imaginary_noncompact);
	diagram.mark(1, root_mark::imaginary_compact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(3, root_mark::imaginary_compact);
	return diagram;
}

// F4(-20), FII
vogan_diagram vogan_diagram_F4_rank1() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_F4());
	diagram.mark(0, root_mark::imaginary_compact);
	diagram.mark(1, root_mark::imaginary_compact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(3, root_mark::imaginary_noncompact);
	return diagram;
}

// E6(6), EI
vogan_diagram vogan_diagram_E6_split() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E6());
	diagram.mark(1, root_mark::imaginary_noncompact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::complex);
	diagram.mark(2, root_mark::complex);
	diagram.mark(4, root_mark::complex);
	diagram.mark(5, root_mark::complex);
	diagram.arrow(0, 5);
	diagram.arrow(2, 4);
	return diagram;
}

// E6(2), EII
vogan_diagram vogan_diagram_E6_quaternionic() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E6());
	diagram.mark(1, root_mark::imaginary_noncompact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::imaginary_compact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(4, root_mark::imaginary_compact);
	diagram.mark(5, root_mark::imaginary_compact);
	return diagram;
}

// E6(-14), EIII
vogan_diagram vogan_diagram_E6_hermitian() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E6());
	diagram.mark(1, root_mark::imaginary_compact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::imaginary_noncompact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(4, root_mark::imaginary_compact);
	diagram.mark(5, root_mark::imaginary_compact);
	return diagram;
}

// E6(-26), EIV
vogan_diagram vogan_diagram_E6_other() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E6());
	diagram.mark(1, root_mark::imaginary_compact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::complex);
	diagram.mark(2, root_mark::complex);
	diagram.mark(4, root_mark::complex);
	diagram.mark(5, root_mark::complex);
	diagram.arrow(0, 5);
	diagram.arrow(2, 4);
	return diagram;
}

// E7(7), EV
vogan_diagram vogan_diagram_E7_split() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E7());
	diagram.mark(1, root_mark::imaginary_noncompact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::imaginary_compact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(4, root_mark::imaginary_compact);
	diagram.mark(5, root_mark::imaginary_compact);
	diagram.mark(6, root_mark::imaginary_compact);
	return diagram;
}


// E7(-5), EVI
vogan_diagram vogan_diagram_E7_quaternionic() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E7());
	diagram.mark(1, root_mark::imaginary_compact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::imaginary_noncompact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(4, root_mark::imaginary_compact);
	diagram.mark(5, root_mark::imaginary_compact);
	diagram.mark(6, root_mark::imaginary_compact);
	return diagram;
}

// E7(-25), EVII
vogan_diagram vogan_diagram_E7_hermitian() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E7());
	diagram.mark(1, root_mark::imaginary_compact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::imaginary_compact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(4, root_mark::imaginary_compact);
	diagram.mark(5, root_mark::imaginary_compact);
	diagram.mark(6, root_mark::imaginary_noncompact);
	return diagram;
}

// E8(8), EVIII
vogan_diagram vogan_diagram_E8_split() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E8());
	diagram.mark(1, root_mark::imaginary_compact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::imaginary_noncompact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(4, root_mark::imaginary_compact);
	diagram.mark(5, root_mark::imaginary_compact);
	diagram.mark(6, root_mark::imaginary_compact);
	diagram.mark(7, root_mark::imaginary_compact);
	return diagram;
}

// E8(-24), EIX
vogan_diagram vogan_diagram_E8_quaternionic() {
	vogan_diagram diagram = vogan_diagram(dynkin_diagram_E8());
	diagram.mark(1, root_mark::imaginary_compact);
	diagram.mark(3, root_mark::imaginary_compact);
	diagram.mark(0, root_mark::imaginary_compact);
	diagram.mark(2, root_mark::imaginary_compact);
	diagram.mark(4, root_mark::imaginary_compact);
	diagram.mark(5, root_mark::imaginary_compact);
	diagram.mark(6, root_mark::imaginary_compact);
	diagram.mark(7, root_mark::imaginary_noncompact);
	return diagram;
}

std::tuple<std::string, int, int> _parse_indefinite(const std::string& s) {
	const std::regex re(R"(^([A-Z]+)\((\d+),(\d+)\)$)");
	std::smatch m;

	if (!std::regex_match(s, m, re)) {
		return { "", -1, -1 };
	}

	return {
		m[1].str(),
		std::stoi(m[2].str()),
		std::stoi(m[3].str())
	};
}

std::map<std::string, vogan_diagram(*)()> diagram_mapping = {
	{ "G2_split", vogan_diagram_G2_split },
	{ "F4_split", vogan_diagram_F4_split },
	{ "F4_rank1", vogan_diagram_F4_rank1 },
	{ "E6_split", vogan_diagram_E6_split },
	{ "E6_quaternionic", vogan_diagram_E6_quaternionic },
	{ "E6_hermitian", vogan_diagram_E6_hermitian },
	{ "E6_other", vogan_diagram_E6_other },
	{ "E7_split", vogan_diagram_E7_split },
	{ "E7_quaternionic", vogan_diagram_E7_quaternionic },
	{ "E7_hermitian", vogan_diagram_E7_hermitian },
	{ "E8_split", vogan_diagram_E8_split },
	{ "E8_quaternionic", vogan_diagram_E8_quaternionic },
};

std::vector<std::string> exceptional_vogan_diagram_identifiers() {
	std::vector<std::string> identifiers;
	for (const auto& [key, _] : diagram_mapping) {
		identifiers.push_back(key);
	}
	return identifiers;
}

bool test_vogan_diagram_identifier(const std::string& s) {
	auto [ type, p, q ] = _parse_indefinite(s);
	if (type == "SO" || type == "SU") {
		return true;
	}
	return diagram_mapping.find(s) != diagram_mapping.end();
}

vogan_diagram vogan_diagram_from_string(const std::string& s) {
	auto [ type, p, q ] = _parse_indefinite(s);
	if (type == "SO") {
		return vogan_diagram_so(p, q);
	} else if (type == "SU") {
		return vogan_diagram_su(p, q);
	}
	auto it = diagram_mapping.find(s);
	if (it != diagram_mapping.end()) {
		return (it->second)();
	}
	throw std::invalid_argument("Invalid diagram string: " + s);
}