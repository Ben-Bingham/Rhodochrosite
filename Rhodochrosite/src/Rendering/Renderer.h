#pragma once
#include <array>

#include "Image.h"
#include "Sphere.h"
#include "Vector.h"

namespace Rhodochrosite {
	class Renderer {
	public:
		Renderer(unsigned int width, unsigned int height);

		void render();
		Ruby::Image& getImage() { return m_RenderImage; }

		void addSphere(const Sphere& sphere);

	private:
		[[nodiscard]] Ruby::Colour perPixelCalculations(const Malachite::Vector2ui& texCords) const;

		Ruby::Image m_RenderImage;
		unsigned int m_Width;
		unsigned int m_Height;

		std::vector<Sphere> m_Spheres;
	};
}