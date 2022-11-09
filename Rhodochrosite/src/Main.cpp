#include "Camera.h"
#include "Window.h"
#include "Timing.h"
#include "Renderer.h"
#include "Scene.h"

#include "Rendering/Renderer.h"
#include "Renderable Objects/ScreenQuad/ScreenQuad.h"

auto scene = Rhodochrosite::SceneName::ONE_SPHERE;
auto device = Rhodochrosite::RenderingDevice::CPU;
auto algorithm = Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING;
void setScene(Rhodochrosite::SceneName newScene);
void setAlgorithm(Rhodochrosite::RenderingAlgorithm newAlgorithm);

std::unique_ptr<Ruby::ShaderProgram> activeProgram{ nullptr };
std::unique_ptr<Rhodochrosite::Renderer> rayTracer{ nullptr };

Rhodochrosite::Scene oneSphereScene;

int main() {
	// Quad Rendering Setup
	Wavellite::Window window{Wavellite::Window::WindowSize::HALF_SCREEN, "Rhodochrosite"};
	window.setSwapInterval(0);
	const Wavellite::Keyboard* keyboard = window.ioManger.getKeyboard();

	Ruby::Renderer renderer{};
	Ruby::Camera cam{};
	Wavellite::Time time{};

	// Scene Setup
	oneSphereScene.spheres.emplace_back(Rhodochrosite::Sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink });
	oneSphereScene.lights.emplace_back(Ruby::DirectionalLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() });

	// Shader Setup
		//TODO

	// Ray tracing Setup
	Ruby::Camera camera{};
	rayTracer = std::make_unique<Rhodochrosite::Renderer>( window.getWidth(), window.getHeight(), camera );

	device = Rhodochrosite::RenderingDevice::CPU;
	setAlgorithm(Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING);
	setScene(Rhodochrosite::SceneName::ONE_SPHERE);

	Ruby::Texture renderTarget{ rayTracer->getImage()};
	Ruby::ScreenQuad screenQuad{ &renderTarget };

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

				screenQuad.render();
				break;
			}
		}

		{ // Camera Movement
			/*const float velocity = 5.0f * time.deltaTime;
			if (keyboard->KEY_W) { camera.position = camera.position + (velocity * Malachite::Vector3f::north); }
			if (keyboard->KEY_S) { camera.position = camera.position + (velocity * Malachite::Vector3f::south); }
			if (keyboard->KEY_A) { camera.position = camera.position + (velocity * Malachite::Vector3f::west); }
			if (keyboard->KEY_D) { camera.position = camera.position + (velocity * Malachite::Vector3f::east); }
			if (keyboard->KEY_SPACE) { camera.position = camera.position + (velocity * Malachite::Vector3f::up); }
			if (keyboard->KEY_LEFT_SHIFT) { camera.position = camera.position + (velocity * Malachite::Vector3f::down); }*/
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
						case Rhodochrosite::SceneName::TWO_SPHERE:
							status += "Currently rendering two spheres ";
							break;
						case Rhodochrosite::SceneName::LARGE_AMOUNT_OF_SPHERES:
							status += "Currently rendering a lot of spheres ";
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
					if (ImGui::Button("Two Spheres")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::TWO_SPHERE);
					}
					if (ImGui::Button("Lots of Spheres")) {
						setAlgorithm(algorithm);
						setScene(Rhodochrosite::SceneName::LARGE_AMOUNT_OF_SPHERES);
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

void uploadSphere(const Rhodochrosite::Sphere& sphere) {
	Ruby::ShaderProgram::upload("sphere.origin", sphere.origin);
	Ruby::ShaderProgram::upload("sphere.radius", sphere.radius);
	Ruby::ShaderProgram::upload("sphere.colour", sphere.colour.colour);
}

void uploadLight(const Ruby::DirectionalLight& light) {
	Ruby::ShaderProgram::upload("dirLight.direction", light.direction);
}

void uploadScene(const Rhodochrosite::Scene& scene) {
	for (const Rhodochrosite::Sphere& sphere : scene.spheres) {
		uploadSphere(sphere);
	}

	for (const Ruby::DirectionalLight& light : scene.lights) {
		uploadLight(light);
	}
}

void setScene(const Rhodochrosite::SceneName newScene) {
	scene = newScene;
	switch (scene) {
	case Rhodochrosite::SceneName::ONE_SPHERE:
		// CPU side
		rayTracer->setScene(oneSphereScene);

		// GPU side
		activeProgram->use();
		uploadScene(oneSphereScene);
		break;
	//case Rhodochrosite::Scene::TWO_SPHERE:
	//	
	//	break;
	//case Rhodochrosite::Scene::LARGE_AMOUNT_OF_SPHERES:
	//	
	//	break;
	}
}

void setAlgorithm(Rhodochrosite::RenderingAlgorithm newAlgorithm) {
	algorithm = newAlgorithm;
	switch (algorithm) {
	case Rhodochrosite::RenderingAlgorithm::BASIC_LIGHTING:
		Ruby::VertexShader vert{ Ruby::TextFile{"assets\\shaders\\RayTracing.vert"} };
		Ruby::FragmentShader frag{ Ruby::TextFile{"assets\\shaders\\RayTracing.frag"} };

		activeProgram = std::make_unique<Ruby::ShaderProgram>(vert, frag, std::vector<Ruby::Attribute>{ 3, 2 });
		break;
	/*case Rhodochrosite::RenderingAlgorithm::ALL_REFLECTIVE:
		break;
	case Rhodochrosite::RenderingAlgorithm::RANDOM_MATERIALS:
		break;
	case Rhodochrosite::RenderingAlgorithm::ALL_DIFFUSE:
		break;*/
	}
}