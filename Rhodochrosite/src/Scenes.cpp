#include "Scenes.h"

namespace Rhodochrosite {
	Scenes::Scenes()
		: oneSphere(oneSphereInit())
		, twoSpheres(twoSpheresInit())
		, lotsOfSpheres(lotsOfSpheresInit())
		, randomSpheres(randomSpheresInit()) {

	}

	Scene Scenes::oneSphereInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink });
		scene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });
		return scene;
	}

	Scene Scenes::twoSpheresInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.0f, 0.0f, -4.0f}, 0.75f, Ruby::Colour::blue });
		scene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });
		return scene;
	}

	Scene Scenes::lotsOfSpheresInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, -100.5f, 0.0f}, 100.0f, Ruby::Colour{88, 104, 117} }); // Floor
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-3.0f, 1.0f, -5.0f}, 0.5f, Ruby::Colour{11, 191, 77} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-2.0f, 0.25f, -6.5f}, 0.25f, Ruby::Colour{68, 70, 112} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.5f, 0.5f, -5.0f}, 1.0f, Ruby::Colour{74, 67, 16} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-1.0f, 0.75f, -11.0f}, 1.75f, Ruby::Colour{114, 158, 101} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{2.25f, 1.5f, -6.5f}, 0.5f, Ruby::Colour{114, 112, 130} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{4.0f, 0.0f, -8.0f}, 1.75f, Ruby::Colour{26, 3, 24} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-3.0f, 2.5f, -7.0f}, 1.0f, Ruby::Colour{181, 101, 112} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.0f, 2.5f, -4.0f}, 0.75f, Ruby::Colour{43, 32, 34} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-3.0f, 3.0f, -7.0f}, 1.5f, Ruby::Colour{56, 15, 92} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{3.5f, 0.5f, -12.0f}, 1.0f, Ruby::Colour{133, 105, 224} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-4.0f, 0.25f, -4.0f}, 0.5f, Ruby::Colour{13, 5, 38} });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.0f, 0.75f, -6.5f}, 0.25f, Ruby::Colour{14, 38, 5} });

		scene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });
		return scene;
	}

	Scene Scenes::randomSpheresInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, -100.5f, 0.0f}, 100.0f, Ruby::Colour::pink }); // Floor
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.0f, 0.0f, -4.0f}, 0.75f, Ruby::Colour::blue });

		scene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });
		return scene;
	}

	void Scenes::regenerateRandomSpheres() {
		randomSpheres = randomSpheresInit();
	}

}