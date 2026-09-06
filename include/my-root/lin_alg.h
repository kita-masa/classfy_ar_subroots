
#include <vector>
#include "vector.h"

class field {
protected:
	ring denom, num;
	void reduce();
public:
	field() : denom(1), num(0) {};
	field(ring numerator, ring denominator);
	field(ring integer);

	field operator+(const field& other) const;
	field operator-(const field& other) const;
	field operator*(const field& other) const;
	field operator/(const field& other) const;
	field operator-() const;
	bool operator==(const field& other) const;
	bool operator!=(const field& other) const;
	bool operator<(const field& other) const;

	ring numerator() const { return num; }
	ring denominator() const { return denom; }
};


math_vector solve_linear_equation_with_unique_solution(const std::vector<std::vector<ring>>& matrix);