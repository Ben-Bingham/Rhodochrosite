#include "Camera.h"
#include "Window.h"
#include "Timing.h"
#include "Renderer.h"
#include "Scene.h"
#include "Scenes.h"

#include "Rendering/Renderer.h"
#include <wtypes.h>

#include "Materials/ScreenMaterial.h"
#include "Geometry/PlaneGeometryData.h"

#include "Rendering/Materials/RayTracingMaterial.h"

auto scene = Rhodochrosite::SceneName::ONE_SPHERE;
auto device = Rhodochrosite::RenderingDevice::CPU;
auto algorithm = Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING;
void setScene(Rhodochrosite::SceneName newScene);
void setAlgorithm(Rhodochrosite::RenderingAlgorithm newAlgorithm);

std::unique_ptr<Ruby::ShaderProgram> basicLighting{ nullptr };
std::unique_ptr<Ruby::ShaderProgram> allReflective{ nullptr };
std::unique_ptr<Ruby::ShaderProgram> allDiffuse{ nullptr };
std::unique_ptr<Ruby::ShaderProgram> randomMaterials{ nullptr };

Rhodochrosite::RayTracingMaterial* activeMaterial{ nullptr };
std::unique_ptr<Rhodochrosite::RayTracingMaterial> basicLightingMaterial{ nullptr };
std::unique_ptr<Rhodochrosite::RayTracingMaterial> allReflectiveMaterial{ nullptr };
std::unique_ptr<Rhodochrosite::RayTracingMaterial> allDiffuseMaterial{ nullptr };
std::unique_ptr<Rhodochrosite::RayTracingMaterial> randomMaterialsMaterial{ nullptr };

std::unique_ptr<Rhodochrosite::Renderer> rayTracer{ nullptr };

Rhodochrosite::Scenes sceneCollection;

// Camera stuff
Ruby::Camera camera{};
struct FPSController {
	bool firstMouse = true;
	int lastX = 0;
	int lastY = 0;
	float mouseSensitivity = 0.1f;
	Malachite::Degree yaw = -90.0f;
	Malachite::Degree pitch;
	Wavellite::Mouse* mouse{ nullptr };
};
FPSController fpsController{ };
bool cursorNormal = false;

void mousePositionCallback(int xpos, int ypos, void* data) {
	FPSController* controller = (FPSController*)data;

	if (controller->firstMouse) {
		controller->lastX = (int)xpos;
		controller->lastY = (int)ypos;
		controller->firstMouse = false;
	}

	float xOffset = (float)(xpos - controller->lastX);
	float yOffset = (float)(controller->lastY - ypos);
	controller->lastX = (int)xpos;
	controller->lastY = (int)ypos;

	xOffset *= controller->mouseSensitivity;
	yOffset *= controller->mouseSensitivity;

	if (controller->mouse != nullptr && controller->mouse->button2 && device == Rhodochrosite::RenderingDevice::GPU) {
		controller->yaw += xOffset;
		controller->pitch += yOffset;

		if (controller->pitch > 89.0f)
			controller->pitch = 89.0f;
		if (controller->pitch < -89.0f)
			controller->pitch = -89.0f;

		Malachite::Vector3f direction;
		direction.x = cos(Malachite::degreesToRadians(controller->yaw)) * cos(Malachite::degreesToRadians(controller->pitch));
		direction.y = sin(Malachite::degreesToRadians(controller->pitch));
		direction.z = sin(Malachite::degreesToRadians(controller->yaw)) * cos(Malachite::degreesToRadians(controller->pitch));

		camera.front = direction.normalize();
		camera.updateCameraVectors();
	}
}

// Switch to most powerful GPU
#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

bool sceneRendered = false;

