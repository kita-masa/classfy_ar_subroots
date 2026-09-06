#pragma once

#include <map>

#include "vector.h"
#include "root-mark.h"

class root {
	math_vector _coordinates, _coroot;
public:
	root(const math_vector& coords, const math_vector& coroots);
	root(const root& other);
	root() = default;

	bool valid() const;

	const math_vector& coordinates() const;
	const math_vector& coroot() const;

	ring pairing(const root& other) const;
	root reflect_by(const root& other) const;
	root operator-() const;
	bool operator==(const root& other) const;
	bool operator!=(const root& other) const;
	bool operator<(const root& other) const;
};
