#include "Camera.h"
#include "Window.h"
#include "Timing.h"
#include "Renderer.h"
#include "Scene.h"
#include "Scenes.h"

#include "Rendering/Renderer.h"
#include "Renderable Objects/ScreenQuad/ScreenQuad.h"

auto scene = Rhodochrosite::SceneName::ONE_SPHERE;
auto device = Rhodochrosite::RenderingDevice::CPU;
auto algorithm = Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING;
void setScene(Rhodochrosite::SceneName newScene);
void setAlgorithm(Rhodochrosite::RenderingAlgorithm newAlgorithm);

Ruby::ShaderProgram* activeProgram{ nullptr };
std::unique_ptr<Ruby::ShaderProgram> basicLighting{ nullptr };
std::unique_ptr<Ruby::ShaderProgram> allReflective{ nullptr };
std::unique_ptr<Ruby::ShaderProgram> allDiffuse{ nullptr };

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

int main() {
	// Quad Rendering Setup
	Wavellite::Window window{Wavellite::Window::WindowSize::HALF_SCREEN, "Rhodochrosite"};
	window.setSwapInterval(0);
	Wavellite::Keyboard* const keyboard = window.ioManger.getKeyboard();
	Wavellite::Mouse* const mouse = window.ioManger.getMouse();
	fpsController.mouse = mouse;

	mouse->addMousePositionCallback(mousePositionCallback, (void*)&fpsController);

	Ruby::Renderer renderer{};
	Ruby::Camera cam{};
	Wavellite::Time time{};

	// Scene Setup
	sceneCollection = Rhodochrosite::Scenes{};

	// Ray tracing Setup
	rayTracer = std::make_unique<Rhodochrosite::Renderer>( window.getWidth(), window.getHeight(), camera );

	// Shader setup
	basicLighting = std::make_unique<Ruby::ShaderProgram>(
	Ruby::VertexShader{ Ruby::TextFile{ "assets\\shaders\\BasicLighting.vert" } },
	Ruby::FragmentShader{ Ruby::TextFile{ "assets\\shaders\\BasicLighting.frag" } },
	std::vector<Ruby::Attribute>{ 3, 2 }
	);
	allReflective = std::make_unique<Ruby::ShaderProgram>(
		Ruby::VertexShader{ Ruby::TextFile{ "assets\\shaders\\AllReflective.vert" } },
		Ruby::FragmentShader{ Ruby::TextFile{ "assets\\shaders\\AllReflective.frag" } },
		std::vector<Ruby::Attribute>{ 3, 2 }
	);
	allDiffuse = std::make_unique<Ruby::ShaderProgram>(
		Ruby::VertexShader{ Ruby::TextFile{ "assets\\shaders\\AllDiffuse.vert" } },
		Ruby::FragmentShader{ Ruby::TextFile{ "assets\\shaders\\AllDiffuse.frag" } },
		std::vector<Ruby::Attribute>{ 3, 2 }
	);

	Ruby::Texture renderTarget{ rayTracer->getImage() };
	Ruby::ScreenQuad screenQuad{ &renderTarget };

	// Default values
	device = Rhodochrosite::RenderingDevice::CPU;
	setAlgorithm(Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING);
	setScene(Rhodochrosite::SceneName::ONE_SPHERE);

	renderer.init(window.getProjectionMatrix());

	while (window.isOpen()) {
		if (keyboard->KEY_ESCAPE) {
			window.close();
		}

		{ // Ray tracing
			renderer.prep(cam.getViewMatrix());

			switch (device) {
			case Rhodochrosite::RenderingDevice::CPU:
				rayTracer->render();
				renderTarget.updateData();

				renderer.screenQuadRenderingPrep();

				renderer.screenQuadRender(screenQuad);

				renderer.screenQuadRenderingEnd();

				break;
			case Rhodochrosite::RenderingDevice::GPU:
				activeProgram->use();
				Ruby::ShaderProgram::upload("aspectRatio", (float)window.getHeight() / (float)window.getWidth());
				Ruby::ShaderProgram::upload("cameraPosition", camera.position);
				Ruby::ShaderProgram::upload("cameraDirection", camera.front.normalize());
				Ruby::ShaderProgram::upload("pixelWidth", (int)rayTracer->getImage().getWidth());
				Ruby::ShaderProgram::upload("pixelHeight", (int)rayTracer->getImage().getHeight());
				Ruby::ShaderProgram::upload("time", time.getTime());

				screenQuad.render();
				break;
			}
		}

		{ // Camera Movement
			if (mouse->button2 && device == Rhodochrosite::RenderingDevice::GPU) {
				window.disableCursor();
				const float velocity = 5.0f * time.deltaTime;
				if (keyboard->KEY_W) { camera.position = camera.position + (velocity * camera.front); }
				if (keyboard->KEY_S) { camera.position = camera.position + (velocity * -camera.front); }
				if (keyboard->KEY_A) { camera.position = camera.position + (velocity * -camera.right); }
				if (keyboard->KEY_D) { camera.position = camera.position + (velocity * camera.right); }
				if (keyboard->KEY_SPACE) { camera.position = camera.position + (velocity * Malachite::Vector3f::up); }
				if (keyboard->KEY_LEFT_SHIFT) { camera.position = camera.position + (velocity * Malachite::Vector3f::down); }
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
					}
					if (ImGui::Button("GPU Rendering")) {
						device = Rhodochrosite::RenderingDevice::GPU;
					}

					ImGui::Text("Rendering Algorithm");
					if (ImGui::Button("Basic Lighting")) {
						setAlgorithm(Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING);
					}
					if (ImGui::Button("Diffuse")) {
						setAlgorithm(Rhodochrosite::RenderingAlgorithm::ALL_DIFFUSE);
					}
					if (ImGui::Button("Reflective")) {
						setAlgorithm(Rhodochrosite::RenderingAlgorithm::ALL_REFLECTIVE);
					}
					if (ImGui::Button("Random Materials")) {
						setAlgorithm(Rhodochrosite::RenderingAlgorithm::RANDOM_MATERIALS);
					}

					ImGui::Text("Scene:");
					if (ImGui::Button("One Sphere")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::ONE_SPHERE);
					}
					if (ImGui::Button("Sphere On Plane")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::SPHERE_ON_PLANE);
					}
					if (ImGui::Button("Two Spheres")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::TWO_SPHERE);
					}
					if (ImGui::Button("Lots of Spheres")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::LARGE_AMOUNT_OF_SPHERES);
					}
					if (ImGui::Button("Random Spheres")) {
						setAlgorithm(algorithm);
						sceneCollection.regenerateRandomSpheres();
						setScene(Rhodochrosite::SceneName::RANDOM_SPHERES);
					}

					ImGui::Text("Frame Time: ");
					ImGui::Text((std::to_string(time.deltaTime * 1000.0f) + "miliseconds").c_str());

					ImGui::End();
				}

				renderer.imGuiEnd();
			}
			renderer.end();
		}

		window.pollEvents();
		window.swapBuffers();
		time.endFrame();
	}

	renderer.imGuiTerminate();
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

