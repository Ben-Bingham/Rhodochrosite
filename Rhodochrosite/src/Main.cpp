#include "Camera.h"
#include "Window.h"
#include "Timing.h"
#include "Renderer.h"
#include "Rendering/Renderer.h"
#include "Renderable Objects/ScreenQuad/ScreenQuad.h"

int main() {
	Wavellite::Window window{Wavellite::Window::WindowSize::HALF_SCREEN, "Rhodochrosite"};
	const Wavellite::Keyboard* keyboard = window.ioManger.getKeyboard();

	Ruby::Renderer renderer{};
	Ruby::Camera cam{};
	Wavellite::Time time{};

	Ruby::Camera camera{};
	Rhodochrosite::Renderer rayTracer{ window.getWidth(), window.getHeight(), camera };

	Rhodochrosite::Sphere sphere{ Malachite::Vector3f{0.0f, 0.0f, -5.0f }, 0.5f, Ruby::Colour::yellow };
	rayTracer.addSphere(sphere);

	Rhodochrosite::Sphere sphere1{ Malachite::Vector3f{0.5f, 0.0f, -4.0f }, 0.25f, Ruby::Colour{3, 111, 252} };
	rayTracer.addSphere(sphere1);

	Ruby::Texture renderTarget{ rayTracer.getImage()};
	Ruby::ScreenQuad screenQuad{ &renderTarget };

	renderer.init(window.getProjectionMatrix());

	while (window.isOpen()) {
		if (keyboard->KEY_ESCAPE) {
			window.close();
		}

		{ // Ray tracing
			rayTracer.render();
			renderTarget.updateData();
		}

		{ // Camera Movement
			const float velocity = 5.0f * time.deltaTime;
			if (keyboard->KEY_W) { camera.position = camera.position + (velocity * Malachite::Vector3f::north); }
			if (keyboard->KEY_S) { camera.position = camera.position + (velocity * Malachite::Vector3f::south); }
			if (keyboard->KEY_A) { camera.position = camera.position + (velocity * Malachite::Vector3f::west); }
			if (keyboard->KEY_D) { camera.position = camera.position + (velocity * Malachite::Vector3f::east); }
			if (keyboard->KEY_SPACE) { camera.position = camera.position + (velocity * Malachite::Vector3f::up); }
			if (keyboard->KEY_LEFT_SHIFT) { camera.position = camera.position + (velocity * Malachite::Vector3f::down); }
		}

		{ // Rendering
			renderer.prep(cam.getViewMatrix());
			{
				renderer.screenQuadRenderingPrep();

				renderer.screenQuadRender(screenQuad);

				renderer.screenQuadRenderingEnd();
			}
			{
				renderer.imGuiPrep();

				{
					ImGui::Begin("Settings");

					if(ImGui::Button("Render")) {
						rayTracer.render();
						renderTarget.updateData();
					}
					//float fov = camera.getFov();
					//ImGui::SliderFloat("FOV", &fov, 1.0f, 90.0f);
					//camera.setFov(fov);

					LOG(camera.position.toString());

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