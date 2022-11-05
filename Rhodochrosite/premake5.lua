project "Rhodochrosite"
	kind "ConsoleApp"
	language "C++"

	cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/%{prj.name}")
	objdir ("%{wks.location}/build/bin-int/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp"
	}

	includedirs {
		"src",
		-- Vendor
		"%{wks.location}/Dependencies/Gemstone/vendor/GLFW/include",
		"%{wks.location}/Dependencies/Gemstone/vendor/glew-2.1.0/include",
		"%{wks.location}/Dependencies/Gemstone/vendor/stb_image",
		-- Dependencies
		"%{wks.location}/Dependencies/Gemstone/Malachite/src",
		"%{wks.location}/Dependencies/Gemstone/Ruby/src",
		"%{wks.location}/Dependencies/Gemstone/Lazuli/src"
	}

	libdirs {
		"%{wks.location}/Dependencies/Gemstone/vendor/GLFW/lib-vc2022",
		"%{wks.location}/Dependencies/Gemstone/vendor/glew-2.1.0/lib/Release/x64"
	}

	links {
		-- Vendor
		"glfw3",
		"glew32s",
		"opengl32",
		-- Dependencies
		"Malachite",
		"Ruby",
		"Lazuli"
	}