void uploadLight(const Ruby::DirectionalLight& light, unsigned int index) {
	Ruby::ShaderProgram::upload("dirLights[" + std::to_string(index) + "].direction", light.direction);
}

void uploadScene(const Rhodochrosite::Scene& scene) {
	Ruby::ShaderProgram::upload("numberOfSpheres", (int)scene.spheres.size());
	for (unsigned int i = 0; i < scene.spheres.size(); i++) {
		uploadSphere(scene.spheres[i], i);
	}

	Ruby::ShaderProgram::upload("numberOfLights", (int)scene.lights.size());
	for (unsigned int i = 0; i < scene.lights.size(); i++) {
		uploadLight(scene.lights[i], i);
	}
}

void setScene(const Rhodochrosite::SceneName newScene) {
	scene = newScene;
	switch (scene) {
	case Rhodochrosite::SceneName::ONE_SPHERE:
		// CPU side
		rayTracer->setScene(sceneCollection.oneSphere);

		// GPU side
		activeProgram->use();
		uploadScene(sceneCollection.oneSphere);
		break;
	case Rhodochrosite::SceneName::SPHERE_ON_PLANE:
		// CPU side
		rayTracer->setScene(sceneCollection.sphereOnPlane);

		// GPU side
		activeProgram->use();
		uploadScene(sceneCollection.sphereOnPlane);
		break;
	case Rhodochrosite::SceneName::TWO_SPHERE:
		// CPU side
		rayTracer->setScene(sceneCollection.twoSpheres);

		// GPU side
		activeProgram->use();
		uploadScene(sceneCollection.twoSpheres);
		break;
	case Rhodochrosite::SceneName::LARGE_AMOUNT_OF_SPHERES:
		// CPU side
		rayTracer->setScene(sceneCollection.lotsOfSpheres);

		// GPU side
		activeProgram->use();
		uploadScene(sceneCollection.lotsOfSpheres);
		break;
	case Rhodochrosite::SceneName::RANDOM_SPHERES:
		// CPU side
		rayTracer->setScene(sceneCollection.randomSpheres);

		// GPU side
		activeProgram->use();
		uploadScene(sceneCollection.randomSpheres);
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
		activeProgram = basicLighting.get();
		break;
	case Rhodochrosite::RenderingAlgorithm::ALL_REFLECTIVE:
		// CPU side
		cpuAlg = &Rhodochrosite::Renderer::allReflectiveAlgorithm;

		// GPU side
		activeProgram = allReflective.get();
		break;
		/*case Rhodochrosite::RenderingAlgorithm::RANDOM_MATERIALS:
		break;*/
	case Rhodochrosite::RenderingAlgorithm::ALL_DIFFUSE:
		// CPU side
		cpuAlg = &Rhodochrosite::Renderer::allDiffuseAlgorithm;

		// GPU side
		activeProgram = allDiffuse.get();
		break;
	}
	rayTracer->setAlgorithm(cpuAlg);
	setScene(scene);
}