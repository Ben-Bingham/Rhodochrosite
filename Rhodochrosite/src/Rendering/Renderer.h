#pragma once

#include "Camera.h"
#include "Image.h"
#include "Scene.h"
#include "Sphere.h"
#include "Vector.h"

namespace Rhodochrosite {
	enum class RenderingDevice {
		GPU,
		CPU
	};

	enum class RenderingAlgorithm {
		BASIC_LIGHTING,
		ALL_DIFFUSE,
		ALL_REFLECTIVE,
		RANDOM_MATERIALS
	};

	enum class SceneName {
		ONE_SPHERE,
		SPHERE_ON_PLANE,
		TWO_SPHERE,
		LARGE_AMOUNT_OF_SPHERES,
		RANDOM_SPHERES
	};

	class Renderer {
	public:
		Renderer(unsigned int width, unsigned int height, Ruby::Camera& camera);

		void render();
		Ruby::Image& getImage() { return m_RenderImage; }

		void setScene(const Scene& scene);
		void setAlgorithm(Ruby::Colour(Renderer::* algorithm)(const Malachite::Vector2f& texCords) const);

		[[nodiscard]] Ruby::Colour basicLightingAlgorithm(const Malachite::Vector2f& texCords) const;
		[[nodiscard]] Ruby::Colour allReflectiveAlgorithm(const Malachite::Vector2f& texCords) const;
		[[nodiscard]] Ruby::Colour allDiffuseAlgorithm(const Malachite::Vector2f& texCords) const;

	private:

		Ruby::Image m_RenderImage;
		unsigned int m_Width;
		unsigned int m_Height;

		Ruby::Camera& m_Camera;

		Scene m_Scene;

		Ruby::Colour(Renderer::* m_PerPixelAlgorithm)(const Malachite::Vector2f& texCords) const;
	};
}