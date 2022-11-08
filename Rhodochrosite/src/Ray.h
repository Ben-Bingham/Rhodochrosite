#pragma once
#include "Sphere.h"
#include "Vector.h"

namespace Rhodochrosite {
	struct Ray {
		Ray() = default;
		Ray(Malachite::Vector3f origin, Malachite::Vector3f direction);

		[[nodiscard]] float hitSphere(const Sphere& sphere) const;
		[[nodiscard]] Malachite::Vector3f at(float distance) const;

		Malachite::Vector3f origin;
		Malachite::Vector3f direction;
	};
}