#include "Renderer.h"

#include <vector>

namespace Rhodochrosite {
	Renderer::Renderer(unsigned int width, unsigned int height)
		: m_RenderImage(Malachite::Vector4f{ 1.0f }, width, height), m_Width(width), m_Height(height) {

	}

	void Renderer::render() {
		std::vector<unsigned char>& content = m_RenderImage.getContent();
		const unsigned int maxSize = m_Width * m_Height;
		for (unsigned int i = 0; i < maxSize; i++) {
			std::array<unsigned char, 4> pixelData = perPixelCalculations(Malachite::Vector2ui{ i % m_Width, i / m_Width });
			for (unsigned int c = 0; c < 4; c++) {
				content[i * 4 + c] = pixelData[c];
			}
		}
	}

	[[nodiscard]] std::array<unsigned char, 4> Renderer::perPixelCalculations(const Malachite::Vector2ui& texCords) const {
		const auto r = static_cast<unsigned char>(static_cast<float>(texCords.x) / m_Width * 255.0f);
		const auto g = static_cast<unsigned char>(static_cast<float>(texCords.y) / m_Height * 255.0f);
		return std::array<unsigned char, 4>{
			r,
			g,
			0,
			255
		};
	}
}