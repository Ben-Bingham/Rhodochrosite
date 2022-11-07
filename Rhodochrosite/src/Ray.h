#pragma once
#include "Vector.h"

namespace Rhodochrosite {
	struct Ray {
		Ray() = default;
		Ray(Malachite::Vector3f origin, Malachite::Vector3f direction);

		[[nodiscard]] Malachite::Vector3f at(float distance) const;

		Malachite::Vector3f origin;
		Malachite::Vector3f direction;
	};
}