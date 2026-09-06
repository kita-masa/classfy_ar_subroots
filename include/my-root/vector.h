#pragma once

#include <vector>
#include <cstddef>
#include <iostream>

using ring = int;

class math_vector {
private:
	std::vector<ring> coordinates;
public:
	math_vector(const std::vector<ring>& coords);
	math_vector(size_t dimension);
	math_vector(const math_vector& other);
	math_vector() = default;

	size_t dimension() const;

	ring& operator[](size_t index);
	const ring& operator[](size_t index) const;

	math_vector operator+(const math_vector& other) const;
	math_vector operator-(const math_vector& other) const;
	math_vector operator*(ring scalar) const;
	math_vector operator/(ring scalar) const;
	math_vector operator-() const;
	ring operator*(const math_vector& other) const;

	friend math_vector operator*(ring scalar, const math_vector& vec);

	bool operator==(const math_vector& other) const;
	bool operator!=(const math_vector& other) const;
	bool operator<(const math_vector& other) const;
	bool zero() const;

	const std::vector<ring>& as_vector() const;
	std::string to_string() const;

	friend std::ostream& operator<<(std::ostream& os, const math_vector& vec);
	friend std::istream& operator>>(std::istream& is, math_vector& vec);
};


