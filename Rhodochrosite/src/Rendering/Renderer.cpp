#include "Renderer.h"

#include <vector>

#include "Ray.h"
#include "Utility.h"

namespace Rhodochrosite {
	Renderer::Renderer(const unsigned int width, const unsigned int height, Ruby::Camera& camera)
		: m_RenderImage(Malachite::Vector4f{1.0f}, width, height)
		, m_Width(width)
		, m_Height(height)
		, m_Camera(camera)
		, m_PerPixelAlgorithm(&Renderer::basicLightingAlgorithm) { }

	void Renderer::render() {
		std::vector<unsigned char>& content = m_RenderImage.getContent();
		for (unsigned int y = 0; y < m_Height; y++) {
			for (unsigned int x = 0; x < m_Width; x++) {
				Malachite::Vector2f cord = {static_cast<float>(x) / static_cast<float>(m_Width), static_cast<float>(y) / static_cast<float>(m_Height)};
				cord.x = cord.x * 2.0f - 1.0f;
				cord.y = (cord.y * 2.0f - 1.0f) * (static_cast<float>(m_Height) / static_cast<float>(m_Width));

				Ruby::Colour pixelColour = (this->*m_PerPixelAlgorithm)(cord);

				const Malachite::Vector4uc colourData = pixelColour.toVec4();

				content[(x + y * m_Width) * 4 + 0] = colourData.x;
				content[(x + y * m_Width) * 4 + 1] = colourData.y;
				content[(x + y * m_Width) * 4 + 2] = colourData.z;
				content[(x + y * m_Width) * 4 + 3] = colourData.w;
			}
		}
	}

	void Renderer::setScene(const Scene& scene) {
		m_Scene = scene;
	}

	void Renderer::setAlgorithm(Ruby::Colour (Renderer::* algorithm)(const Malachite::Vector2f& texCords) const) {
		m_PerPixelAlgorithm = algorithm;
	}

	constexpr float infinity = std::numeric_limits<float>::max();

	[[nodiscard]] Ruby::Colour Renderer::basicLightingAlgorithm(const Malachite::Vector2f& texCords) const {
		const Ray ray{ Malachite::Vector3f::zero, Malachite::Vector3f{texCords.x, texCords.y, -1.f} };

		const Sphere* hitSphere{ nullptr };
		float closestHit= infinity;
		for (unsigned int i = 0; i < m_Scene.spheres.size(); i++) {
			// Discriminant calculations
			const float a = dot(ray.direction, ray.direction);
			const float b = 2.0f * dot(ray.origin - m_Scene.spheres[i].origin, ray.direction);
			const float c = dot(ray.origin - m_Scene.spheres[i].origin, ray.origin - m_Scene.spheres[i].origin) - (m_Scene.spheres[i].radius * m_Scene.spheres[i].radius);

			const float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f) {
				continue;
			}

			// Solving the quadratic formula
			const float hitDistance = (-b - std::sqrt(discriminant)) / 2.0f * a;
			if (hitDistance < 0.0f) {
				continue;
			}

			if (hitDistance < closestHit) {
				closestHit = hitDistance;
				hitSphere = &m_Scene.spheres[i];
			}
		}

		if (hitSphere == nullptr) {
			return Ruby::Colour::black;
		}

		const Malachite::Vector3f hitPosition = ray.at(closestHit);

		// Lighting Calculations
		Malachite::Vector3f normal = hitPosition - hitSphere->origin;
		normal = normal.normalize();

		float lightIntensity{ 0.0f };
		for (unsigned int i = 0; i < m_Scene.lights.size(); i++) {
			lightIntensity += Malachite::max(dot(normal, -m_Scene.lights[i].direction), 0.0f);
		}

		lightIntensity = Malachite::clamp(lightIntensity, 0.0f, 1.0f);

