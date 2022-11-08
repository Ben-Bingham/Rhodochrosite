#include "Renderer.h"

#include <vector>

#include "Ray.h"

namespace Rhodochrosite {
	Renderer::Renderer(const unsigned int width, const unsigned int height, Ruby::Camera& camera)
		: m_RenderImage(Malachite::Vector4f{ 1.0f }, width, height), m_Width(width), m_Height(height), m_Camera(camera) {

	}

	void Renderer::render() {
		std::vector<unsigned char>& content = m_RenderImage.getContent();
		for (unsigned int y = 0; y < m_Height; y++) {
			for (unsigned int x = 0; x < m_Width; x++) {
				Malachite::Vector2f coord = { (float)x / (float)m_Width, (float)y / (float)m_Height };
				coord.x = coord.x * 2.0f - 1.0f;
				coord.y = (coord.y * 2.0f - 1.0f) * ((float)m_Height / (float)m_Width);
				Ruby::Colour pixelColour = perPixelCalculations(coord);
				const Malachite::Vector4uc colourData = pixelColour.toVec4();
				content[(x + y * m_Width) * 4 + 0] = colourData.x;
				content[(x + y * m_Width) * 4 + 1] = colourData.y;
				content[(x + y * m_Width) * 4 + 2] = colourData.z;
				content[(x + y * m_Width) * 4 + 3] = colourData.w;
			}
		}
	}

	void Renderer::addSphere(const Sphere& sphere) {
		m_Spheres.push_back(sphere);
	}

	constexpr float infinity = std::numeric_limits<float>::max();

	[[nodiscard]] Ruby::Colour Renderer::perPixelCalculations(const Malachite::Vector2f& texCords) const {
		const Ray ray{ m_Camera.position, Malachite::Vector3f{texCords.x, texCords.y, -1.0f} };

		float closestHit = infinity;
		const Sphere* hitSphere{ nullptr };
		for (const Sphere& sphere : m_Spheres) {
			const float distance = ray.hitSphere(sphere);
			if (distance == 0.0f) { continue; }

			if (distance < closestHit) {
				closestHit = distance;
				hitSphere = &sphere;
			}
		}

		if (!hitSphere) {
			return Ruby::Colour::blue;
		}
		return hitSphere->colour;
	}
}