int main() {
	// Quad Rendering Setup
	Wavellite::Window window{Wavellite::Window::WindowSize::HALF_SCREEN, "Rhodochrosite"};
	window.setSwapInterval(0);
	Wavellite::Keyboard& keyboard = window.ioManger.getKeyboard();
	Wavellite::Mouse& mouse = window.ioManger.getMouse();
	fpsController.mouse = &mouse;

	mouse.addMousePositionCallback(mousePositionCallback, (void*)&fpsController);

	Ruby::Camera cam{};
	Ruby::Renderer renderer{ cam, window };
	Wavellite::Time time{};

	// Scene Setup
	sceneCollection = Rhodochrosite::Scenes{};

	// Ray tracing Setup
	rayTracer = std::make_unique<Rhodochrosite::Renderer>( window.getWidth(), window.getHeight(), camera );

	// Shader setup
	basicLighting = std::make_unique<Ruby::ShaderProgram>(
		Ruby::VertexShader{
			Ruby::TextFile{ "assets\\shaders\\BasicLighting.vert"},
			Ruby::VertexShader::LayoutData{
				Ruby::VertexShader::LayoutDataElement{
					Ruby::VertexShader::LayoutDataElement::DataType::VECTOR_3F,
					Ruby::VertexShader::LayoutDataElement::DataName::POSITION
				},
				Ruby::VertexShader::LayoutDataElement{
					Ruby::VertexShader::LayoutDataElement::DataType::VECTOR_2F,
					Ruby::VertexShader::LayoutDataElement::DataName::TEXTURE_COORDINATES
				}
			}
		},
		Ruby::FragmentShader{ Ruby::TextFile{ "assets\\shaders\\BasicLighting.frag" } }
	);
	allReflective = std::make_unique<Ruby::ShaderProgram>(
		Ruby::VertexShader{
			Ruby::TextFile{ "assets\\shaders\\AllReflective.vert" },
			Ruby::VertexShader::LayoutData{
				Ruby::VertexShader::LayoutDataElement{
					Ruby::VertexShader::LayoutDataElement::DataType::VECTOR_3F,
					Ruby::VertexShader::LayoutDataElement::DataName::POSITION
				},
				Ruby::VertexShader::LayoutDataElement{
					Ruby::VertexShader::LayoutDataElement::DataType::VECTOR_2F,
					Ruby::VertexShader::LayoutDataElement::DataName::TEXTURE_COORDINATES
				}
			}
		},
		Ruby::FragmentShader{ Ruby::TextFile{ "assets\\shaders\\AllReflective.frag" } }
	);
	allDiffuse = std::make_unique<Ruby::ShaderProgram>(
		Ruby::VertexShader{
			Ruby::TextFile{ "assets\\shaders\\AllDiffuse.vert" },
			Ruby::VertexShader::LayoutData{
				Ruby::VertexShader::LayoutDataElement{
					Ruby::VertexShader::LayoutDataElement::DataType::VECTOR_3F,
					Ruby::VertexShader::LayoutDataElement::DataName::POSITION
				},
				Ruby::VertexShader::LayoutDataElement{
					Ruby::VertexShader::LayoutDataElement::DataType::VECTOR_2F,
					Ruby::VertexShader::LayoutDataElement::DataName::TEXTURE_COORDINATES
				}
			}
		},
		Ruby::FragmentShader{ Ruby::TextFile{ "assets\\shaders\\AllDiffuse.frag" } }
	);
	randomMaterials = std::make_unique<Ruby::ShaderProgram>(
		Ruby::VertexShader{
			Ruby::TextFile{ "assets\\shaders\\Default.vert" },
			Ruby::VertexShader::LayoutData{
				Ruby::VertexShader::LayoutDataElement{
					Ruby::VertexShader::LayoutDataElement::DataType::VECTOR_3F,
					Ruby::VertexShader::LayoutDataElement::DataName::POSITION
				},
				Ruby::VertexShader::LayoutDataElement{
					Ruby::VertexShader::LayoutDataElement::DataType::VECTOR_2F,
					Ruby::VertexShader::LayoutDataElement::DataName::TEXTURE_COORDINATES
				}
			}
		},
		Ruby::FragmentShader{ Ruby::TextFile{ "assets\\shaders\\Default.frag" } }
	);

	allDiffuseMaterial = std::make_unique<Rhodochrosite::RayTracingMaterial>(*allDiffuse);
	basicLightingMaterial = std::make_unique<Rhodochrosite::RayTracingMaterial>(*basicLighting);
	allReflectiveMaterial = std::make_unique<Rhodochrosite::RayTracingMaterial>(*allReflective);
	randomMaterialsMaterial = std::make_unique<Rhodochrosite::RayTracingMaterial>(*randomMaterials);

	Ruby::Texture renderTarget{ rayTracer->getImage() };

	Ruby::PlaneGeometryData planeGeoData{};
	Ruby::ScreenMaterial screenQuadMaterial{ renderTarget };
	Ruby::Renderable screenRenderable{ planeGeoData, screenQuadMaterial };

	// Default values
	device = Rhodochrosite::RenderingDevice::CPU;
	setAlgorithm(Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING);
	setScene(Rhodochrosite::SceneName::ONE_SPHERE);

	float temp = 0.5f;

	while (window.isOpen()) {
		if (keyboard.KEY_ESCAPE) {
			window.close();
		}
		
		{ // Ray tracing
			renderer.beginFrame();
			renderer.render(screenRenderable);
			switch (device) {
			case Rhodochrosite::RenderingDevice::CPU:
				if (!sceneRendered) {
					rayTracer->render();
					renderTarget.updateData();
					sceneRendered = true;
				}
				screenRenderable.setMaterial(screenQuadMaterial);

				break;
			case Rhodochrosite::RenderingDevice::GPU:
				Rhodochrosite::RayTracingMaterial::aspectRatio = (float)window.getHeight() / (float)window.getWidth();
				Rhodochrosite::RayTracingMaterial::cameraPosition = camera.position;
				Rhodochrosite::RayTracingMaterial::cameraDirection = camera.front.normalize();
				Rhodochrosite::RayTracingMaterial::pixelWidth = (int)rayTracer->getImage().getWidth();
				Rhodochrosite::RayTracingMaterial::pixelHeight = (int)rayTracer->getImage().getHeight();
				Rhodochrosite::RayTracingMaterial::time = time.getTime();

				screenRenderable.setMaterial(*activeMaterial);

				break;
			}
			renderer.render(screenRenderable);
		}

		{ // Camera Movement
			if (mouse.button2 && device == Rhodochrosite::RenderingDevice::GPU) {
				window.disableCursor();
				const float velocity = 5.0f * time.deltaTime;
				if (keyboard.KEY_W) { camera.position = camera.position + (velocity * camera.front); }
				if (keyboard.KEY_S) { camera.position = camera.position + (velocity * -camera.front); }
				if (keyboard.KEY_A) { camera.position = camera.position + (velocity * -camera.right); }
				if (keyboard.KEY_D) { camera.position = camera.position + (velocity * camera.right); }
				if (keyboard.KEY_SPACE) { camera.position = camera.position + (velocity * Malachite::Vector3f::up); }
				if (keyboard.KEY_LEFT_SHIFT) { camera.position = camera.position + (velocity * Malachite::Vector3f::down); }
			}
			else {
				window.enableCursor();
			}
		}

		{ // Rendering
			{
				renderer.imGuiPrep();

				{
					ImGui::Begin("Settings");

					// Status message
					std::string status;
					{
						switch (scene) {
						case Rhodochrosite::SceneName::ONE_SPHERE:
							status += "Currently rendering one sphere ";
							break;
						case Rhodochrosite::SceneName::SPHERE_ON_PLANE:
							status += "Currently rendering a sphere on a plane ";
							break;
						case Rhodochrosite::SceneName::TWO_SPHERE:
							status += "Currently rendering two spheres ";
							break;
						case Rhodochrosite::SceneName::LARGE_AMOUNT_OF_SPHERES:
							status += "Currently rendering a lot of spheres ";
							break;
						case Rhodochrosite::SceneName::RANDOM_SPHERES:
							status += "Currently rendering a random collection of spheres ";
							break;
						}

						switch (device) {
						case Rhodochrosite::RenderingDevice::CPU:
							status += "on the CPU ";
							break;

						case Rhodochrosite::RenderingDevice::GPU:
							status += "on the GPU ";
							break;
						}

						switch (algorithm) {
						case Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING:
							status += "with the basic lighting algorithm.";
							break;
						case Rhodochrosite::RenderingAlgorithm::ALL_REFLECTIVE:
							status += "with the reflection algorithm.";
							break;
						case Rhodochrosite::RenderingAlgorithm::RANDOM_MATERIALS:
							status += "with the random materials algorithm.";
							break;
						case Rhodochrosite::RenderingAlgorithm::ALL_DIFFUSE:
							status += "with the diffuse algorithm.";
							break;
						}
					}
					ImGui::Text(status.c_str());

					ImGui::Text("Rendering Device:");
					if (ImGui::Button("CPU Rendering")) {
						device = Rhodochrosite::RenderingDevice::CPU;
						sceneRendered = false;
					}
					if (ImGui::Button("GPU Rendering")) {
						device = Rhodochrosite::RenderingDevice::GPU;
						sceneRendered = false;
					}

					ImGui::Text("Rendering Algorithm");
					if (ImGui::Button("Basic Lighting")) {
						setAlgorithm(Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING);
						sceneRendered = false;
					}
					if (device == Rhodochrosite::RenderingDevice::GPU && ImGui::Button("Diffuse")) {
						setAlgorithm(Rhodochrosite::RenderingAlgorithm::ALL_DIFFUSE);
						sceneRendered = false;
					}
					if (device == Rhodochrosite::RenderingDevice::GPU && ImGui::Button("Reflective")) {
						setAlgorithm(Rhodochrosite::RenderingAlgorithm::ALL_REFLECTIVE);
						sceneRendered = false;
					}
					if (device == Rhodochrosite::RenderingDevice::GPU && ImGui::Button("Random Materials")) {
						setAlgorithm(Rhodochrosite::RenderingAlgorithm::RANDOM_MATERIALS);
						sceneRendered = false;
					}

					ImGui::Text("Scene:");
					if (ImGui::Button("One Sphere")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::ONE_SPHERE);
						sceneRendered = false;
					}
					if (ImGui::Button("Sphere On Plane")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::SPHERE_ON_PLANE);
						sceneRendered = false;
					}
					if (ImGui::Button("Two Spheres")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::TWO_SPHERE);
						sceneRendered = false;
					}
					if (ImGui::Button("Lots of Spheres")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::LARGE_AMOUNT_OF_SPHERES);
						sceneRendered = false;
					}
					if (ImGui::Button("Random Spheres")) {
						setAlgorithm(algorithm);
						sceneCollection.regenerateRandomSpheres();
						setScene(Rhodochrosite::SceneName::RANDOM_SPHERES);
						sceneRendered = false;
					}

					ImGui::Text("Frame Time: ");
					ImGui::Text((std::to_string(time.deltaTime * 1000.0f) + "miliseconds").c_str());

					ImGui::SliderFloat("Temp", &temp, -1.0f, 1.0f);

					ImGui::End();
				}

				renderer.imGuiEnd();
			}
			renderer.endFrame();
		}

		window.pollEvents();
		window.swapBuffers();
		time.endFrame();
	}
}

