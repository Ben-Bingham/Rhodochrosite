#include "Camera.h"
#include "Window.h"
#include "Timing.h"
#include "Renderer.h"
#include "Rendering/Renderer.h"
#include "Renderable Objects/ScreenQuad/ScreenQuad.h"

int main() {
	Wavellite::Window window{Wavellite::Window::WindowSize::HALF_SCREEN};
	const Wavellite::Keyboard* keyboard = window.ioManger.getKeyboard();

	Ruby::Renderer renderer{};
	Ruby::Camera cam{};
	Wavellite::Time time{};

	Rhodochrosite::Renderer rayTracer{ window.getWidth(), window.getHeight() };

	Rhodochrosite::Sphere sphere{ Malachite::Vector3f{0.0f, 0.0f, -5.0f }, 0.5f, Ruby::Colour::black };
	rayTracer.addSphere(sphere);

	Rhodochrosite::Sphere sphere1{ Malachite::Vector3f{0.5f, 0.0f, -4.0f }, 0.25f, Ruby::Colour::pink };
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