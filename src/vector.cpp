#include "my-root/vector.h"

math_vector::math_vector(const std::vector<ring>& coords) : coordinates(coords) {}
math_vector::math_vector(size_t dimension) : coordinates(dimension, 0) {}
math_vector::math_vector(const math_vector& other) : coordinates(other.coordinates) {}

size_t math_vector::dimension() const {
	return coordinates.size();
}

ring& math_vector::operator[](size_t index) {
	return coordinates[index];
}

const ring& math_vector::operator[](size_t index) const {
	return coordinates[index];
}

math_vector math_vector::operator+(const math_vector& other) const {
	std::vector<ring> new_coords(coordinates.size());
	for (size_t i = 0; i < coordinates.size(); ++i) {
		new_coords[i] = coordinates[i] + other.coordinates[i];
	}
	return math_vector(new_coords);
}

math_vector math_vector::operator-(const math_vector& other) const {
	std::vector<ring> new_coords(coordinates.size());
	for (size_t i = 0; i < coordinates.size(); ++i) {
		new_coords[i] = coordinates[i] - other.coordinates[i];
	}
	return math_vector(new_coords);
}

math_vector math_vector::operator*(ring scalar) const {
	std::vector<ring> new_coords(coordinates.size());
	for (size_t i = 0; i < coordinates.size(); ++i) {
		new_coords[i] = coordinates[i] * scalar;
	}
	return math_vector(new_coords);
}

math_vector math_vector::operator/(ring scalar) const {
	std::vector<ring> new_coords(coordinates.size());
	for (size_t i = 0; i < coordinates.size(); ++i) {
		new_coords[i] = coordinates[i] / scalar;
	}
	return math_vector(new_coords);
}

math_vector math_vector::operator-() const {
	std::vector<ring> new_coords(coordinates.size());
	for (size_t i = 0; i < coordinates.size(); ++i) {
		new_coords[i] = -coordinates[i];
	}
	return math_vector(new_coords);
}

ring math_vector::operator*(const math_vector& other) const {
	ring result = 0;
	for (size_t i = 0; i < coordinates.size(); ++i) {
		result += coordinates[i] * other.coordinates[i];
	}
	return result;
}

math_vector operator*(ring scalar, const math_vector& vec) {
	return vec * scalar;
}

bool math_vector::operator==(const math_vector& other) const {
	return coordinates == other.coordinates;
}

bool math_vector::operator!=(const math_vector& other) const {
	return !(*this == other);
}

bool math_vector::operator<(const math_vector& other) const {
	return coordinates < other.coordinates;
}

bool math_vector::zero() const {
	for (const auto& coord : coordinates) {
		if (coord != 0) return false;
	}
	return true;
}

const std::vector<ring>& math_vector::as_vector() const {
	return coordinates;
}

std::ostream& operator<<(std::ostream& os, const math_vector& vec) {
	os << vec.to_string();
	return os;
}

std::istream& operator>>(std::istream& is, math_vector& vec) {
	for (size_t i = 0; i < vec.coordinates.size(); ++i) {
		is >> vec.coordinates[i];
	}
	return is;
}

std::string math_vector::to_string() const {
	std::string result;
	for (size_t i = 0; i < coordinates.size(); ++i) {
		result += std::to_string(coordinates[i]);
		if (i < coordinates.size() - 1) {
			result += " ";
		}
	}
	return result;
}