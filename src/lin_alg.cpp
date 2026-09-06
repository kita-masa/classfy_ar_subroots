
#include <numeric>

#include "my-root/lin_alg.h"

void field::reduce() {
	ring g = std::gcd(num, denom);
	num /= g;
	denom /= g;
	if (denom < 0) {
		num = -num;
		denom = -denom;
	}
}

field::field(ring numerator, ring denominator) : num(numerator), denom(denominator) {
	reduce();
}

field::field(ring integer) : num(integer), denom(1) {}

field field::operator+(const field& other) const {
	return field(num * other.denom + other.num * denom, denom * other.denom);
}

field field::operator-(const field& other) const {
	return field(num * other.denom - other.num * denom, denom * other.denom);
}

field field::operator*(const field& other) const {
	return field(num * other.num, denom * other.denom);
}

field field::operator/(const field& other) const {
	return field(num * other.denom, denom * other.num);
}

field field::operator-() const {
	return field(-num, denom);
}

bool field::operator==(const field& other) const {
	return num == other.num && denom == other.denom;
}

bool field::operator!=(const field& other) const {
	return !(*this == other);
}

bool field::operator<(const field& other) const {
	return num * other.denom < other.num * denom;
}

void _print_matrix(const std::vector<std::vector<field>>& matrix) {
	for (const auto& row : matrix) {
		for (const auto& x : row) {
			std::cout << x.numerator() << "/" << x.denominator() << " ";
		}
		std::cout << "\n";
	}
}

void _print_vector(const std::vector<field>& vec) {
	for (const auto& x : vec) {
		std::cout << x.numerator() << "/" << x.denominator() << " ";
	}
	std::cout << "\n";
}

math_vector solve_linear_equation_with_unique_solution(const std::vector<std::vector<ring>>& matrix) {
	int n = matrix.size() + 1;
	std::vector<std::vector<field>> mat(n - 1, std::vector<field>(n));
	for (int i = 0; i < n - 1; ++i) {
		for (int j = 0; j < n; ++j) {
			mat[i][j] = matrix[i][j];
		}
	}
	int bad_col = -1;
	for (int i = 0, row = 0; i < n; ++i) {
		int pivot = row;
		while (pivot < n - 1 && mat[pivot][i] == 0) {
			++pivot;
		}
		if (pivot == n - 1) {
			bad_col = i;
			continue;
		}
		if (pivot != row) {
			std::swap(mat[row], mat[pivot]);
		}
		field inv_pivot = field(1) / mat[row][i];
		for (int k = i; k < n; ++k) {
			mat[row][k] = mat[row][k] * inv_pivot;
		}
		for (int j = 0; j < n - 1; ++j) {
			if (row == j) continue;
			field factor = mat[j][i];
			for (int k = i; k < n; ++k) {
				mat[j][k] = mat[j][k] - factor * mat[row][k];
			}
		}
		++row;
	}
	std::vector<field> solution(n);
	for (int i = 0, row = 0; i < n; ++i) {
		if (i == bad_col) continue;
		if (row < n - 1) {
			solution[i] = mat[row][bad_col];
		}
		++row;
	}
	solution[bad_col] = field(-1);
	ring factor = 1;
	for (const auto& x : solution) {
		if (x != 0) {
			factor = std::lcm(factor, x.denominator());
		}
	}
	std::vector<ring> result(n);
	for (int i = 0; i < n; ++i) {
		result[i] = (solution[i] * factor).numerator();
	}
	return result;
}