void uploadSphere(const Rhodochrosite::Sphere& sphere, unsigned int index) {
	Ruby::ShaderProgram::upload("spheres[" + std::to_string(index) + "].origin", sphere.origin);
	Ruby::ShaderProgram::upload("spheres[" + std::to_string(index) + "].radius", sphere.radius);
	Ruby::ShaderProgram::upload("spheres[" + std::to_string(index) + "].colour", sphere.colour.colour);

	unsigned int mat = 0;
	switch (sphere.material) {
	default:
	case Rhodochrosite::Material::DIFFUSE:
		mat = 0;
		break;
	case Rhodochrosite::Material::REFLECTION:
		mat = 1;
		break;
	case Rhodochrosite::Material::REFRACTION:
		mat = 2;
		break;
	}

	Ruby::ShaderProgram::upload("spheres[" + std::to_string(index) + "].material", (int)mat);
}

void setScene(const Rhodochrosite::SceneName newScene) {
	scene = newScene;
	switch (scene) {
	case Rhodochrosite::SceneName::ONE_SPHERE:
		// CPU side
		rayTracer->setScene(sceneCollection.oneSphere);

		// GPU side
		Rhodochrosite::RayTracingMaterial::spheres = sceneCollection.oneSphere.spheres;
		Rhodochrosite::RayTracingMaterial::dirLights = sceneCollection.oneSphere.lights;
		break;
	case Rhodochrosite::SceneName::SPHERE_ON_PLANE:
		// CPU side
		rayTracer->setScene(sceneCollection.sphereOnPlane);

		// GPU side
		Rhodochrosite::RayTracingMaterial::spheres = sceneCollection.sphereOnPlane.spheres;
		Rhodochrosite::RayTracingMaterial::dirLights = sceneCollection.sphereOnPlane.lights;
		break;
	case Rhodochrosite::SceneName::TWO_SPHERE:
		// CPU side
		rayTracer->setScene(sceneCollection.twoSpheres);

		// GPU side
		Rhodochrosite::RayTracingMaterial::spheres = sceneCollection.twoSpheres.spheres;
		Rhodochrosite::RayTracingMaterial::dirLights = sceneCollection.twoSpheres.lights;
		break;
	case Rhodochrosite::SceneName::LARGE_AMOUNT_OF_SPHERES:
		// CPU side
		rayTracer->setScene(sceneCollection.lotsOfSpheres);

		// GPU side
		Rhodochrosite::RayTracingMaterial::spheres = sceneCollection.lotsOfSpheres.spheres;
		Rhodochrosite::RayTracingMaterial::dirLights = sceneCollection.lotsOfSpheres.lights;
		break;
	case Rhodochrosite::SceneName::RANDOM_SPHERES:
		// CPU side
		rayTracer->setScene(sceneCollection.randomSpheres);

		// GPU side
		Rhodochrosite::RayTracingMaterial::spheres = sceneCollection.randomSpheres.spheres;
		Rhodochrosite::RayTracingMaterial::dirLights = sceneCollection.randomSpheres.lights;
		break;
	}
}

