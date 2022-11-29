#pragma once

#include "Utility/Colour.h"
#include "Vector.h"

namespace Rhodochrosite {
	enum class Material {
		REFLECTION,
		REFRACTION,
		DIFFUSE
	};

	struct Sphere {
		Sphere() = default;
		Sphere(Malachite::Vector3f origin, float radius, Ruby::Colour colour, Material = Material::DIFFUSE);

		Malachite::Vector3f origin;
		float radius{ 0.0f };
		Ruby::Colour colour{ 0, 0, 0, 255 };
		Material material{ Material::DIFFUSE };
	};
}