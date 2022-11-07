#pragma once

#include "Vector.h"

namespace Rhodochrosite {
	struct Sphere {
		Sphere() = default;
		Sphere(Malachite::Vector3f origin, float radius);

		Malachite::Vector3f origin;
		float radius{ 0.0f };
	};
}