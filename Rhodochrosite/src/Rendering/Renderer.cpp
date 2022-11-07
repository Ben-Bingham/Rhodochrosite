#include "Renderer.h"

#include <vector>

#include "Ray.h"

namespace Rhodochrosite {
	Renderer::Renderer(unsigned int width, unsigned int height)
		: m_RenderImage(Malachite::Vector4f{ 1.0f }, width, height), m_Width(width), m_Height(height) {

	}

	void Renderer::render() {
		std::vector<unsigned char>& content = m_RenderImage.getContent();
		const unsigned int maxSize = m_Width * m_Height;
		for (unsigned int i = 0; i < maxSize; i++) {
			Ruby::Colour pixelColour = perPixelCalculations(Malachite::Vector2ui{ i % m_Width, i / m_Width });
			const Malachite::Vector4uc colourData = pixelColour.toVec4();
			content[i * 4 + 0] = colourData.x;
			content[i * 4 + 1] = colourData.y;
			content[i * 4 + 2] = colourData.z;
			content[i * 4 + 3] = colourData.w;

			/*for (unsigned int c = 0; c < 4; c++) {
				content[i * 4 + c] = pixelColour[c];
			}*/
		}
	}

	void Renderer::addSphere(const Sphere& sphere) {
		m_Spheres.push_back(sphere);
	}


	[[nodiscard]] Ruby::Colour Renderer::perPixelCalculations(const Malachite::Vector2ui& texCords) const {
		const float aspectRatio = static_cast<float>(m_Height) / m_Width;
		const float x = ((static_cast<float>(texCords.x) / m_Width) * 2) - 1;
		const float y = ((static_cast<float>(texCords.y) / m_Height) * (aspectRatio * 2)) - aspectRatio;
		constexpr float z = 0.0f;

		Ray ray{ Malachite::Vector3f{x, y, z}, Malachite::Vector3f::north };

		// (t * t) * (rayDir * rayDir) + 2 * t * rayDir * (rayOrigin - sphereOrigin) + ((rayOrigin - sphereOrigin) * (rayOrigin - sphereOrigin)) - (sphereRadius * sphereRadius)
		// a = (rayDir * rayDir)
		// b = 2 * rayDir * (rayOrigin - sphereOrigin)
		// c = ((rayOrigin - sphereOrigin) * (rayOrigin - sphereOrigin)) - (sphereRadius * sphereRadius)
		float closestHit = std::numeric_limits<float>::max();
		const Sphere* hitSphere{ nullptr };
		std::array<unsigned char, 4> colour;
		for (const Sphere& sphere : m_Spheres) {
			const Malachite::Vector3f distanceBetween = (ray.origin - sphere.origin);
			const float a = dot(ray.direction, ray.direction);
			const float b = 2.0f * dot(ray.direction, distanceBetween);
			const float c = dot(distanceBetween, distanceBetween) - (sphere.radius * sphere.radius);

			const float discriminant = (b * b) - (4.0f * a * c);
			if (discriminant == 0) {
				float solution = (-b + sqrt(discriminant)) / (2.0f * a);
				if (solution < closestHit) {
					closestHit = solution;
					hitSphere = &sphere;
				}
			}
			else if (discriminant > 0) {
				float solution1 = (-b + sqrt(discriminant)) / (2.0f * a);
				float solution2 = (-b - sqrt(discriminant)) / (2.0f * a);
				if (solution1 < closestHit) {
					closestHit = solution1;
					hitSphere = &sphere;
				}
				if (solution2 < closestHit) {
					closestHit = solution2;
					hitSphere = &sphere;
				}
			}
		}

		if (!hitSphere) {
			return Ruby::Colour::blue;
		}
		else {
			return hitSphere->colour;
		}
	}
}