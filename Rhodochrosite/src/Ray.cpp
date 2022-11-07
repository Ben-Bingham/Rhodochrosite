#include "Ray.h"

namespace Rhodochrosite {
	Ray::Ray(Malachite::Vector3f Origin, Malachite::Vector3f Direction)
		: origin(Origin)
		, direction(Direction) { }

	Malachite::Vector3f Ray::at(const float distance) const {
		return (distance * direction) + origin;
	}
}