#include "my-root/root.h"

root::root(const math_vector& coords, const math_vector& coroots) : _coordinates(coords), _coroot(coroots) {}
root::root(const root& other) : _coordinates(other._coordinates), _coroot(other._coroot) {}

bool root::valid() const {
	return _coordinates.dimension() > 0 && _coroot.dimension() > 0;
}

const math_vector& root::coordinates() const {
	return _coordinates;
}

const math_vector& root::coroot() const {
	return _coroot;
}

ring root::pairing(const root& other) const {
	return _coordinates * other._coroot;
}

root root::reflect_by(const root& other) const {
	math_vector new_coords = _coordinates - (_coordinates * other._coroot) * other._coordinates;
	math_vector new_coroot = _coroot - (_coroot * other._coordinates) * other._coroot;
	return root(new_coords, new_coroot);
}

root root::operator-() const {
	return root(-_coordinates, -_coroot);
}

bool root::operator==(const root& other) const {
	return _coordinates == other._coordinates;
}

bool root::operator!=(const root& other) const {
	return !(*this == other);
}

bool root::operator<(const root& other) const {
	return _coordinates < other._coordinates;
}
