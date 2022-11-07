#pragma once

#include "Colour.h"
#include "Vector.h"

namespace Rhodochrosite {
	struct Sphere {
		Sphere() = default;
		Sphere(Malachite::Vector3f origin, float radius, Ruby::Colour colour);

		Malachite::Vector3f origin;
		float radius{ 0.0f };
		Ruby::Colour colour{ 0, 0, 0, 255 };
	};
}