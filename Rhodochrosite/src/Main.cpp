#include "Camera.h"
#include "Window.h"
#include "Timing.h"
#include "Renderer.h"
#include "Rendering/Renderer.h"
#include "Renderable Objects/ScreenQuad/ScreenQuad.h"

int main() {
	Wavellite::Window window{Wavellite::Window::WindowSize::HALF_SCREEN, "Rhodochrosite"};
	window.setSwapInterval(0);
	const Wavellite::Keyboard* keyboard = window.ioManger.getKeyboard();

	Ruby::Renderer renderer{};
	Ruby::Camera cam{};
	Wavellite::Time time{};

	Ruby::Camera camera{};
	Rhodochrosite::Renderer rayTracer{ window.getWidth(), window.getHeight(), camera };

	Rhodochrosite::Sphere sphere{ Malachite::Vector3f{0.0f, 0.0f, -2.0f}, 0.5f, Ruby::Colour::pink };
	Ruby::DirectionalLight dirLight{ Malachite::Vector3f{-1.0f, -1.0f, -1.0f}.normalize() };

	rayTracer.setSphere(sphere);
	rayTracer.setDirectionalLight(dirLight);

	bool cpuRendering{ true };
	bool gpuRendering{ false };

	Ruby::Texture renderTarget{ rayTracer.getImage()};
	Ruby::ScreenQuad screenQuad{ &renderTarget };

	// GPU rendering
	Ruby::VertexShader rayTracingVertShader{ Ruby::TextFile{"assets\\shaders\\RayTracing.vert"} };
	Ruby::FragmentShader rayTracingFragShader{ Ruby::TextFile{"assets\\shaders\\RayTracing.frag"} };
	Ruby::ShaderProgram rayTracingShader{ rayTracingVertShader, rayTracingFragShader, std::vector<Ruby::Attribute>{ 3, 2 } };

	rayTracingShader.use();

	Ruby::ShaderProgram::upload("cameraPosition", camera.position);

	Ruby::ShaderProgram::upload("sphere.origin", sphere.origin);
	Ruby::ShaderProgram::upload("sphere.radius", sphere.radius);
	Ruby::ShaderProgram::upload("sphere.colour", sphere.colour.colour);

	Ruby::ShaderProgram::upload("dirLight.direction", dirLight.direction);

	renderer.addShader(rayTracingShader);

	renderer.init(window.getProjectionMatrix());

	while (window.isOpen()) {
		if (keyboard->KEY_ESCAPE) {
			window.close();
		}

		{ // Ray tracing
			if (cpuRendering) {
				rayTracer.render();
				renderTarget.updateData();
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
			renderer.prep(cam.getViewMatrix());
			{
				if (cpuRendering) {
					renderer.screenQuadRenderingPrep();

					renderer.screenQuadRender(screenQuad);

					renderer.screenQuadRenderingEnd();
				}
			}

			{
				if (gpuRendering) {
					rayTracingShader.use();
					Ruby::ShaderProgram::upload("aspectRatio", (float)window.getHeight() / (float)window.getWidth());

					screenQuad.render();
				}
			}

			{
				renderer.imGuiPrep();

				{
					ImGui::Begin("Settings");

					if(ImGui::Button("Render")) {
						rayTracer.render();
						renderTarget.updateData();
					}

					if (ImGui::Button("CPU Rendering")) {
						cpuRendering = true;
						gpuRendering = false;
					}
					if (ImGui::Button("GPU Rendering")) {
						cpuRendering = false;
						gpuRendering = true;
					}

					if (cpuRendering) {
						ImGui::Text("Currently rendering on the CPU");
					}
					else {
						ImGui::Text("Currently rendering on the GPU");
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