#include "Scenes.h"

#include "Random.h"

namespace Rhodochrosite {
	Scenes::Scenes()
		: oneSphere(oneSphereInit())
		, sphereOnPlane(sphereOnPlaneInit())
		, twoSpheres(twoSpheresInit())
		, lotsOfSpheres(lotsOfSpheresInit())
		, randomSpheres(randomSpheresInit()) {

	}

	Scene Scenes::oneSphereInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink, Material::DIFFUSE });
		scene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });
		return scene;
	}

	Scene Scenes::sphereOnPlaneInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink, Material::DIFFUSE });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, -1000.5f, 0.0f}, 1000.0f, Ruby::Colour{88, 104, 117} }); // Floor
		return scene;
	}


	Scene Scenes::twoSpheresInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink, Material::DIFFUSE });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.0f, 0.0f, -4.0f}, 0.75f, Ruby::Colour::blue, Material::DIFFUSE });
		scene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });
		return scene;
	}

	Scene Scenes::lotsOfSpheresInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, -1000.5f, 0.0f}, 1000.0f, Ruby::Colour{88, 104, 117} }); // Floor
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-3.0f, 1.0f, -5.0f}, 0.5f, Ruby::Colour{11, 191, 77}, Material::REFLECTION });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-2.0f, 0.25f, -6.5f}, 0.25f, Ruby::Colour{68, 70, 112}, Material::DIFFUSE });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.5f, 0.5f, -5.0f}, 1.0f, Ruby::Colour{74, 67, 16}, Material::REFLECTION });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-1.0f, 0.75f, -11.0f}, 1.75f, Ruby::Colour{114, 158, 101}, Material::REFRACTION });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{2.25f, 1.5f, -6.5f}, 0.5f, Ruby::Colour{114, 112, 130}, Material::DIFFUSE });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{4.0f, 0.0f, -8.0f}, 1.75f, Ruby::Colour{26, 3, 24}, Material::REFLECTION });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.0f, 2.5f, -4.0f}, 0.75f, Ruby::Colour{43, 32, 34}, Material::DIFFUSE });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-3.0f, 3.0f, -7.0f}, 1.5f, Ruby::Colour{56, 15, 92}, Material::REFLECTION });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{3.5f, 0.5f, -12.0f}, 1.0f, Ruby::Colour{133, 105, 224}, Material::REFRACTION });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{-4.0f, 0.25f, -4.0f}, 0.5f, Ruby::Colour{13, 5, 38}, Material::DIFFUSE });
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{1.0f, 0.75f, -6.5f}, 0.25f, Ruby::Colour{14, 38, 5}, Material::REFLECTION });

		scene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });
		return scene;
	}

	Scene Scenes::randomSpheresInit() {
		Scene scene;
		scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{0.0f, -1000.5f, 0.0f}, 1000.0f, Ruby::Colour{88, 104, 117} }); // Floor

		Material mat = Material::DIFFUSE;
		const auto numberOfSpheres = Malachite::random<unsigned int>(20, 25);
		for (unsigned int i = 0; i < numberOfSpheres; i++) {
			const auto xPos = Malachite::random<float>(-5.0f, 5.0f);
			const auto yPos = Malachite::random<float>(-0.5f, 5.0f);
			const auto zPos = Malachite::random<float>(-0.5f, -20.0f);
			const auto radius = Malachite::random<float>(0.25f, 1.5f);
			auto colour = Malachite::random<float>(Malachite::Vector4f{ 0.0f }, Malachite::Vector4f{ 1.0f });
			colour.w = 1.0f;

			scene.spheres.emplace_back(Sphere{ Malachite::Vector3f{xPos, yPos, zPos}, radius, Ruby::Colour{colour}, mat });

			switch (mat) {
			default:
			case Material::DIFFUSE:
				mat = Material::REFLECTION;
				break;
			case Material::REFLECTION:
				mat = Material::REFRACTION;
				break;
			case Material::REFRACTION:
				mat = Material::DIFFUSE;
				break;
			}
		}

		scene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });
		return scene;
	}

	void Scenes::regenerateRandomSpheres() {
		randomSpheres = randomSpheresInit();
	}

}