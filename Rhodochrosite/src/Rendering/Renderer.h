#pragma once

#include "Camera.h"
#include "Image.h"
#include "Lights.h"
#include "Sphere.h"
#include "Vector.h"

namespace Rhodochrosite {
	class Renderer {
	public:
		Renderer(unsigned int width, unsigned int height, Ruby::Camera& camera);

		void render();
		Ruby::Image& getImage() { return m_RenderImage; }

		//void addSphere(const Sphere& sphere);

		void setSphere(const Sphere& sphere);
		void setDirectionalLight(const Ruby::DirectionalLight dirLight);

	private:
		[[nodiscard]] Ruby::Colour perPixelCalculations(const Malachite::Vector2f& texCords) const;

		Ruby::Image m_RenderImage;
		unsigned int m_Width;
		unsigned int m_Height;

		Ruby::Camera& m_Camera;

		//std::vector<Sphere> m_Spheres;
		Sphere m_Sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink };
		Ruby::DirectionalLight m_DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() };
		//std::vector<Ruby::DirectionalLight> m_DirectionalLights;

		unsigned int m_NumberOfSamples = 2;
		unsigned int m_NumberOfBounces = 4;
	};
}