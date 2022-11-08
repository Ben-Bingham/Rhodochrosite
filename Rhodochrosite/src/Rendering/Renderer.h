#pragma once

#include "Camera.h"
#include "Image.h"
#include "Sphere.h"
#include "Vector.h"

namespace Rhodochrosite {
	class Renderer {
	public:
		Renderer(unsigned int width, unsigned int height, Ruby::Camera& camera);

		void render();
		Ruby::Image& getImage() { return m_RenderImage; }

		void addSphere(const Sphere& sphere);

	private:
		[[nodiscard]] Ruby::Colour perPixelCalculations(const Malachite::Vector2f& texCords) const;

		Ruby::Image m_RenderImage;
		unsigned int m_Width;
		unsigned int m_Height;

		Ruby::Camera& m_Camera;

		std::vector<Sphere> m_Spheres;
	};
}