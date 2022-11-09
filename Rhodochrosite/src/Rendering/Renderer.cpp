#include "Renderer.h"

#include <vector>

#include "Random.h"
#include "Ray.h"
#include "Utility.h"

namespace Rhodochrosite {
	Renderer::Renderer(const unsigned int width, const unsigned int height, Ruby::Camera& camera)
		: m_RenderImage(Malachite::Vector4f{ 1.0f }, width, height), m_Width(width), m_Height(height), m_Camera(camera) {

	}

	void Renderer::render() {
		std::vector<unsigned char>& content = m_RenderImage.getContent();
		for (unsigned int y = 0; y < m_Height; y++) {
			for (unsigned int x = 0; x < m_Width; x++) {
				Malachite::Vector2f cord = { static_cast<float>(x) / static_cast<float>(m_Width), static_cast<float>(y) / static_cast<float>(m_Height) };
				cord.x = cord.x * 2.0f - 1.0f;
				cord.y = (cord.y * 2.0f - 1.0f) * (static_cast<float>(m_Height) / static_cast<float>(m_Width));

				Ruby::Colour pixelColour = perPixelCalculations(cord);

				const Malachite::Vector4uc colourData = pixelColour.toVec4();

				content[(x + y * m_Width) * 4 + 0] = colourData.x;
				content[(x + y * m_Width) * 4 + 1] = colourData.y;
				content[(x + y * m_Width) * 4 + 2] = colourData.z;
				content[(x + y * m_Width) * 4 + 3] = colourData.w;
			}
		}
	}

	void Renderer::setDirectionalLight(const Ruby::DirectionalLight dirLight) {
		m_DirectionalLight = dirLight;
	}

	void Renderer::setSphere(const Sphere& sphere) {
		m_Sphere = sphere;
	}


	/*void Renderer::addSphere(const Sphere& sphere) {
		m_Spheres.push_back(sphere);
	}*/

	/*void Renderer::addDirectionalLight(const Ruby::DirectionalLight& light) {
		m_DirectionalLights.push_back(light);
	}*/

	constexpr float infinity = std::numeric_limits<float>::max();

	[[nodiscard]] Ruby::Colour Renderer::perPixelCalculations(const Malachite::Vector2f& texCords) const {
		const Ray ray{ m_Camera.position, Malachite::Vector3f{texCords.x, texCords.y, -1.0f} };

		// Discriminant calculations
		const float a = dot(ray.direction, ray.direction);
		const float b = 2.0f * dot(ray.origin - m_Sphere.origin, ray.direction);
		const float c = dot(ray.origin - m_Sphere.origin, ray.origin - m_Sphere.origin) - (m_Sphere.radius * m_Sphere.radius);

		const float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f) {
			return Ruby::Colour::black;
		}

		// Solving the quadratic formula
		const float hitDistance = (-b - std::sqrt(discriminant)) / 2.0f * a;
		const Malachite::Vector3f hitPosition = ray.at(hitDistance);

		// Lighting Calculations
		Malachite::Vector3f normal = hitPosition - m_Sphere.origin;
		normal = normal.normalize();

		const float lightIntensity = Malachite::max(dot(normal, -m_DirectionalLight.direction), 0.0f);

		const Malachite::Vector4f sphereColour = m_Sphere.colour.colour * lightIntensity;
		return Ruby::Colour{ sphereColour.x, sphereColour.y, sphereColour.z, 1.0f };
	}
}