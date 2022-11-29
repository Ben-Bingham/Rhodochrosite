#pragma once
#include "Sphere.h"

#include "Materials/Material.h"

#include "Shaders/Uniforms/Uniform.h"
#include "Shaders/Uniforms/UniformSet.h"

namespace Rhodochrosite {
	class RayTracingMaterial : public Ruby::Material {
	public:
		RayTracingMaterial(Ruby::ShaderProgram& program)
			: Material(program) {

		}

		void use(const Malachite::Matrix4f& model, const Malachite::Matrix4f& view, const Malachite::Matrix4f& projection) override {
			m_Program->use();
			m_Uniforms.upload();
		}

		static inline Malachite::Vector3f cameraPosition{ 0.0f };
		static inline Malachite::Vector3f cameraDirection{ 0.0f };
		static inline float aspectRatio{ 0.0f };
		static inline int pixelWidth{ 0 };
		static inline int pixelHeight{ 0 };
			   
		static inline float time{ 0.0f };
			  
		static inline std::vector<Sphere> spheres{ };
			   
		static inline std::vector<Ruby::DirectionalLight> dirLights{ };

	private:
		Ruby::UniformSet<
			Malachite::Vector3f, // Camera pos
			Malachite::Vector3f, // Camera direction
			float,				 // Aspect ration
			int,				 // Pixel Width
			int,				 // Pixel Height
			float,				 // Time
			std::vector<Sphere>, // Spheres
			std::vector<Ruby::DirectionalLight> // Directional Lights
		> m_Uniforms{
			Ruby::Uniform{"cameraPosition", cameraPosition},
			Ruby::Uniform{"cameraDirection", cameraDirection},
			Ruby::Uniform{"aspectRatio", aspectRatio},
			Ruby::Uniform{"pixelWidth", pixelWidth},
			Ruby::Uniform{"pixelHeight", pixelHeight},
			Ruby::Uniform{"time", time},
			Ruby::Uniform{"spheres", spheres},
			Ruby::Uniform{"dirLights", dirLights},
		};
	};
}

namespace ShaderProgramUploads {
	inline void upload(const std::string& variableName, const Rhodochrosite::Sphere& sphere) {
		Ruby::ShaderProgram::upload(variableName + ".origin", sphere.origin);
		Ruby::ShaderProgram::upload(variableName + ".radius", sphere.radius);
		Ruby::ShaderProgram::upload(variableName + ".colour", sphere.colour.colour);
		switch (sphere.material) {
		case Rhodochrosite::Material::REFLECTION:
			Ruby::ShaderProgram::upload(variableName + ".material", 1);
			break;
		case Rhodochrosite::Material::REFRACTION:
			Ruby::ShaderProgram::upload(variableName + ".material", 2);
			break;
		case Rhodochrosite::Material::DIFFUSE:
			Ruby::ShaderProgram::upload(variableName + ".material", 0);
			break;
		}
	}

	inline void upload(const std::string& variableName, const std::vector<Rhodochrosite::Sphere>& spheres) {
		Ruby::ShaderProgram::upload("numberOfSpheres", (int)spheres.size());
		unsigned int i{ 0 };
		for (Rhodochrosite::Sphere sphere : spheres) {
			upload(variableName + "[" + std::to_string(i) + "]", sphere);
			i++;
		}
	}
}