		const Malachite::Vector4f sphereColour = hitSphere->colour.colour * lightIntensity;
		return Ruby::Colour{sphereColour.x, sphereColour.y, sphereColour.z, 1.0f};
	}

	[[nodiscard]] Ruby::Colour Renderer::allReflectiveAlgorithm(const Malachite::Vector2f& texCords) const {
		const Ray ray{ Malachite::Vector3f::zero, Malachite::Vector3f{texCords.x, texCords.y, -1.f} };

		const Sphere* hitSphere{ nullptr };
		float closestHit = infinity;
		for (unsigned int i = 0; i < m_Scene.spheres.size(); i++) {
			// Discriminant calculations
			const float a = dot(ray.direction, ray.direction);
			const float b = 2.0f * dot(ray.origin - m_Scene.spheres[i].origin, ray.direction);
			const float c = dot(ray.origin - m_Scene.spheres[i].origin, ray.origin - m_Scene.spheres[i].origin) - (m_Scene.spheres[i].radius * m_Scene.spheres[i].radius);

			const float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f) {
				continue;
			}

			// Solving the quadratic formula
			const float hitDistance = (-b - std::sqrt(discriminant)) / 2.0f * a;
			if (hitDistance < 0.0f) {
				continue;
			}

			if (hitDistance < closestHit) {
				closestHit = hitDistance;
				hitSphere = &m_Scene.spheres[i];
			}
		}

		if (hitSphere == nullptr) {
			return Ruby::Colour::black;
		}

		const Malachite::Vector3f hitPosition = ray.at(closestHit);

		// Lighting Calculations
		Malachite::Vector3f normal = hitPosition - hitSphere->origin;
		normal = normal.normalize();

		float lightIntensity{ 0.0f };
		for (unsigned int i = 0; i < m_Scene.lights.size(); i++) {
			lightIntensity += Malachite::max(dot(normal, -m_Scene.lights[i].direction), 0.0f);
		}

		lightIntensity = Malachite::clamp(lightIntensity, 0.0f, 1.0f);

		const Malachite::Vector4f sphereColour = hitSphere->colour.colour * lightIntensity;
		return Ruby::Colour{ sphereColour.x, sphereColour.y, sphereColour.z, 1.0f };
	}

	[[nodiscard]] Ruby::Colour Renderer::allDiffuseAlgorithm(const Malachite::Vector2f& texCords) const {
		const Ray ray{ Malachite::Vector3f::zero, Malachite::Vector3f{texCords.x, texCords.y, -1.f} };

		const Sphere* hitSphere{ nullptr };
		float closestHit = infinity;
		for (unsigned int i = 0; i < m_Scene.spheres.size(); i++) {
			// Discriminant calculations
			const float a = dot(ray.direction, ray.direction);
			const float b = 2.0f * dot(ray.origin - m_Scene.spheres[i].origin, ray.direction);
			const float c = dot(ray.origin - m_Scene.spheres[i].origin, ray.origin - m_Scene.spheres[i].origin) - (m_Scene.spheres[i].radius * m_Scene.spheres[i].radius);

			const float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f) {
				continue;
			}

			// Solving the quadratic formula
			const float hitDistance = (-b - std::sqrt(discriminant)) / 2.0f * a;
			if (hitDistance < 0.0f) {
				continue;
			}

			if (hitDistance < closestHit) {
				closestHit = hitDistance;
				hitSphere = &m_Scene.spheres[i];
			}
		}

		if (hitSphere == nullptr) {
			return Ruby::Colour::black;
		}

		const Malachite::Vector3f hitPosition = ray.at(closestHit);

		// Lighting Calculations
		Malachite::Vector3f normal = hitPosition - hitSphere->origin;
		normal = normal.normalize();

		float lightIntensity{ 0.0f };
		for (unsigned int i = 0; i < m_Scene.lights.size(); i++) {
			lightIntensity += Malachite::max(dot(normal, -m_Scene.lights[i].direction), 0.0f);
		}

		lightIntensity = Malachite::clamp(lightIntensity, 0.0f, 1.0f);

		const Malachite::Vector4f sphereColour = hitSphere->colour.colour * lightIntensity;
		return Ruby::Colour{ sphereColour.x, sphereColour.y, sphereColour.z, 1.0f };
	}

	[[nodiscard]] Ruby::Colour Renderer::randomMaterialsAlgorithm(const Malachite::Vector2f& texCords) const {
		const Ray ray{ Malachite::Vector3f::zero, Malachite::Vector3f{texCords.x, texCords.y, -1.f} };

		const Sphere* hitSphere{ nullptr };
		float closestHit = infinity;
		for (unsigned int i = 0; i < m_Scene.spheres.size(); i++) {
			// Discriminant calculations
			const float a = dot(ray.direction, ray.direction);
			const float b = 2.0f * dot(ray.origin - m_Scene.spheres[i].origin, ray.direction);
			const float c = dot(ray.origin - m_Scene.spheres[i].origin, ray.origin - m_Scene.spheres[i].origin) - (m_Scene.spheres[i].radius * m_Scene.spheres[i].radius);

			const float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f) {
				continue;
			}

			// Solving the quadratic formula
			const float hitDistance = (-b - std::sqrt(discriminant)) / 2.0f * a;
			if (hitDistance < 0.0f) {
				continue;
			}

			if (hitDistance < closestHit) {
				closestHit = hitDistance;
				hitSphere = &m_Scene.spheres[i];
			}
		}

		if (hitSphere == nullptr) {
			return Ruby::Colour::black;
		}

		const Malachite::Vector3f hitPosition = ray.at(closestHit);

		// Lighting Calculations
		Malachite::Vector3f normal = hitPosition - hitSphere->origin;
		normal = normal.normalize();

		float lightIntensity{ 0.0f };
		for (unsigned int i = 0; i < m_Scene.lights.size(); i++) {
			lightIntensity += Malachite::max(dot(normal, -m_Scene.lights[i].direction), 0.0f);
		}

		lightIntensity = Malachite::clamp(lightIntensity, 0.0f, 1.0f);

		const Malachite::Vector4f sphereColour = hitSphere->colour.colour * lightIntensity;
		return Ruby::Colour{ sphereColour.x, sphereColour.y, sphereColour.z, 1.0f };
	}
}