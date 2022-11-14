#pragma once

#include "Scene.h"

namespace Rhodochrosite {
	class Scenes {
	public:
		Scenes();

		Scene oneSphere;
		Scene sphereOnPlane;
		Scene twoSpheres;
		Scene lotsOfSpheres;
		Scene randomSpheres;

		void regenerateRandomSpheres();

	private:
		static Scene oneSphereInit();
		static Scene sphereOnPlaneInit();
		static Scene twoSpheresInit();
		static Scene lotsOfSpheresInit();
		static Scene randomSpheresInit();
	};
}