#pragma once

#include <string>

enum class root_mark {
	real, complex, imaginary_compact, imaginary_noncompact
};


struct root_mark_func {
	static bool is_imaginary(root_mark type) {
		return type == root_mark::imaginary_compact || type == root_mark::imaginary_noncompact;
	}
	static root_mark reverse_imaginary_root(root_mark type) {
		if (type == root_mark::imaginary_compact) {
			return root_mark::imaginary_noncompact;
		} else if (type == root_mark::imaginary_noncompact) {
			return root_mark::imaginary_compact;
		} else {
			return type;
		}
	}
	static std::string to_string(root_mark type) {
		switch (type) {
			case root_mark::real:
				return "real";
			case root_mark::complex:
				return "complex";
			case root_mark::imaginary_compact:
				return "imaginary compact";
			case root_mark::imaginary_noncompact:
				return "imaginary noncompact";
		}
		return "unknown";
	}
};

struct marking_data {
	std::map<math_vector, root_mark> marks;
	std::vector<math_vector> involuted_roots;
};