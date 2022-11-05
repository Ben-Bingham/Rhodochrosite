#include "Window.h"

#include "Renderer.h"

#include "Renderable Objects/Solid/SolidGeometry.h"
#include "Geometry/CubeGeometry.h"

int main() {
	Wavellite::Window window{ Wavellite::Window::WindowSize::HALF_SCREEN };
	const Wavellite::Keyboard* keyboard = &window.ioManger.keyboard;

	Ruby::Renderer renderer{};
	Ruby::Camera cam{};
	Ruby::SolidMaterial mat{ Malachite::Vector3f{1.0f, 0.0f, 0.0f} };
	Ruby::SolidGeometry cube{ std::make_unique<Ruby::CubeGeometry>(), mat };
	cube.model.translate(0.0f, 0.0f, 5.0f);

	while (window.isOpen())
	{
		if (keyboard->KEY_ESCAPE)
		{
			window.close();
		}

		{ // Rendering
			renderer.prep(cam.getViewMatrix());
			{
				renderer.solidRenderingPrep();

				renderer.solidRender(cube);

				renderer.solidRenderingEnd();
			}
			renderer.end();
		}

		window.pollEvents();
		window.swapBuffers();
	}
}
