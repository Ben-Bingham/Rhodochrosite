#pragma once

#include <vector>

#include "Lights.h"
#include "Sphere.h"

namespace Rhodochrosite {
	struct Scene {
		std::vector<Sphere> spheres;
		std::vector<Ruby::DirectionalLight> lights;
	};
}