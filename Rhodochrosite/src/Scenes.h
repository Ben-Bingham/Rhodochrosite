#pragma once

#include "Scene.h"

namespace Rhodochrosite {
	class Scenes {
	public:
		Scenes();

		Scene oneSphere;
		Scene twoSpheres;
		Scene lotsOfSpheres;
		Scene randomSpheres;

		void regenerateRandomSpheres();

	private:
		static Scene oneSphereInit();
		static Scene twoSpheresInit();
		static Scene lotsOfSpheresInit();
		static Scene randomSpheresInit();
	};
}