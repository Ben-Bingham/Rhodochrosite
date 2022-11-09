#pragma once

#include "Camera.h"
#include "Image.h"
#include "Lights.h"
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
		TWO_SPHERE,
		LARGE_AMOUNT_OF_SPHERES
	};

	class Renderer {
	public:
		Renderer(unsigned int width, unsigned int height, Ruby::Camera& camera);

		void render();
		Ruby::Image& getImage() { return m_RenderImage; }

		void setScene(const Scene& scene);

	private:
		[[nodiscard]] Ruby::Colour perPixelCalculations(const Malachite::Vector2f& texCords) const;

		Ruby::Image m_RenderImage;
		unsigned int m_Width;
		unsigned int m_Height;

		Ruby::Camera& m_Camera;

		Scene m_Scene;

		unsigned int m_NumberOfSamples = 2;
		unsigned int m_NumberOfBounces = 4;
	};
}