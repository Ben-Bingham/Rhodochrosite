#include "Ray.h"

namespace Rhodochrosite {
	Ray::Ray(Malachite::Vector3f Origin, Malachite::Vector3f Direction)
		: origin(Origin)
		, direction(Direction) { }

	float Ray::hitSphere(const Sphere& sphere) const {
		const Malachite::Vector3f distanceBetween = origin - sphere.origin;

		const float a = direction.lengthSquared();
		const float halfB = dot(distanceBetween, direction);
		const float c = distanceBetween.lengthSquared() - (sphere.radius * sphere.radius);

		const float discriminant = halfB * halfB - a * c;
		if (discriminant < 0.0f) {
			return 0.0f;
		}

		return (-halfB - std::sqrt(discriminant)) / a;
	}

	Malachite::Vector3f Ray::at(const float distance) const {
		return distance * direction + origin;
	}
}