void setAlgorithm(Rhodochrosite::RenderingAlgorithm newAlgorithm) {
	algorithm = newAlgorithm;
	Ruby::Colour(Rhodochrosite::Renderer::* cpuAlg)(const Malachite::Vector2f & texCords) const;
	std::string vertPath = "";
	std::string fragPath = "";

	switch (algorithm) {
	case Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING:
		// CPU side
		cpuAlg = &Rhodochrosite::Renderer::basicLightingAlgorithm;

		// GPU side
		activeMaterial = basicLightingMaterial.get();
		break;
	case Rhodochrosite::RenderingAlgorithm::ALL_REFLECTIVE:
		// CPU side
		cpuAlg = &Rhodochrosite::Renderer::allReflectiveAlgorithm;

		// GPU side
		activeMaterial = allReflectiveMaterial.get();
		break;
	case Rhodochrosite::RenderingAlgorithm::ALL_DIFFUSE:
		// CPU side
		cpuAlg = &Rhodochrosite::Renderer::allDiffuseAlgorithm;

		// GPU side
		activeMaterial = allDiffuseMaterial.get();
		break;
	case Rhodochrosite::RenderingAlgorithm::RANDOM_MATERIALS:
		// CPU side
		cpuAlg = &Rhodochrosite::Renderer::randomMaterialsAlgorithm;

		// GPU side
		activeMaterial = randomMaterialsMaterial.get();
		break;
	}
	rayTracer->setAlgorithm(cpuAlg);
	setScene(scene);
}