#pragma once
#include <array>

#include "Image.h"
#include "Vector.h"

namespace Rhodochrosite {
	class Renderer {
	public:
		Renderer(unsigned int width, unsigned int height);

		void render();
		Ruby::Image& getImage() { return m_RenderImage; }

	private:
		[[nodiscard]] std::array<unsigned char, 4> perPixelCalculations(const Malachite::Vector2ui& texCords) const;

		Ruby::Image m_RenderImage;
		unsigned int m_Width;
		unsigned int m_Height;
	};
}